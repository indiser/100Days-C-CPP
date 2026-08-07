#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<assert.h>

#define NUM_SLOTS 100000
#define NUM_ROUNDS 100

typedef struct {
    int x, y, z;
}Vector3D;

typedef struct PoolObject
{
    Vector3D obj;
    struct PoolObject *next;
} PoolObject;

PoolObject object_pool[NUM_SLOTS] = {0};
PoolObject *freelist = NULL;

__attribute__((constructor)) void InitFreeList()
{
    for (int i = 0; i < NUM_SLOTS - 1; i++)
    {
        object_pool[i].next = &(object_pool[i+1]);
    }
    // freelist = object_pool;
    freelist = &(object_pool[0]);
    object_pool[NUM_SLOTS - 1].next = NULL;
}

Vector3D *AllocateMemory(void)
{
    if(freelist)
    {
        PoolObject *result = freelist;
        freelist = freelist->next;
        return &(result->obj);
    }
    return NULL;
}

void FreeMemory(Vector3D *vec)
{
    unsigned int i = ((uintptr_t)vec - (uintptr_t)object_pool) / sizeof(PoolObject);

    assert(&(object_pool[i].obj) == vec);

    PoolObject *po = &(object_pool[i]);
    po->next = freelist;
    freelist = po;
    return;
}


int main()
{
    for (int i = 0; i < NUM_ROUNDS; i++)
    {
        int numObjs = rand() % NUM_SLOTS;

        Vector3D *vector[numObjs];

        for (int j = 0; j < numObjs; j++)
        {
            vector[j] = AllocateMemory();
        }

        printf("round of %d -- got %d vectors\n", i, numObjs);
        
        for (int j = 0; j < numObjs; j++)
        {
            FreeMemory(vector[j]);
        }
        
    }
    
    return 0;
}