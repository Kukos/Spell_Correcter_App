#include <log.h>
#include <common.h>
#include <compiler.h>
#include <spell_corrector.h>
#include <stdlib.h>
#include <cstring.h>
#include <stdio.h>
#include <sys/time.h>

___before_main___(0) void init(void);
___after_main___(0) void deinit(void);

___before_main___(0) void init(void)
{
    (void)log_init(NULL, NO_LOG_TO_FILE);
}

___after_main___(0) void deinit(void)
{
    log_deinit();
}

static ___inline___ double getrealtime(const struct timeval *start, const struct timeval *end)
{
    return (double)end->tv_sec - (double)start->tv_sec + (((double)end->tv_usec - (double)start->tv_usec) / (double)1000000);
}

int main(int argc, char **argv)
{
    size_t k = 5;
    size_t size;
    size_t i;
    char word[1024];
    String **words;
    String *string;

    struct timeval start, end;
    
    if (argc > 1)
    {
        k = (size_t)atol(argv[1]);
        if (k == 0 && argv[1][0] != '0')
        {
            (void)fprintf(stderr, "Number of correct words must be int\n");
            return 1;
        }
        if (k < 1)
        {
            (void)fprintf(stderr, "Number of correct words must be >= 1\n");
            return 1;
        }
    }

    spell_corrector_init("./data/dictionary.txt");
    while (scanf("%s", word) > 0)
    {
        string = string_create_from_str(word);
        if (string == NULL)
        {
            (void)fprintf(stderr, "String create error\n");
            return 1;
        }
        
        if (gettimeofday(&start, NULL) == -1)
        {
            (void)fprintf(stderr, "Get time error\n");
            return 1;
        }

        words = spell_corrector_correct_word(string, k, &size);
        if (words == NULL)
        {
            (void)fprintf(stderr, "Correct words error\n");
            return 1;
        }

        if (gettimeofday(&end, NULL) == -1)
        {
            (void)fprintf(stderr, "Get time error\n");
            return 1;
        }

        (void)printf("TIME = %lf\n", getrealtime(&start, &end));

        (void)printf("WORDS:\n");
        if (size == 0)
            (void)printf("EMPTY\n");
        else
            for (i = 0; i < size; ++i)
            {
                (void)printf("%s\n", string_get_str(words[i]));
                string_destroy(words[i]);
            }

        FREE(words);
        string_destroy(string);
    }

    spell_corrector_deinit();
    return 0;
}