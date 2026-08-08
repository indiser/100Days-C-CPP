#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#define SYMBOL_LENGTH 8
#define INITIAL_CAPACITY 16   // must be power of 2
#define MAX_LOAD 0.7

typedef enum { EMPTY, OCCUPIED, DELETED } SlotState;

typedef struct
{
    char symbol[SYMBOL_LENGTH];
    double price;
    SlotState state;
} Slot;

typedef struct
{
    Slot *slots;
    size_t capacity;   // power of 2
    size_t count;       // occupied only
    size_t tombstones;  // deleted count
} SymbolTable;

static SymbolTable g_table;

unsigned int hashFunc(const char *str)
{
    uint32_t hash = 0x811C9DC5;
    const uint32_t prime = 0x01000193;

    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= prime;
    }
    return hash;
}

static bool table_alloc(SymbolTable *t, size_t capacity)
{
    t->slots = calloc(capacity, sizeof(Slot));  // EMPTY == 0
    if (!t->slots) return false;
    t->capacity = capacity;
    t->count = 0;
    t->tombstones = 0;
    return true;
}

void symbol_table_create(void)
{
    table_alloc(&g_table, INITIAL_CAPACITY);
}

static bool table_insert_raw(Slot *slots, size_t capacity, const char *symbol, double price)
{
    unsigned int mask = capacity - 1;
    unsigned int index = hashFunc(symbol) & mask;
    unsigned int start = index;
    long first_tombstone = -1;

    for (;;)
    {
        Slot *s = &slots[index];

        if (s->state == EMPTY)
        {
            size_t dest = (first_tombstone != -1) ? (size_t)first_tombstone : index;
            Slot *d = &slots[dest];
            strncpy(d->symbol, symbol, SYMBOL_LENGTH - 1);
            d->symbol[SYMBOL_LENGTH - 1] = '\0';
            d->price = price;
            d->state = OCCUPIED;
            return true;
        }

        if (s->state == DELETED)
        {
            if (first_tombstone == -1) first_tombstone = index;
        }
        else if (strncmp(s->symbol, symbol, SYMBOL_LENGTH) == 0)
        {
            s->price = price;
            return true;
        }

        index = (index + 1) & mask;
        if (index == start) return false; // full, shouldn't happen if resize works
    }
}

static bool table_resize(size_t new_capacity)
{
    Slot *old_slots = g_table.slots;
    size_t old_capacity = g_table.capacity;

    Slot *new_slots = calloc(new_capacity, sizeof(Slot));
    if (!new_slots) return false;

    for (size_t i = 0; i < old_capacity; i++)
    {
        if (old_slots[i].state == OCCUPIED)
        {
            table_insert_raw(new_slots, new_capacity, old_slots[i].symbol, old_slots[i].price);
        }
    }

    free(old_slots);
    g_table.slots = new_slots;
    g_table.capacity = new_capacity;
    g_table.tombstones = 0; // tombstones don't carry over, rebuilt clean
    return true;
}

bool symbol_table_insert(const char *symbol, double price)
{
    // resize if load (occupied+tombstones) crosses threshold
    double load = (double)(g_table.count + g_table.tombstones + 1) / g_table.capacity;
    if (load > MAX_LOAD)
    {
        if (!table_resize(g_table.capacity * 2)) return false;
    }

    unsigned int mask = g_table.capacity - 1;
    unsigned int index = hashFunc(symbol) & mask;
    unsigned int start = index;
    long first_tombstone = -1;

    for (;;)
    {
        Slot *s = &g_table.slots[index];

        if (s->state == EMPTY)
        {
            size_t dest = (first_tombstone != -1) ? (size_t)first_tombstone : index;
            Slot *d = &g_table.slots[dest];
            bool was_tombstone = (d->state == DELETED);
            strncpy(d->symbol, symbol, SYMBOL_LENGTH - 1);
            d->symbol[SYMBOL_LENGTH - 1] = '\0';
            d->price = price;
            d->state = OCCUPIED;
            g_table.count++;
            if (was_tombstone) g_table.tombstones--;
            return true;
        }

        if (s->state == DELETED)
        {
            if (first_tombstone == -1) first_tombstone = index;
        }
        else if (strncmp(s->symbol, symbol, SYMBOL_LENGTH) == 0)
        {
            s->price = price; // update existing
            return true;
        }

        index = (index + 1) & mask;
        if (index == start) return false; // full, shouldn't hit given resize
    }
}

bool symbol_table_lookup(const char *symbol, double *out_price)
{
    unsigned int mask = g_table.capacity - 1;
    unsigned int index = hashFunc(symbol) & mask;
    unsigned int start = index;

    for (;;)
    {
        Slot *s = &g_table.slots[index];

        if (s->state == EMPTY) return false; // empty slot -> key not present

        if (s->state == OCCUPIED && strncmp(s->symbol, symbol, SYMBOL_LENGTH) == 0)
        {
            if (out_price) *out_price = s->price;
            return true;
        }

        index = (index + 1) & mask;
        if (index == start) return false;
    }
}

bool symbol_table_delete(const char *symbol)
{
    unsigned int mask = g_table.capacity - 1;
    unsigned int index = hashFunc(symbol) & mask;
    unsigned int start = index;

    for (;;)
    {
        Slot *s = &g_table.slots[index];

        if (s->state == EMPTY) return false;

        if (s->state == OCCUPIED && strncmp(s->symbol, symbol, SYMBOL_LENGTH) == 0)
        {
            s->state = DELETED;
            g_table.count--;
            g_table.tombstones++;
            return true;
        }

        index = (index + 1) & mask;
        if (index == start) return false;
    }
}

void symbol_table_destroy(void)
{
    free(g_table.slots);
    g_table.slots = NULL;
    g_table.capacity = 0;
    g_table.count = 0;
    g_table.tombstones = 0;
}


int main(void)
{
    symbol_table_create();

    symbol_table_insert("AAPL", 182.50);
    symbol_table_insert("NVDA", 721.33);
    symbol_table_insert("TSLA", 193.57);

    symbol_table_insert("AAPL", 185.00); // update

    double price = 0.0;
    if (symbol_table_lookup("AAPL", &price))
        printf("AAPL price: %.2f\n", price);
    else
        printf("AAPL not found\n");

    if (symbol_table_lookup("NVDA", &price))
        printf("NVDA price: %.2f\n", price);

    if (!symbol_table_lookup("MSFT", &price))
        printf("MSFT not found\n");

    symbol_table_delete("NVDA");
    if (!symbol_table_lookup("NVDA", &price))
        printf("NVDA deleted, not found\n");

    // force resize, prove it survives
    for (int i = 0; i < 50; i++)
    {
        char sym[SYMBOL_LENGTH];
        snprintf(sym, sizeof(sym), "S%d", i);
        symbol_table_insert(sym, i * 1.5);
    }
    printf("capacity after growth: %zu, count: %zu\n", g_table.capacity, g_table.count);

    if (symbol_table_lookup("AAPL", &price))
        printf("AAPL still found after resize: %.2f\n", price);

    symbol_table_destroy();
    return 0;
}