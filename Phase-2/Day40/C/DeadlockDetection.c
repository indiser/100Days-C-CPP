#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#define MAX_NODES 64

typedef enum {
    NODE_THREAD,
    NODE_MUTEX
} NodeType;

typedef struct {
    int id;
    NodeType type;
    bool active;
} Node;

typedef struct {
    Node nodes[MAX_NODES];
    int adj[MAX_NODES][MAX_NODES];
    int node_count;
    pthread_mutex_t lock;
} Graph;

typedef struct {
    int id;
    pthread_mutex_t native;
} my_mutex_t;

static Graph g_graph;

uint64_t get_thread_id(void) {
    return (uint64_t)(uintptr_t)pthread_self();
}

int get_or_create_node(int id, NodeType type) {
    for (int i = 0; i < g_graph.node_count; i++) {
        if (g_graph.nodes[i].id == id && g_graph.nodes[i].type == type) {
            return i;
        }
    }
    if (g_graph.node_count >= MAX_NODES) {
        fprintf(stderr, "Graph full! Exceeded MAX_NODES\n");
        exit(EXIT_FAILURE);
    }
    int idx = g_graph.node_count++;
    g_graph.nodes[idx].id = id;
    g_graph.nodes[idx].type = type;
    g_graph.nodes[idx].active = true;
    return idx;
}

bool dfs(int curr, int visited[], int stack[]) {
    visited[curr] = 1;
    stack[curr] = 1;

    for (int next = 0; next < g_graph.node_count; next++) {
        if (g_graph.adj[curr][next]) {
            if (!visited[next]) {
                if (dfs(next, visited, stack)) return true;
            } else if (stack[next]) {
                return true;
            }
        }
    }
    stack[curr] = 0;
    return false;
}

bool detect_cycle(void) {
    int visited[MAX_NODES] = {0};
    int stack[MAX_NODES] = {0};

    for (int i = 0; i < g_graph.node_count; i++) {
        if (!visited[i]) {
            if (dfs(i, visited, stack)) return true;
        }
    }
    return false;
}

void graph_init(void) {
    g_graph.node_count = 0;
    pthread_mutex_init(&g_graph.lock, NULL);
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            g_graph.adj[i][j] = 0;
        }
    }
}

void my_mutex_init(my_mutex_t *m, int id) {
    m->id = id;
    pthread_mutex_init(&m->native, NULL);
}

int my_mutex_lock(my_mutex_t *m) {
    int tid = (int)pthread_self();

    pthread_mutex_lock(&g_graph.lock);
    int t_idx = get_or_create_node(tid, NODE_THREAD);
    int m_idx = get_or_create_node(m->id, NODE_MUTEX);

    g_graph.adj[t_idx][m_idx] = 1;

    if (detect_cycle()) {
        printf("[DEADLOCK DETECTED] Thread %d requesting Mutex %d creates cycle!\n", tid, m->id);
        g_graph.adj[t_idx][m_idx] = 0;
        pthread_mutex_unlock(&g_graph.lock);
        return -1;
    }
    pthread_mutex_unlock(&g_graph.lock);

    pthread_mutex_lock(&m->native);

    pthread_mutex_lock(&g_graph.lock);
    g_graph.adj[t_idx][m_idx] = 0;
    g_graph.adj[m_idx][t_idx] = 1;
    pthread_mutex_unlock(&g_graph.lock);

    return 0;
}

void my_mutex_unlock(my_mutex_t *m) {
    int tid = (int)pthread_self();

    pthread_mutex_unlock(&m->native);

    pthread_mutex_lock(&g_graph.lock);
    int t_idx = get_or_create_node(tid, NODE_THREAD);
    int m_idx = get_or_create_node(m->id, NODE_MUTEX);
    g_graph.adj[m_idx][t_idx] = 0;
    pthread_mutex_unlock(&g_graph.lock);
}

my_mutex_t m1, m2, m3;

void *worker_1(void *arg) {
    (void)arg;
    if (my_mutex_lock(&m1) == 0) {
        printf("T1 got M1\n");
        sleep(1);
        printf("T1 requesting M2...\n");
        if (my_mutex_lock(&m2) == 0) {
            printf("T1 got M2\n");
            my_mutex_unlock(&m2);
        }
        my_mutex_unlock(&m1);
    }
    return NULL;
}

void *worker_2(void *arg) {
    (void)arg;
    if (my_mutex_lock(&m2) == 0) {
        printf("T2 got M2\n");
        sleep(1);
        printf("T2 requesting M3...\n");
        if (my_mutex_lock(&m3) == 0) {
            printf("T2 got M3\n");
            my_mutex_unlock(&m3);
        }
        my_mutex_unlock(&m2);
    }
    return NULL;
}

void *worker_3(void *arg) {
    (void)arg;
    if (my_mutex_lock(&m3) == 0) {
        printf("T3 got M3\n");
        sleep(1);
        printf("T3 requesting M1...\n");
        if (my_mutex_lock(&m1) == 0) {
            printf("T3 got M1\n");
            my_mutex_unlock(&m1);
        }
        my_mutex_unlock(&m3);
    }
    return NULL;
}

int main(void) {
    graph_init();

    my_mutex_init(&m1, 101);
    my_mutex_init(&m2, 102);
    my_mutex_init(&m3, 103);

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, worker_1, NULL);
    pthread_create(&t2, NULL, worker_2, NULL);
    pthread_create(&t3, NULL, worker_3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}