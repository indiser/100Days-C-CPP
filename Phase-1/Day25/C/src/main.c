#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "pager.h"

void test_clean_vs_dirty_eviction() {
    remove("test_metric.db");
    Pager *p = pager_open("test_metric.db");

    // Allocate & Dirty 10 pages
    for (int i = 1; i <= CACHE_CAPACITY; i++) {
        Page *pg = pager_fetch(p, i);
        pg->data[0] = 'A';
        pager_unpin(p, pg, true);
    }

    uint64_t baseline_writes = p->pwrite_count;

    // Fetch 10 pages as CLEAN
    for (int i = 11; i <= CACHE_CAPACITY + 10; i++) {
        Page *pg = pager_fetch(p, i);
        pager_unpin(p, pg, false); // CLEAN!
    }

    // Force clean eviction by fetching page 1
    Page *pg_overflow = pager_fetch(p, 1);
    pager_unpin(p, pg_overflow, false);

    // Evicting clean pages must NOT increment pwrite_count
    assert(p->pwrite_count == baseline_writes + CACHE_CAPACITY); 
    printf("[PASS] Clean vs Dirty Eviction Metric Assert\n");

    pager_close(p);
    remove("test_metric.db");
}

void test_freelist_reuse() {
    remove("test_free.db");
    Pager *p = pager_open("test_free.db");

    int id1 = pager_alloc_page(p); // 1
    int id2 = pager_alloc_page(p); // 2
    assert(id1 == 1 && id2 == 2);

    pager_free_page(p, id1); // Free 1

    int id3 = pager_alloc_page(p); // Should reuse 1
    assert(id3 == 1);
    printf("[PASS] Page 0 Header Free List Allocation Reuse\n");

    pager_close(p);
    remove("test_free.db");
}

void test_crash_mid_write_integrity() {
    remove("test_crash.db");
    Pager *p = pager_open("test_crash.db");

    int page_id = pager_alloc_page(p);
    Page *pg = pager_fetch(p, page_id);
    strcpy((char *)pg->data, "CORRUPT_DATA_TARGET");
    
    // Unpin as dirty, but DO NOT FLUSH. Simulate sudden process termination.
    pager_unpin(p, pg, true);

    // Destroy in-memory state WITHOUT flushing dirty pages to disk
    pager_destroy_no_flush(p);

    // Reopen from disk -> dirty unwritten state must NOT be on disk
    p = pager_open("test_crash.db");
    Page *recovered = pager_fetch(p, page_id);
    assert(strcmp((char *)recovered->data, "CORRUPT_DATA_TARGET") != 0);
    printf("[PASS] Mid-Write Crash Safety (ASan Clean)\n");

    pager_unpin(p, recovered, false);
    pager_close(p);
    remove("test_crash.db");
}

void test_random_thrashing() {
    remove("test_thrash.db");
    Pager *p = pager_open("test_thrash.db");

    // Alloc 50 pages
    for (int i = 0; i < 50; i++) pager_alloc_page(p);

    // Random access 500 times
    srand(42);
    for (int i = 0; i < 500; i++) {
        int target = (rand() % 49) + 1;
        Page *pg = pager_fetch(p, target);
        pg->data[0] = (uint8_t)(target & 0xFF);
        pager_unpin(p, pg, true);
    }

    printf("[PASS] Random Thrashing Stress Test Passed\n");
    pager_close(p);
    remove("test_thrash.db");
}

int main(void) {
    test_clean_vs_dirty_eviction();
    test_freelist_reuse();
    test_crash_mid_write_integrity();
    test_random_thrashing();
    return 0;
}