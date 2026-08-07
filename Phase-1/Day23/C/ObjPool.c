#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<assert.h>

#define NUM_SLOTS 10

typedef struct {
    int x, y, z;
}Vector3D;

typedef struct
{
    bool allocated;
    Vector3D obj;
} PoolObject;

PoolObject object_pool[NUM_SLOTS] = {0};

Vector3D *AllocateMemory(void)
{
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        if(!object_pool[i].allocated)
        {
            object_pool[i].allocated = true;
            return &(object_pool[i].obj);
        }
    }
    return NULL;
}

void FreeMemory(Vector3D *vec)
{
    unsigned int i = ((uintptr_t)vec - (uintptr_t)object_pool) / sizeof(PoolObject);

    assert(&(object_pool[i].obj) == vec);
    assert(object_pool[i].allocated);
    object_pool[i].allocated = false;
    return;
}

/*
void FreeMemory(Vector3D *vec)
{
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        if(&(object_pool[i].obj) == vec)
        {
            assert(object_pool[i].allocated)
            object_pool[i].allocated = false;
            return;
        }
    }
        assert(false);
}
*/

int main()
{
    for (int i = 0; i < NUM_SLOTS; i++)
    {
        Vector3D *v1 = AllocateMemory();
        Vector3D *v2 = AllocateMemory();
        printf("The address of %d is: %p %p\n", i, v1, v2);
        FreeMemory(v1);
        FreeMemory(v2);
    }
    
    return 0;
}