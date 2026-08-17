#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>

// --- C++ LRU CACHE IMPLEMENTATION ---

template <typename K, typename V>
class LRUCache {
    struct Node {
        K key; V val;
        Node *prev, *next;
        Node(K k, V v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };

    Node *head, *tail;
    std::unordered_map<K, Node*> mp;
    size_t limit;

    inline void addNode(Node *n) {
        Node *old = head->next;
        head->next = n; n->prev = head;
        n->next = old; old->prev = n;
    }
    inline void delNode(Node *n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

public:
    LRUCache(size_t capacity) : limit(capacity) {
        if (limit == 0) limit = 1;
        head = new Node(K{}, V{});
        tail = new Node(K{}, V{});
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node *cur = head;
        while (cur) { Node *next = cur->next; delete cur; cur = next; }
    }

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    inline bool get(const K &key, V &out) {
        auto it = mp.find(key);
        if (it == mp.end()) return false;
        Node *n = it->second;
        delNode(n);
        addNode(n);
        out = n->val;
        return true;
    }

    inline void put(const K &key, const V &value) {
        auto it = mp.find(key);
        if (it != mp.end()) {
            Node *n = it->second;
            n->val = value;
            delNode(n);
            addNode(n);
            return;
        }
        if (mp.size() == limit) {
            Node *victim = tail->prev;
            mp.erase(victim->key);
            delNode(victim);
            delete victim;
        }
        Node *n = new Node(key, value);
        addNode(n);
        mp[key] = n;
    }
};

// --- BENCHMARK DRIVER ---

int main() {
    const int N = 1000000;
    const int CAP = 10000;
    const int KEY_RANGE = 8000; // Warm cache setup

    LRUCache<int, int> cache(CAP);

    // Warm-up phase
    for (int i = 0; i < KEY_RANGE; ++i) {
        cache.put(i, i * 2);
    }

    int val = 0;
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < N; ++i) {
        int k = i % KEY_RANGE;
        cache.put(k, i);
        cache.get(k, val);
    }

    auto end = std::chrono::steady_clock::now();

    double total_sec = std::chrono::duration<double>(end - start).count();
    double total_ops = N * 2.0;
    double mean_lat_ns = (total_sec * 1e9) / total_ops;
    double ops_sec = total_ops / total_sec;

    std::cout << "C++ Benchmark Results:\n";
    std::cout << "  Total Ops:    " << static_cast<long long>(total_ops) << "\n";
    std::cout << "  Mean Latency: " << std::fixed << std::setprecision(2) << mean_lat_ns << " ns/op\n";
    std::cout << "  Throughput:   " << std::fixed << std::setprecision(2) << ops_sec / 1e6 << " M ops/sec\n";

    return 0;
}