#include <spell_corrector.h>
#include <cstring.h>
#include <trie.h>
#include <darray.h>
#include <log.h>
#include <filebuffer.h>
#include <common.h>
#include <word.h>
#include <avl.h>

#define WORD_SET_INIT_SIZE  BIT(10)
#define WORD_MAX_SIZE       BIT(6)
#define __ALPHABET_SIZE     26

/*
    Insert generated words to set

    PARAMS
    @IN gen - darray with generated words
    @IN set - avl set with correct words
    @IN ld - ld turn

    RETURN
    This is a void function
*/
static ___inline___ void insert_generated_words(Darray *gen, Avl *set, int ld);

/*
    Insert word to set

    PARAMS
    @IN s - pointer to String to insert
    @IN set - avl set with correct words
    @IN ld - ld turn
    @IN temp - pointer to temp word (like buffer)

    RETURN
    This is a void function
*/
static ___inline___ void insert_word_to_set(String *s, Avl *set, int ld, Word *temp);

/* 
    Compare word in map

    PARAMS:
    @IN a - first word
    @IN b - second word

    RETURN
    0 iff eq
    -1 iff a < b
    1 iff a > b
 */
static int word_map_cmp(const void *a, const void *b);

/* 
    Compare word in word set

    PARAMS:
    @IN a - first word
    @IN b - second word

    RETURN
    0 iff eq
    -1 iff a < b
    1 iff a > b
 */
static int word_set_cmp(const void *a, const void *b);

/*
    Destroy word map

    PARAMS
    @IN a - word map

    RETURN
    This is a void function
*/
static void word_map_destructor(void *a);

/*
    Destroy string

    PARAMS
    @IN str - String**

    RETURN
    This is a void function

*/
static void __string_destroy(void *str);

static Trie *dictionary;
static Avl  *word_map;

static int word_map_cmp(const void *a, const void *b)
{
    Word *__a = *(Word **)a;
    Word *__b = *(Word **)b;

    return word_cmp_b(__a, __b);
}

static int word_set_cmp(const void *a, const void *b)
{
    Word *__a = *(Word **)a;
    Word *__b = *(Word **)b;

    return word_cmp_a(__a, __b);
}

static void word_map_destructor(void *a)
{
    word_destroy(*(Word **)a);
}

static size_t calc_ld_dist_set_size(String *s)
{
    return ((__ALPHABET_SIZE + 1) << 1) * (size_t)string_get_length(s) + __ALPHABET_SIZE - 1;
}

static void __string_destroy(void *str)
{
    String *s = *(String **)str;
    string_destroy(s);
}

static ___inline___ void insert_word_to_set(String *s, Avl *set, int ld, Word *temp)
{
    Word *w;

    /* set temp string for searching */
    word_set_string(temp, s);

    /* search and get real word */
    (void)avl_search(word_map, (void *)&temp, (void *)&w);
            
    /* set ld turn and insert word to set */
    word_set_ld_dist_turn(w, ld);
    (void)avl_insert(set, (void *)&w);
}

static ___inline___ void insert_generated_words(Darray *gen, Avl *set, int ld)
{
    String *s;
    Word *temp;
    
    temp = word_create(NULL, 0, -1);
    if (temp == NULL)
    {
        LOG("Word create error\n");
        return;
    }

    LOG("Insert words from %d turn\n", ld);
    for_each_data(gen, Darray, s)
        if (trie_find(dictionary, string_get_str(s)))
        {
            LOG("Word %s is correct\n", string_get_str(s));
            insert_word_to_set(s, set, ld, temp);
            
        }
    LOG("Words inserted\n");

    word_set_string(temp, NULL);
    word_destroy(temp);
}

static Darray *generate_ld_dist(String *string)
{
    Darray *set;
    String *s;

    size_t i;
    size_t len;

    char c;

    TRACE();

    set = darray_create(DARRAY_UNSORTED, calc_ld_dist_set_size(string), sizeof(String *), NULL);
    if (set == NULL)
        ERROR("darray create error\n", NULL);

    len = (size_t)string_get_length(string);

    LOG("Create words by insert\n");
    /* INSERT */
    for (i = 0; i <= len; ++i)
        for (c = 'a'; c <= 'z'; ++c)
        {
            s = string_clone(string);
            (void)string_insert_c(s, c, i);
            (void)darray_insert(set, (void *)&s);
        }

    LOG("Create words by delete\n");
    /* DELETE */
    if (len > 1)
        for (i = 0; i < len; ++i)
        {
            s = string_clone(string);
            (void)string_delete(s, i, 1);
            (void)darray_insert(set, (void *)&s);
        }

    LOG("Create words by replace\n");
    /* REPLACE */
    for (i = 0; i < len; ++i)
        for (c = 'a'; c <= 'z'; ++c)
        {
            s = string_clone(string);
            (void)string_set_char(s, i, c);
            (void)darray_insert(set, (void *)&s);
        }

    LOG("Create words by swap\n");
    /* SWAP */
    for (i = 0; i < len - 1; ++i)
    {
        s = string_clone(string);
        c = string_get_char(s, i);
        (void)string_set_char(s, i, string_get_char(s, i + 1));
        (void)string_set_char(s, i + 1, c);
        (void)darray_insert(set, (void *)&s);
    }

    return set;
}

int spell_corrector_init(const char *path)
{
    File_buffer *fb;
    char *buf;
    char word[WORD_MAX_SIZE];
    ___unused___ char *word2;

    int len;
    long n;

    Word *w;

    TRACE();

    if (path == NULL)
        ERROR("path == NULL\n", 1);

    LOG("Initializating Spell Corrector ...\n");
    LOG("Init needed structures\n");

    dictionary = trie_create();
    if (dictionary == NULL)
        ERROR("trie create error\n", 1);

    word_map = avl_create(sizeof(Word *), word_map_cmp);
    if (word_map == NULL)
        ERROR("avl create error\n", 1);

    fb = file_buffer_create_from_path(path, PROT_READ | PROT_WRITE, O_RDWR);
    if (fb == NULL)
    {
        trie_destroy(dictionary);
        ERROR("file buffer create error\n", 1);
    }

    buf = file_buffer_get_buff(fb);

    LOG("READ Words from dictionary\n");
    while (sscanf(buf, "%[^;];%ld\n%n", word, &n, &len) != -1)
    {
        buf += len;
        if (trie_insert(dictionary, word))
        {
            file_buffer_destroy(fb);
            spell_corrector_deinit();

            ERROR("trie insert error\n", 1);
        }

        w = word_create(string_create_from_str(word), -1, -1);
        if (w == NULL)
        {
            file_buffer_destroy(fb);
            spell_corrector_deinit();

            ERROR("word_create error\n", 1);
        }

        if (avl_insert(word_map, (void *)&w))
        {
            file_buffer_destroy(fb);
            spell_corrector_deinit();

            ERROR("avl_insert error\n", 1);
        }

    }

    file_buffer_destroy(fb);
    LOG("Spell Corrector Inited, ready to use\n");
    return 0;
}

void spell_corrector_deinit(void)
{
    TRACE();

    LOG("Deinitializating Spell Corrector ...\n");

    trie_destroy(dictionary);
    avl_destroy_with_entries(word_map, word_map_destructor);

    LOG("Spell Corrector deinited\n");
    return;
}

String **spell_corrector_correct_word(String *word, size_t k, size_t *size)
{
    TRACE();

    Avl *set; /* set with correct words */
    Darray *gen1 = NULL; /* generated words in 1st turn */
    Darray *gen2 = NULL; /* generated words in 2nd turn */

    /* temporary pointers */
    String *s;
    Word *temp;
    Word *data;
    size_t i;

    String **res; /* finally set */

    if (word == NULL)
        ERROR("string == NULL\n", NULL);

    if (string_is_empty(word))
        ERROR("word is a empty string\n", NULL);

    if (k < 1)
        ERROR("k < 1\n", NULL);

    if (size == NULL)
        ERROR("size == NULL\n", NULL);

    set = avl_create(sizeof(Word *), word_set_cmp);
    if (set == NULL)
        ERROR("avl create error\n", NULL);
        
    temp = word_create(NULL, 0, 0);
    if (temp == NULL)
    {
        avl_destroy(set);
        ERROR("Word create error\n", NULL);
    }

    /* if word is correct insert to set */
    if (trie_find(dictionary, string_get_str(word)))
    {
        LOG("Word %s is correct\n", string_get_str(word));
        insert_word_to_set(word, set, 0, temp);
        word_set_string(temp, NULL);
    }
    word_destroy(temp);

    /* if after turn 0 we have enough words goto end */
    if ((size_t)avl_get_num_entries(set) >= k)
        goto result;

    LOG("Generate LD 1\n");
    gen1 = generate_ld_dist(word);
    LOG("LD 1 generated\n");

    insert_generated_words(gen1, set, 1);

    /* if after turn 1 we have enough words goto end */
    if ((size_t)avl_get_num_entries(set) >= k)
        goto result;

    LOG("Generate LD2\n");
    for_each_data(gen1, Darray, s)
    {
        if ((size_t)avl_get_num_entries(set) >= k)
            break;

        LOG("Generate LD2 From %s\n", string_get_str(s));
        gen2 = generate_ld_dist(s);
        LOG("LD 2 from %s generated\n", string_get_str(s));

        insert_generated_words(gen2, set, 2);
        darray_destroy_with_entries(gen2, __string_destroy);
    }

result:
    LOG("Create result\n");
    res = (String **)malloc(sizeof(String *) * k);
    if (res == NULL)
    {
        darray_destroy_with_entries(gen1, __string_destroy);
        avl_destroy(set); /* destroy set without entries, becasue include only entries from word map */

        ERROR("malloc error\n", NULL);
    }

    /* copy set to result */
    i = 0;
    for_each_data(set, Avl, data)
    {
        if (i >= k)
            break;

        LOG("Copy %s to result\n", string_get_str(word_get_string(data)));
        res[i] = string_clone(word_get_string(data));
        ++i;
    }
    *size = MIN(i, k);

    darray_destroy_with_entries(gen1, __string_destroy);
    avl_destroy(set); /* destroy set without entries, becasue include only entries from word map */

    return res;
}
