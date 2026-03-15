/*
 * Very bad implementation for a char* -> uint8_t hashmap
 */

#ifndef BAD_HASHTABLE_H
#define BAD_HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define UNICODE_MAX_SIZE 4

typedef struct {
    char unicode[UNICODE_MAX_SIZE];
    bool init;
} BadHashTableNode;
typedef struct { BadHashTableNode t[UINT8_MAX+1]; } BadHashTable_t;

static void bad_ht_initialize(BadHashTable_t *ht) {
    for (size_t i = 0; i < UINT8_MAX+1; ++i) {
        ht->t[i].init = false;
    }
}

static void bad_ht_insert(BadHashTable_t *ht, const char key[UNICODE_MAX_SIZE], uint8_t value) {
    strncpy(ht->t[value].unicode, key, UNICODE_MAX_SIZE);
    ht->t[value].init = true;
}

static bool bad_ht_delete(BadHashTable_t *ht, const char key[UNICODE_MAX_SIZE], uint8_t value) {
    // returns true if value could be deleted. returns false if value didn't exist in table
    if (!ht->t[value].init) {
        return false; // value "doesn't exist" in hash table
    }
    ht->t[value].init = false;
    return true;
}

static bool bad_ht_contains(BadHashTable_t *ht, uint8_t value) {
    return ht->t[value].init;
}

static bool bad_ht_get(BadHashTable_t *ht, const char key[UNICODE_MAX_SIZE], uint8_t *value) {
    // returns true if has key, false otherwise. If has key, the value associated is copied into value
    for (size_t i = 0; i < UINT8_MAX+1; ++i) {
        if (ht->t[i].init && strncmp(ht->t[i].unicode, key, UNICODE_MAX_SIZE) == 0) {
            *value = i;
            return true;
        }
    }
    return false;
}


#endif /* BAD_HASHTABLE_H */
