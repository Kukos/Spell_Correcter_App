#ifndef SPELL_CORRECTOR_H
#define SPELL_CORRECTOR_H

/*
    Spell corrector use Damerau–Levenshtein distance (O(n * m))
    wchich has 4 operation:
    1. Insert 1 letter into any position
    2. Delete 1 letter from any position
    3. Swap 2 neighboring letters
    4. Replace 1 letter for another letter from Alphabet

    There are two basic idea to correct words:
    1.  for each word in dictionary:
            calc DL-dist
        Get k words with min DL-dist

        This idea is so costly for big dictionary,
        but big dicionary is advantage of this problem, because
        we can find better word to replace by mistaking word.
        So I don't want use this idea.

    2.  Let i = 1
        Generate all Words with DL-dist = i
        for each word from generated:
             Check word in dictionary, if exists add to Finally set
             If Finally Set size == k then return Finally set
        i = i + 1

        This idea is so costly for big words, but average word len = 8,
        so we need to generate: 210.000 words.
        (Please note that words will be similar, so memcpy dominating, so we can use SSE)

    In This implemntation after analysis of problem I choose IDEA 2.

    Dictionary can be implemented as AVL / Hash Table (C++ / Java/ C#) or TRIE
    The biggest performance for idea 2 (words are similar) has TRIE
    (of course we can use hardest in implementation but a little faster PATRICK's Tree)

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3
*/

#include <stddef.h>
#include <cstring.h>

/*
    Init spell_corrector (this is a preprocessing step)

    PARAMS
    @IN path - path to dictionary with format (word;occurences)

    RETURN
    0 iff success
    Non-zero value iff failure
*/
int spell_corrector_init(const char *path);

/*
    Release spell_corrector

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void spell_corrector_deinit(void);

/*
    Correct word @word, get at most @k words

    PARAMS
    @IN word - word to correct
    @IN k - num of maximum possible correction words
    @OUT size - real num of correction words

    INFO:
        You have to free whole array of Strings

    RETURN
    NULL iff failure
    Pointer to Array of String * iff success
*/
String **spell_corrector_correct_word(String *word, size_t k, size_t *size);

#endif