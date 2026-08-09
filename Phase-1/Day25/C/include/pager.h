#ifndef PAGER_H
#define PAGER_H

#include<stdint.h>
#include<stdbool.h>

#define PAGE_SIZE 4096
#define CACHE_CAPACITY 10
#define HASH_MAP_SIZE (CACHE_CAPACITY * 2)

typedef struct DatabaseHeader {
    uint32_t page_size;
    uint32_t total_pages;
    int32_t free_list_head;
} DatabaseHeader;

typedef struct Page
{
    int page_id;
    uint8_t data[PAGE_SIZE];
    int pin_count;
    bool is_dirty;
    struct Page *next, *prev, *hash_next;
}Page;

typedef struct DoublyLinkedList
{
    Page *head, *tail;
    int size;
}DoublyLinkedList;

typedef struct Pager
{
    int fd;
    DoublyLinkedList lru_list;
    Page *hash_map[HASH_MAP_SIZE];
    int capacity;
    uint64_t pwrite_count;
}Pager;

Pager *pager_open(const char *filename);
Page *pager_fetch(Pager *pager, int page_id);
int pager_alloc_page(Pager *pager);
void pager_free_page(Pager *pager, int page_id);
void pager_unpin(Pager *pager, Page *page, bool is_dirty);
void pager_flush(Pager *pager, Page *page);
void pager_close(Pager *pager);
void pager_destroy_no_flush(Pager *pager);


#endif