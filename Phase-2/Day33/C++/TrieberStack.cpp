#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include "TrieberStack.h"

using namespace std;

struct Node {
    int value;
    Node* next;
};

int main() {
    TreiberStackWithStealMPMC<Node> stack;
    constexpr int NUM_PRODUCERS = 10;
    constexpr int NUM_CONSUMERS = 10;
    constexpr int MSGS_PER_PRODUCER = 10'000;
    
    // Lifecycle management state
    std::atomic<int> active_producers{NUM_PRODUCERS};
    std::atomic<int> total_consumed{0}; 

    std::vector<std::jthread> producers;
    for (std::size_t i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back([&stack, &active_producers]() {
            for (int j = 0; j < MSGS_PER_PRODUCER; ++j) {
                auto* node = new Node();
                node->value = j;
                stack.push(node);
            }
            // Signal this thread is done pushing
            active_producers.fetch_sub(1, std::memory_order_release);
        });
    }

    std::vector<std::jthread> consumers;
    for (std::size_t i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back([&stack, &active_producers, &total_consumed]() {
            int local_count = 0;
            
            while (true) {
                auto* node = stack.steal();
                
                if (node == nullptr) {
                    // Check if producers are still alive
                    if (active_producers.load(std::memory_order_acquire) == 0) {
                        // Final safety check in case a producer pushed right before dying
                        node = stack.steal();
                        if (node == nullptr) break;
                    } else {
                        // Backoff to prevent 100% CPU starvation
                        std::this_thread::yield(); 
                        continue;
                    }
                }

                auto* head = node;
                while(head != nullptr) {
                    auto* next = head->next;
                    local_count++; 
                    delete head; // Safe because this thread now owns the disconnected chain
                    head = next;
                }
            }
            // Aggregate outside the hot path
            total_consumed.fetch_add(local_count, std::memory_order_relaxed);
        });
    }
    
    // Force threads to join before printing
    producers.clear(); 
    consumers.clear(); 
    
    std::cout << "Target: " << (NUM_PRODUCERS * MSGS_PER_PRODUCER) << "\n";
    std::cout << "Total nodes consumed: " << total_consumed.load() << std::endl;
    return 0;
}