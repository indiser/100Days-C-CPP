#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct {
    void *data;
    int refcount;
} RefBox;

RefBox* refbox_create(void *data) {
    if(!data) return NULL;
    RefBox *box = malloc(sizeof(RefBox));
    if(!box) return NULL;
    box->data = data;
    box->refcount = 1;
    return box;
}

void refbox_retain(RefBox *box) {
    if(!box) return;
    box->refcount++;
}

void refbox_release(RefBox *box) {
    if(!box) return;
    box->refcount--;
    if (box->refcount == 0) {
        free(box->data);
        free(box);
    }
}

typedef struct {
    int id;
    char name[20];
} User;


int main()
{
    User *u = malloc(sizeof(User));
    u->id = 1;

    RefBox *box1 = refbox_create(u);
    printf("Initial refCount: %d\n", box1->refcount);

    RefBox *box2 = box1;
    refbox_retain(box2);
    printf("Shared pointer Count: %d\n", box2->refcount);

    refbox_release(box1);
    printf("After releasing of 1: %d\n", box2->refcount);

    refbox_release(box2);
    printf("Freed all pointers");
    
    return 0;
}