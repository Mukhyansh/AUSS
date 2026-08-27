#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX 256

typedef struct trie {
    uint32_t value;
    struct trie* children[MAX];
    bool isleaf;
} trie;


trie* maketrie(void);

void freenode(trie* node);

void insertchar(trie* root, unsigned char ch, uint32_t val);

bool searchchar(trie* root, unsigned char ch, trie** node);

#endif