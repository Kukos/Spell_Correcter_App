#include <word.h>
#include <cstring.h>
#include <log.h>
#include <stdlib.h>
#include <common.h>

Word *word_create(const String *str, long occurrence, int ld)
{
    Word *w;

    TRACE();

    w = malloc(sizeof(Word));
    if (w == NULL)
        ERROR("malloc error\n", NULL);

    w->____str = (String *)str;
    w->____occurrence = occurrence;
    w->____ld_dist = ld;

    return w;
}

void word_destroy(Word *word)
{
    TRACE();

    if (word == NULL)
        return;

    string_destroy(word->____str);
    FREE(word);
}

String *word_get_string(const Word *word)
{
    TRACE();

    if (word == NULL)
        return NULL;

    return word->____str;
}

long word_get_occurrence(const Word *word)
{
    TRACE();

    if (word == NULL)
        return -1;

    return word->____occurrence;
}

int word_get_ld_dist_turn(const Word *word)
{
    TRACE();

    if (word == NULL)
        return -1;

    return word->____ld_dist;
}

void word_set_string(Word *word, const String *str)
{
    TRACE();

    if (word == NULL)
        return;

    word->____str = (String *)str;
}

void word_set_occurrence(Word *word, long occ)
{
    TRACE();

    if (word == NULL)
        return;

    word->____occurrence = occ;
}

void word_set_ld_dist_turn(Word *word, int ld)
{
    TRACE();

    if (word == NULL)
        return;

    word->____ld_dist = ld;
}

int word_cmp_b(const Word *w1, const Word *w2)
{
    TRACE();

    if (w1 == NULL || w2 == NULL)
        return 0;

    return string_cmp(w1->____str, w2->____str);
}

int word_cmp_a(const Word *w1, const Word *w2)
{
    TRACE();

    if (w1 == NULL || w2 == NULL)
        return 0;

    /* the most important is ld distance wave */
    if (w1->____ld_dist < w2->____ld_dist)
        return -1;

    if (w1->____ld_dist > w2->____ld_dist)
        return 1;

    /* if ld are the same, next compare occurrences */
    if (w1->____occurrence < w2->____occurrence)
        return 1;

    if (w1->____occurrence > w2->____occurrence)
        return -1;

    /* if occurrences are the same, compare strings */
    return string_cmp(w1->____str, w2->____str);
}