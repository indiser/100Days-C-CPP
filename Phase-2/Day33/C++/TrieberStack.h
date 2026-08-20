#pragma once

#include <atomic>
#include <type_traits>

template<typename Node>
struct TreiberStackWithStealMPMC {
    using value_type = Node;
    using pointer_type = std::add_pointer_t<Node>;
    using atomic_pointer_type = std::atomic<pointer_type>;

private:
    atomic_pointer_type head { nullptr };

public:
    TreiberStackWithStealMPMC() = default;
    TreiberStackWithStealMPMC(const TreiberStackWithStealMPMC&) = delete;
    TreiberStackWithStealMPMC& operator=(const TreiberStackWithStealMPMC&) = delete;

    void push(Node* node) {
        push(node, node);
    }
    
    void push(Node* chain_head, Node* chain_tail) {
        chain_tail->next = this->head.load(std::memory_order_relaxed);
        while (!this->head.compare_exchange_weak(
            chain_tail->next,
            chain_head,
            std::memory_order_release,
            std::memory_order_relaxed)) {
        }
    }

    Node* steal() noexcept {
        return head.exchange(nullptr, std::memory_order_acquire);
    }

    bool empty_unsafe() const noexcept {
        return head.load(std::memory_order_relaxed) == nullptr;
    }
};