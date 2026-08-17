#include <iostream>
#include <unordered_map>
#include <chrono>
using namespace std;

template <typename K, typename V>
class LRUCache {
    struct Node {
        K key; V val;
        Node *prev, *next;
        Node(K k, V v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };

    Node *head, *tail;   // sentinels
    unordered_map<K, Node*> mp;
    size_t limit;

    void addNode(Node *n) {
        Node *old = head->next;
        head->next = n; n->prev = head;
        n->next = old; old->prev = n;
    }
    void delNode(Node *n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

public:
    LRUCache(size_t capacity) : limit(capacity) {
        if (limit == 0) limit = 1;   // avoid degenerate eviction-on-empty case
        head = new Node(K{}, V{});
        tail = new Node(K{}, V{});
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node *cur = head;
        while (cur) { Node *next = cur->next; delete cur; cur = next; }
    }

    // no copying — raw owning pointers, keep it simple
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    bool get(const K &key, V &out) {
        auto it = mp.find(key);
        if (it == mp.end()) return false;
        Node *n = it->second;
        delNode(n);
        addNode(n);
        out = n->val;
        return true;
    }

    void put(const K &key, const V &value) {
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
            delete victim;          // <-- was leaking before
        }
        Node *n = new Node(key, value);
        addNode(n);
        mp[key] = n;
    }
};

int main() {
    LRUCache<int,int> lru(2);
    int v;
    lru.put(1,1);
    lru.put(2,2);
    if (lru.get(1,v)) cout << v << endl;
    lru.put(3,3);                       // evicts key 2
    cout << (lru.get(2,v) ? v : -1) << endl;
    cout << (lru.get(3,v) ? v : -1) << endl;
    cout << (lru.get(1,v) ? v : -1) << endl;
    return 0;
}