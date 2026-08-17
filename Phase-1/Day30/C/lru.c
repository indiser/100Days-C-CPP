#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 3      // cache capacity
#define TABLE_SIZE 16    // hash table buckets

typedef struct Node {
    int key, value;
    struct Node *prev, *next;   // LRU list links
    struct Node *hnext;         // hash bucket chain link
} Node;

typedef struct LRUCache {
    Node *head, *tail;   // LRU list (head = MRU, tail = LRU)
    Node *table[TABLE_SIZE];
    int size, capacity;
} LRUCache;

unsigned hash(int key) {
    return ((unsigned)key * 2654435761u) % TABLE_SIZE;
}

LRUCache* createCache(int capacity) {
    LRUCache *c = calloc(1, sizeof(LRUCache));
    c->capacity = capacity;
    return c;
}

Node* tableFind(LRUCache *c, int key) {
    Node *n = c->table[hash(key)];
    while (n) {
        if (n->key == key) return n;
        n = n->hnext;
    }
    return NULL;
}

void tableInsert(LRUCache *c, Node *node) {
    unsigned h = hash(node->key);
    node->hnext = c->table[h];
    c->table[h] = node;
}

void tableRemove(LRUCache *c, int key) {
    unsigned h = hash(key);
    Node **pp = &c->table[h];
    while (*pp) {
        if ((*pp)->key == key) { *pp = (*pp)->hnext; return; }
        pp = &(*pp)->hnext;
    }
}

void listAddFront(LRUCache *c, Node *n) {
    n->prev = NULL;
    n->next = c->head;
    if (c->head) c->head->prev = n;
    c->head = n;
    if (!c->tail) c->tail = n;
}

void listUnlink(LRUCache *c, Node *n) {
    if (n->prev) n->prev->next = n->next; else c->head = n->next;
    if (n->next) n->next->prev = n->prev; else c->tail = n->prev;
}

void moveToFront(LRUCache *c, Node *n) {
    if (c->head == n) return;
    listUnlink(c, n);
    listAddFront(c, n);
}

void evictLRU(LRUCache *c) {
    Node *victim = c->tail;
    if (!victim) return;
    printf("Evicting key %d\n", victim->key);
    listUnlink(c, victim);
    tableRemove(c, victim->key);
    free(victim);
    c->size--;
}

void put(LRUCache *c, int key, int value) {
    Node *n = tableFind(c, key);
    if (n) {
        n->value = value;
        moveToFront(c, n);
        printf("Updated key %d -> %d\n", key, value);
        return;
    }
    if (c->size == c->capacity) evictLRU(c);

    n = malloc(sizeof(Node));
    n->key = key; n->value = value; n->hnext = NULL;
    listAddFront(c, n);
    tableInsert(c, n);
    c->size++;
    printf("Inserted key %d -> %d\n", key, value);
}

int get(LRUCache *c, int key) {
    Node *n = tableFind(c, key);
    if (!n) { printf("Get %d -> not found\n", key); return -1; }
    moveToFront(c, n);
    printf("Get %d -> %d\n", key, n->value);
    return n->value;
}

void freeCache(LRUCache *c) {
    Node *n = c->head;
    while (n) { Node *next = n->next; free(n); n = next; }
    free(c);
}

int main() {
    LRUCache *cache = createCache(CAPACITY);
    put(cache, 1, 10);
    put(cache, 2, 20);
    put(cache, 3, 30);
    get(cache, 1);
    put(cache, 4, 40);   // evicts key 2
    get(cache, 2);
    get(cache, 3);
    freeCache(cache);
    return 0;
}