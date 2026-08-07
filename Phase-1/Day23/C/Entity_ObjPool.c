#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<assert.h>
#include<stddef.h>


#define NUM_ALLOCTION  1028

typedef struct
{
    uint64_t x;
    uint64_t y;
    uint64_t z;
    unsigned int velocity;
    unsigned int health;
    bool flag;
}Entity;

typedef struct PoolNode
{
    union
    {
        Entity entity;
        struct PoolNode *next;
    };
    bool in_use;
}PoolNode;

PoolNode object_pool[NUM_ALLOCTION] = {0};
PoolNode *freelist = NULL;

__attribute__((constructor)) void Entity_Init()
{
    for (int i = 0; i < NUM_ALLOCTION - 1; i++)
    {
        object_pool[i].next = &(object_pool[i+1]);
        object_pool[i].in_use = false;
    }
    object_pool[NUM_ALLOCTION - 1].next = NULL;
    object_pool[NUM_ALLOCTION - 1].in_use = false;
    freelist = &(object_pool[0]);
}

Entity *Entity_Pool_Alloc(void)
{
    if(freelist)
    {
        PoolNode *node = freelist;
        freelist = freelist->next;
        node->in_use = true;
        return &(node->entity);
    }
    return NULL;
}

bool Entity_Free(Entity *entity)
{
    if (entity == NULL)
        return false;

    uintptr_t base = (uintptr_t)object_pool;
    uintptr_t p = (uintptr_t)entity;

    /* bounds check, replaces assert */
    if (p < base || p >= base + sizeof(object_pool))
    {
        fprintf(stderr, "Entity_Free: pointer outside pool\n");
        return false;
    }

    unsigned int i = (p - base) / sizeof(PoolNode);
    PoolNode *pool = &(object_pool[i]);

    /* offset check, replaces assert */
    if (&(pool->entity) != entity)
    {
        fprintf(stderr, "Entity_Free: misaligned pointer\n");
        return false;
    }

    /* double-free guard */
    if (!pool->in_use)
    {
        fprintf(stderr, "Entity_Free: double free detected\n");
        return false;
    }

    pool->in_use = false;
    pool->next = freelist;
    freelist = pool;
    return true;
}

void Pool_Reset(void)
{
    for (int i = 0; i < NUM_ALLOCTION - 1; i++)
    {
        object_pool[i].next = &(object_pool[i+1]);
        object_pool[i].in_use = false;
    }
    object_pool[NUM_ALLOCTION - 1].next = NULL;
    object_pool[NUM_ALLOCTION - 1].in_use = false;
    freelist = &(object_pool[0]);
}

int main()
{
    Entity *e1 = Entity_Pool_Alloc();
    Entity *e2 = Entity_Pool_Alloc();
    assert(e1 != NULL);
    assert(e2 != NULL);

    e1->x = 10;
    e1->health = 100;

    Entity_Free(e1);
    Entity_Free(e2);

    /* double-free test */
    assert(Entity_Free(e1) == false);

    /* exhaustion test */
    Entity *arr[NUM_ALLOCTION];
    for (int i = 0; i < NUM_ALLOCTION; i++)
        arr[i] = Entity_Pool_Alloc();
    assert(Entity_Pool_Alloc() == NULL); /* pool exhausted */
    for (int i = 0; i < NUM_ALLOCTION; i++)
        Entity_Free(arr[i]);

    Pool_Reset();

    return 0;
}