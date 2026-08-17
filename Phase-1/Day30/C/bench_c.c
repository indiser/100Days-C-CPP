#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- C LRU CACHE IMPLEMENTATION ---

#define TABLE_SIZE 16384  // Fast power-of-two bucket count

typedef struct Node {
    int key, value;
    struct Node *prev, *next;
    struct Node *hnext;
} Node;

typedef struct LRUCache {
    Node *head, *tail;
    Node **table;
    int size, capacity;
} LRUCache;

static inline unsigned hash_func(int key) {
    return ((unsigned)key * 2654435761u) & (TABLE_SIZE - 1);
}

LRUCache* createCache(int capacity) {
    LRUCache *c = (LRUCache*)calloc(1, sizeof(LRUCache));
    c->capacity = capacity;
    c->table = (Node**)calloc(TABLE_SIZE, sizeof(Node*));
    return c;
}

static inline Node* tableFind(LRUCache *c, int key) {
    Node *n = c->table[hash_func(key)];
    while (n) {
        if (n->key == key) return n;
        n = n->hnext;
    }
    return NULL;
}

static inline void tableInsert(LRUCache *c, Node *node) {
    unsigned h = hash_func(node->key);
    node->hnext = c->table[h];
    c->table[h] = node;
}

static inline void tableRemove(LRUCache *c, int key) {
    unsigned h = hash_func(key);
    Node **pp = &c->table[h];
    while (*pp) {
        if ((*pp)->key == key) { 
            *pp = (*pp)->hnext; 
            return; 
        }
        pp = &(*pp)->hnext;
    }
}

static inline void listAddFront(LRUCache *c, Node *n) {
    n->prev = NULL;
    n->next = c->head;
    if (c->head) c->head->prev = n;
    c->head = n;
    if (!c->tail) c->tail = n;
}

static inline void listUnlink(LRUCache *c, Node *n) {
    if (n->prev) n->prev->next = n->next; else c->head = n->next;
    if (n->next) n->next->prev = n->prev; else c->tail = n->prev;
}

static inline void moveToFront(LRUCache *c, Node *n) {
    if (c->head == n) return;
    listUnlink(c, n);
    listAddFront(c, n);
}

static inline void evictLRU(LRUCache *c) {
    Node *victim = c->tail;
    if (!victim) return;
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
        return;
    }
    if (c->size == c->capacity) evictLRU(c);

    n = (Node*)malloc(sizeof(Node));
    n->key = key; 
    n->value = value; 
    n->hnext = NULL;
    listAddFront(c, n);
    tableInsert(c, n);
    c->size++;
}

int get(LRUCache *c, int key) {
    Node *n = tableFind(c, key);
    if (!n) return -1;
    moveToFront(c, n);
    return n->value;
}

void freeCache(LRUCache *c) {
    Node *n = c->head;
    while (n) { 
        Node *next = n->next; 
        free(n); 
        n = next; 
    }
    free(c->table);
    free(c);
}

// --- BENCHMARK DRIVER ---

int main() {
    const int N = 1000000;
    const int CAP = 10000;
    const int KEY_RANGE = 8000; // Warm cache setup

    LRUCache *cache = createCache(CAP);

    // Warm-up phase
    for (int i = 0; i < KEY_RANGE; ++i) {
        put(cache, i, i * 2);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < N; ++i) {
        int k = i % KEY_RANGE;
        put(cache, k, i);
        get(cache, k);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double total_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double total_ops = N * 2.0;
    double mean_lat_ns = (total_sec * 1e9) / total_ops;
    double ops_sec = total_ops / total_sec;

    printf("C Benchmark Results:\n");
    printf("  Total Ops:    %.0f\n", total_ops);
    printf("  Mean Latency: %.2f ns/op\n", mean_lat_ns);
    printf("  Throughput:   %.2f M ops/sec\n", ops_sec / 1e6);

    freeCache(cache);
    return 0;
}