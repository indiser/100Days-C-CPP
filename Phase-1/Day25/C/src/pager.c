#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "pager.h"

static void list_init(DoublyLinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

static void list_unlink(DoublyLinkedList *list, Page *page) {
    if (page->prev) page->prev->next = page->next;
    else list->head = page->next;

    if (page->next) page->next->prev = page->prev;
    else list->tail = page->prev;

    page->next = NULL;
    page->prev = NULL;
    list->size--;
}

static void list_add_front(DoublyLinkedList *list, Page *page) {
    page->next = list->head;
    page->prev = NULL;

    if (list->head) list->head->prev = page;
    list->head = page;

    if (!list->tail) list->tail = page;
    list->size++;
}

static void list_move_to_front(DoublyLinkedList *list, Page *page) {
    if (list->head == page) return;
    list_unlink(list, page);
    list_add_front(list, page);
}

static int hash_func(int page_id) {
    return page_id % HASH_MAP_SIZE;
}

static Page *hash_find(Pager *pager, int page_id) {
    int bucket = hash_func(page_id);
    Page *curr = pager->hash_map[bucket];
    while (curr) {
        if (curr->page_id == page_id) return curr;
        curr = curr->hash_next;
    }
    return NULL;
}

static void hash_insert(Pager *pager, Page *page) {
    int bucket = hash_func(page->page_id);
    page->hash_next = pager->hash_map[bucket];
    pager->hash_map[bucket] = page;
}

static void hash_remove(Pager *pager, Page *page) {
    int bucket = hash_func(page->page_id);
    Page *curr = pager->hash_map[bucket];
    Page *prev = NULL;

    while (curr) {
        if (curr == page) {
            if (prev) prev->hash_next = curr->hash_next;
            else pager->hash_map[bucket] = curr->hash_next;
            curr->hash_next = NULL;
            return;
        }
        prev = curr;
        curr = curr->hash_next;
    }
}

void pager_flush(Pager *pager, Page *page) {
    if (!page->is_dirty) return;
    off_t offset = (off_t)page->page_id * PAGE_SIZE;
    ssize_t written = pwrite(pager->fd, page->data, PAGE_SIZE, offset);
    if (written == PAGE_SIZE) {
        pager->pwrite_count++;
        page->is_dirty = false;
    }
}

static Page *pager_evict(Pager *pager) {
    Page *curr = pager->lru_list.tail;
    while (curr) {
        if (curr->pin_count == 0) {
            pager_flush(pager, curr);
            hash_remove(pager, curr);
            list_unlink(&pager->lru_list, curr);
            return curr;
        }
        curr = curr->prev;
    }
    return NULL;
}

Pager *pager_open(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd < 0) return NULL;

    Pager *pager = (Pager *)malloc(sizeof(Pager));
    if (!pager) { close(fd); return NULL; }

    pager->fd = fd;
    pager->capacity = CACHE_CAPACITY;
    pager->pwrite_count = 0;
    list_init(&pager->lru_list);
    for (int i = 0; i < HASH_MAP_SIZE; i++) pager->hash_map[i] = NULL;

    // Check if empty file -> Initialize Page 0 (Header)
    off_t file_len = lseek(fd, 0, SEEK_END);
    if (file_len == 0) {
        Page *header_page = (Page *)malloc(sizeof(Page));
        if (!header_page) { free(pager); close(fd); return NULL; }

        header_page->page_id = 0;
        header_page->pin_count = 0;
        header_page->is_dirty = true;
        memset(header_page->data, 0, PAGE_SIZE);

        DatabaseHeader *hdr = (DatabaseHeader *)header_page->data;
        hdr->page_size = PAGE_SIZE;
        hdr->total_pages = 1; // Page 0 used by header
        hdr->free_list_head = -1;

        hash_insert(pager, header_page);
        list_add_front(&pager->lru_list, header_page);
        pager_flush(pager, header_page);
    }

    return pager;
}

Page *pager_fetch(Pager *pager, int page_id) {
    Page *page = hash_find(pager, page_id);
    if (page) {
        page->pin_count++;
        list_move_to_front(&pager->lru_list, page);
        return page;
    }

    if (pager->lru_list.size >= pager->capacity) {
        page = pager_evict(pager);
        if (!page) return NULL; // All pinned OOM
    } else {
        page = (Page *)malloc(sizeof(Page));
        if (!page) return NULL;
    }

    page->page_id = page_id;
    page->pin_count = 1;
    page->is_dirty = false;
    
    off_t offset = (off_t)page_id * PAGE_SIZE;
    memset((void *)page->data, 0, PAGE_SIZE);
    ssize_t bytes_read = pread(pager->fd, page->data, PAGE_SIZE, offset);
    (void)bytes_read;

    hash_insert(pager, page);
    list_add_front(&pager->lru_list, page);
    return page;
}

int pager_alloc_page(Pager *pager) {
    Page *hdr_page = pager_fetch(pager, 0);
    DatabaseHeader *hdr = (DatabaseHeader *)hdr_page->data;

    int new_page_id;
    if (hdr->free_list_head != -1) {
        // Reuse page from free list
        new_page_id = hdr->free_list_head;
        Page *free_page = pager_fetch(pager, new_page_id);
        
        // Next free pointer stored in first 4 bytes of freed page
        int32_t next_free;
        memcpy(&next_free, free_page->data, sizeof(int32_t));
        hdr->free_list_head = next_free;
        
        pager_unpin(pager, free_page, false);
    } else {
        // Append new page at file end
        new_page_id = hdr->total_pages;
        hdr->total_pages++;
    }

    pager_unpin(pager, hdr_page, true);
    return new_page_id;
}

void pager_free_page(Pager *pager, int page_id) {
    if (page_id <= 0) return; // Cannot free header page

    Page *hdr_page = pager_fetch(pager, 0);
    DatabaseHeader *hdr = (DatabaseHeader *)hdr_page->data;

    Page *target = pager_fetch(pager, page_id);
    memset(target->data, 0, PAGE_SIZE);
    
    // Write current free list head into new free page payload
    memcpy(target->data, &hdr->free_list_head, sizeof(int32_t));
    
    // Free page becomes new head
    hdr->free_list_head = page_id;

    pager_unpin(pager, target, true);
    pager_unpin(pager, hdr_page, true);
}

void pager_unpin(Pager *pager, Page *page, bool is_dirty) {
    (void)pager;
    if (!page) return;
    if (is_dirty) page->is_dirty = true;
    if (page->pin_count > 0) page->pin_count--;
}

void pager_close(Pager *pager) {
    if (!pager) return;
    Page *curr = pager->lru_list.head;
    while (curr) {
        Page *next = curr->next;
        pager_flush(pager, curr);
        free(curr);
        curr = next;
    }
    close(pager->fd);
    free(pager);
}

void pager_destroy_no_flush(Pager *pager) {
    if (!pager) return;
    Page *curr = pager->lru_list.head;
    while (curr) {
        Page *next = curr->next;
        free(curr); // Free node directly, NO pager_flush call
        curr = next;
    }
    close(pager->fd);
    free(pager);
}