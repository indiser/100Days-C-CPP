/*
 * Lock-free Treiber stack in C, with a REAL pop() — the operation
 * your C++ version dodged by only implementing bulk steal().
 *
 * ABA fix: tagged pointer (pointer + 64-bit counter) packed into a
 * 16-byte struct, CAS'd atomically as one unit via GCC's __int128
 * intrinsics (compiles to CMPXCHG16B on x86-64). This is the "real"
 * fix mentioned in the syllabus notes, not hand-waved.
 *
 * Build:
 *   gcc -std=c11 -O2 -mcx16 -pthread -fsanitize=thread -g \
 *       treiber_stack.c -o stack_tsan
 *
 * -mcx16 is required or GCC will silently lower the 128-bit CAS to
 * a libatomic lock-based fallback, which defeats the entire point.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>

/* ---------- Node ---------- */

typedef struct Node {
    int value;
    struct Node *next;        /* live stack linkage -- may be read by a
                                * hazard-protected thread at any time */
    struct Node *retire_next; /* retire-list linkage ONLY. Separate field
                                * on purpose: reusing `next` here caused a
                                * real TSan-caught race against readers
                                * still walking the stack via `next`. */
} Node;

/* ---------- Tagged pointer ----------
 * ptr:  actual head node
 * tag:  monotonically incrementing counter, bumped on every
 *       successful push AND pop. Two different points in time can
 *       never produce the same (ptr, tag) pair even if the
 *       allocator reuses the same address, because the tag keeps
 *       climbing. This is what breaks ABA: the CAS compares the
 *       WHOLE 128-bit word, not just the pointer.
 */
typedef struct {
    Node    *ptr;
    uint64_t tag;
} TaggedPtr;

/* GCC/Clang: a 16-byte struct with correct alignment is lock-free
 * on x86-64 with -mcx16, backed by CMPXCHG16B. */
typedef _Atomic(__int128) atomic_int128_t;

typedef union {
    TaggedPtr    tp;
    __int128     raw;
} TPUnion;

static inline __int128 tp_to_raw(TaggedPtr tp) {
    TPUnion u;
    u.tp = tp;
    return u.raw;
}

static inline TaggedPtr raw_to_tp(__int128 raw) {
    TPUnion u;
    u.raw = raw;
    return u.tp;
}

/* ---------- Hazard pointers ----------
 * Tagging fixes the CAS. It does NOT stop another thread from
 * free()-ing a node while we're mid-dereference of it (line marked
 * below). TSan caught exactly this on the first stress run. Hazard
 * pointers close that window: before dereferencing a node, a thread
 * publishes "I am reading this pointer right now" into a slot every
 * other thread checks before actually freeing anything. */

#define MAX_HAZARD_THREADS 32

static _Atomic(Node *) g_hazard[MAX_HAZARD_THREADS];
static _Thread_local int t_hazard_slot = -1;
static atomic_int g_next_hazard_slot = 0;

/* Simple retire list: nodes that were popped and are ready to free,
 * but might still be someone's hazard target. Protected by a mutex
 * since retirement is off the hot path -- only the CAS loops need
 * to be lock-free, not cleanup. */
static pthread_mutex_t g_retire_lock = PTHREAD_MUTEX_INITIALIZER;
static Node *g_retire_list = NULL;

static int hazard_slot_for_this_thread(void) {
    if (t_hazard_slot == -1) {
        t_hazard_slot = atomic_fetch_add_explicit(&g_next_hazard_slot, 1,
                                                    memory_order_relaxed);
        assert(t_hazard_slot < MAX_HAZARD_THREADS);
    }
    return t_hazard_slot;
}

static int node_is_hazardous(Node *n) {
    for (int i = 0; i < MAX_HAZARD_THREADS; ++i) {
        if (atomic_load_explicit(&g_hazard[i], memory_order_acquire) == n) {
            return 1;
        }
    }
    return 0;
}

/* Called after a node is unlinked from the stack. Frees it
 * immediately if no thread is currently dereferencing it (the
 * common case); otherwise defers it to the retire list. */
static void retire_node(Node *n) {
    if (!node_is_hazardous(n)) {
        free(n);
        return;
    }
    pthread_mutex_lock(&g_retire_lock);
    n->retire_next = g_retire_list;
    g_retire_list = n;
    pthread_mutex_unlock(&g_retire_lock);
}

/* Periodically sweep the retire list -- nodes that were hazardous
 * earlier may be safe to free now. Cheap to call often since it's
 * O(retired) and the list is normally near-empty. */
static void retire_sweep(void) {
    pthread_mutex_lock(&g_retire_lock);
    Node *prev = NULL;
    Node *cur = g_retire_list;
    while (cur != NULL) {
        Node *next = cur->retire_next;
        if (!node_is_hazardous(cur)) {
            if (prev) prev->retire_next = next; else g_retire_list = next;
            free(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
    pthread_mutex_unlock(&g_retire_lock);
}

/* ---------- Stack ---------- */

typedef struct {
    atomic_int128_t head; /* packs TaggedPtr */
} TreiberStack;

static void stack_init(TreiberStack *s) {
    TaggedPtr initial = { .ptr = NULL, .tag = 0 };
    atomic_store_explicit(&s->head, tp_to_raw(initial), memory_order_relaxed);
    for (int i = 0; i < MAX_HAZARD_THREADS; ++i) {
        atomic_init(&g_hazard[i], NULL);
    }
}

/* push: classic single-node link, no ABA hazard here (see writeup) --
 * but we still bump the tag on every successful CAS so pop's ABA
 * defense stays airtight regardless of push/pop interleaving. */
static void stack_push(TreiberStack *s, Node *node) {
    __int128 old_raw = atomic_load_explicit(&s->head, memory_order_relaxed);
    TaggedPtr new_tp;

    for (;;) {
        TaggedPtr old_tp = raw_to_tp(old_raw);
        node->next = old_tp.ptr;
        new_tp.ptr = node;
        new_tp.tag = old_tp.tag + 1;

        if (atomic_compare_exchange_weak_explicit(
                &s->head, &old_raw, tp_to_raw(new_tp),
                memory_order_release, memory_order_relaxed)) {
            return;
        }
        /* old_raw was updated in place by the failed CAS; loop retries
         * with the fresh value automatically. */
    }
}

/* pop: THE operation that actually has the ABA hazard.
 *
 * Classic failure without tagging:
 *   1. Thread A reads head = X, reads X->next = Y.
 *   2. A is preempted.
 *   3. Thread B pops X, pops Y, frees X, allocates a new node that
 *      the allocator happens to place at address X, pushes it.
 *      head is now X again -- but its ->next is NOT Y anymore.
 *   4. A resumes, CAS(head, expected=X, new=Y) SUCCEEDS because the
 *      pointer matches, silently resurrecting a stale/wrong chain.
 *
 * With tagging: even though the pointer X repeats, the tag counter
 * has moved on, so the 128-bit expected value from step 1 no longer
 * matches the 128-bit actual value at step 4, and the CAS correctly
 * fails and retries. */
static Node *stack_pop(TreiberStack *s) {
    int slot = hazard_slot_for_this_thread();

    for (;;) {
        __int128 raw1 = atomic_load_explicit(&s->head, memory_order_acquire);
        TaggedPtr tp1 = raw_to_tp(raw1);
        if (tp1.ptr == NULL) {
            atomic_store_explicit(&g_hazard[slot], NULL, memory_order_release);
            return NULL; /* empty */
        }

        /* Publish intent to read tp1.ptr BEFORE trusting it. */
        atomic_store_explicit(&g_hazard[slot], tp1.ptr, memory_order_release);

        /* Re-check head hasn't already moved past what we published --
         * without this, the node could've been freed in the gap
         * between the first load and the hazard publish. */
        __int128 raw2 = atomic_load_explicit(&s->head, memory_order_acquire);
        if (raw2 != raw1) {
            continue; /* stale, retry from scratch */
        }

        /* Now safe: no one will free tp1.ptr while our hazard is set. */
        Node *next = tp1.ptr->next;

        TaggedPtr new_tp;
        new_tp.ptr = next;
        new_tp.tag = tp1.tag + 1;

        __int128 expected = raw1;
        if (atomic_compare_exchange_weak_explicit(
                &s->head, &expected, tp_to_raw(new_tp),
                memory_order_acquire, memory_order_acquire)) {
            atomic_store_explicit(&g_hazard[slot], NULL, memory_order_release);
            return tp1.ptr; /* this thread now exclusively owns it */
        }
        /* CAS failed: retry. Hazard stays set to tp1.ptr momentarily,
         * harmless -- we clear it as soon as we take a fresh reading. */
    }
}

static int stack_empty_unsafe(TreiberStack *s) {
    TaggedPtr tp = raw_to_tp(atomic_load_explicit(&s->head, memory_order_relaxed));
    return tp.ptr == NULL;
}

/* ---------- Stress test ---------- */

#define NUM_PRODUCERS      10
#define NUM_CONSUMERS      10
#define MSGS_PER_PRODUCER  10000
#define TOTAL_MSGS         (NUM_PRODUCERS * MSGS_PER_PRODUCER)

static TreiberStack g_stack;
static atomic_int    g_active_producers;
static atomic_long   g_total_consumed;
static atomic_long   g_total_pushed;

static void *producer_fn(void *arg) {
    (void)arg;
    for (int j = 0; j < MSGS_PER_PRODUCER; ++j) {
        Node *n = malloc(sizeof(Node));
        assert(n != NULL);
        n->value = j;
        stack_push(&g_stack, n);
        atomic_fetch_add_explicit(&g_total_pushed, 1, memory_order_relaxed);
    }
    atomic_fetch_sub_explicit(&g_active_producers, 1, memory_order_release);
    return NULL;
}

static void *consumer_fn(void *arg) {
    (void)arg;
    long local_count = 0;

    for (;;) {
        Node *n = stack_pop(&g_stack);

        if (n == NULL) {
            if (atomic_load_explicit(&g_active_producers, memory_order_acquire) == 0
                && stack_empty_unsafe(&g_stack)) {
                /* final drain in case something landed right before
                 * the last producer signalled done */
                n = stack_pop(&g_stack);
                if (n == NULL) break;
            } else {
                sched_yield();
                continue;
            }
        }

        local_count++;
        retire_node(n); /* not a raw free() -- respects other threads' hazards */

        if ((local_count & 0xFF) == 0) {
            retire_sweep(); /* periodically reclaim what's now safe */
        }
    }
    retire_sweep(); /* final drain of anything left pending */

    atomic_fetch_add_explicit(&g_total_consumed, local_count, memory_order_relaxed);
    return NULL;
}

int main(void) {
    stack_init(&g_stack);
    atomic_init(&g_active_producers, NUM_PRODUCERS);
    atomic_init(&g_total_consumed, 0);
    atomic_init(&g_total_pushed, 0);

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        pthread_create(&consumers[i], NULL, consumer_fn, NULL);
    }
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        pthread_create(&producers[i], NULL, producer_fn, NULL);
    }

    for (int i = 0; i < NUM_PRODUCERS; ++i) pthread_join(producers[i], NULL);
    for (int i = 0; i < NUM_CONSUMERS; ++i) pthread_join(consumers[i], NULL);

    long pushed   = atomic_load(&g_total_pushed);
    long consumed = atomic_load(&g_total_consumed);

    printf("Target:          %d\n", TOTAL_MSGS);
    printf("Total pushed:    %ld\n", pushed);
    printf("Total consumed:  %ld\n", consumed);
    printf("Stack empty at end: %s\n", stack_empty_unsafe(&g_stack) ? "yes" : "NO -- BUG");

    if (pushed != TOTAL_MSGS || consumed != TOTAL_MSGS) {
        fprintf(stderr, "MISMATCH -- lost or duplicated nodes\n");
        return 1;
    }

    printf("OK: single-node pop() verified under contention.\n");
    return 0;
}