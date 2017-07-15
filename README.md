# Spell_Corrector_App
Spell Corrector Console Application in C

### Description

Spell corrector use Damerau–Levenshtein distance (O(n * m)) wchich has 4 operation:
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


2.  
    Generate all Words with DL-dist = 1

    for each word from generated:

        Check word in dictionary, if exists add to Finally set
        If Finally Set size == k then return Finally set


    Generate all Words with DL-dist = 2

    for each word from generated:

        Check word in dictionary, if exists add to Finally set
        If Finally Set size == k then return Finally set


    This idea is so costly for big words, but average word len = 8,
    so we need to generate: 210.000 words.
    Please note that words will be similar, so memcpy is dominating operation.
    We we can use SSE to minimalize this cost.


In This implemntation after analysis I choose IDEA 2.


Dictionary can be implemented as AVL / Hash Table (C++ / Java/ C#) or TRIE
The biggest performance for idea 2 (words are similar) has TRIE
of course we can use hardest in implementation but a little faster PATRICK's Tree)


### Build
Just make

### Usage
./spell_corrector [max correct words]

on stdout put word and enter

To exit put ctr+D