#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <chrono>
#include <stdexcept>

class DeadlockException : public std::runtime_error {
public:
    explicit DeadlockException(const std::string& msg) : std::runtime_error(msg) {}
};

class LockGraph {
private:
    std::unordered_map<uint64_t, std::unordered_set<uint64_t>> adj_;
    std::mutex registry_mutex_;

    bool dfs(uint64_t curr, std::unordered_set<uint64_t>& visited, std::unordered_set<uint64_t>& stack) {
        visited.insert(curr);
        stack.insert(curr);

        if (adj_.find(curr) != adj_.end()) {
            for (uint64_t next : adj_[curr]) {
                if (visited.find(next) == visited.end()) {
                    if (dfs(next, visited, stack)) return true;
                } else if (stack.find(next) != stack.end()) {
                    return true;
                }
            }
        }
        stack.erase(curr);
        return false;
    }

public:
    static LockGraph& instance() {
        static LockGraph graph;
        return graph;
    }

    void add_request(uint64_t tid, uint64_t mid) {
        std::lock_guard<std::mutex> lock(registry_mutex_);
        adj_[tid].insert(mid);

        if (detect_cycle()) {
            adj_[tid].erase(mid);
            throw DeadlockException("[DEADLOCK DETECTED] Thread " + std::to_string(tid) +
                                   " requesting Mutex " + std::to_string(mid) + " causes cycle!");
        }
    }

    void acquire(uint64_t tid, uint64_t mid) {
        std::lock_guard<std::mutex> lock(registry_mutex_);
        adj_[tid].erase(mid);
        adj_[mid].insert(tid);
    }

    void release(uint64_t tid, uint64_t mid) {
        std::lock_guard<std::mutex> lock(registry_mutex_);
        adj_[mid].erase(tid);
    }

    bool detect_cycle() {
        std::unordered_set<uint64_t> visited;
        std::unordered_set<uint64_t> stack;

        for (const auto& pair : adj_) {
            uint64_t node = pair.first;
            if (visited.find(node) == visited.end()) {
                if (dfs(node, visited, stack)) return true;
            }
        }
        return false;
    }
};

class DeadlockDetectorMutex {
private:
    std::mutex native_mutex_;
    uint64_t id_;

    uint64_t get_tid() const {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

public:
    explicit DeadlockDetectorMutex(uint64_t id) : id_(id) {}

    void lock() {
        uint64_t tid = get_tid();
        LockGraph::instance().add_request(tid, id_);
        native_mutex_.lock();
        LockGraph::instance().acquire(tid, id_);
    }

    void unlock() {
        uint64_t tid = get_tid();
        native_mutex_.unlock();
        LockGraph::instance().release(tid, id_);
    }
};

DeadlockDetectorMutex m1(101);
DeadlockDetectorMutex m2(102);
DeadlockDetectorMutex m3(103);

void worker_1() {
    bool m1_acquired = false;
    try {
        m1.lock();
        m1_acquired = true;
        std::cout << "T1 got M1\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::cout << "T1 requesting M2...\n";
        m2.lock(); // If throws here, m2_acquired false
        std::cout << "T1 got M2\n";
        m2.unlock();
        
        m1.unlock();
    } catch (const DeadlockException& e) {
        std::cout << e.what() << "\n";
        if (m1_acquired) m1.unlock();
    }
}

void worker_2() {
    try {
        m2.lock();
        std::cout << "T2 got M2\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "T2 requesting M3...\n";
        m3.lock();
        std::cout << "T2 got M3\n";
        m3.unlock();
        m2.unlock();
    } catch (const DeadlockException& e) {
        std::cout << e.what() << "\n";
        m2.unlock();
    }
}

void worker_3() {
    try {
        m3.lock();
        std::cout << "T3 got M3\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "T3 requesting M1...\n";
        m1.lock();
        std::cout << "T3 got M1\n";
        m1.unlock();
        m3.unlock();
    } catch (const DeadlockException& e) {
        std::cout << e.what() << "\n";
        m3.unlock();
    }
}

int main() {
    std::thread t1(worker_1);
    std::thread t2(worker_2);
    std::thread t3(worker_3);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}