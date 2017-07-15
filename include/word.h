#ifndef WORD_H
#define WORD_H

/*
    This file contains definision for Word needed in spell_corrector

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3

*/

#include <cstring.h>

typedef struct Word
{
    String *____str;
    long    ____occurrence;
    int     ____ld_dist;

}Word;


/*
    Create new Word

    PARAMS
    @IN str - pointer to String
    @IN occurrence - occurrence in dictionary
    #IN ld - turn wchich word was find

    RETURN
    NULL iff failure
    Pointer to new Word iff success
*/
Word *word_create(const String *str, long occurrence, int ld);

/*
    Destroy Word

    PARAMS
    @IN word - pointer to Word

    RETURN
    This is a void function
*/
void word_destroy(Word *word);

/*
    Getters

    PARAMS
    @IN word - pointer to Word

    RETURN
    Word member iff success
    NULL / -1 iff failure
*/
String *word_get_string(const Word *word);
long word_get_occurrence(const Word *word);
int word_get_ld_dist_turn(const Word *word);

/*
    Setters

    PARAMS
    @IN word - pointer to Word
    @IN ____ - Word member

    RETURN
    There are void functions
*/
void word_set_string(Word *word, const String *str);
void word_set_occurrence(Word *word, long occ);
void word_set_ld_dist_turn(Word *word, int ld);

/*
    Basic comparator
    Compare words using only strings

    PARAMS
    @IN w1 - pointer to first Word
    @IN w2 - pointer to second Word

    RETURN
    < 0 iff w1 < w2
    = 0 iff w1 == w2
    > 0 iff w1 > w2
*/
int word_cmp_b(const Word *w1, const Word *w2);

/*
    Advantace comparator
    Compare words using all members

    PARAMS
    @IN w1 - pointer to first Word
    @IN w2 - pointer to second Word

    RETURN
    < 0 iff w1 < w2
    = 0 iff w1 == w2
    > 0 iff w1 > w2
*/
int word_cmp_a(const Word *w1, const Word *w2);

#endif