#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <variant>
#include <memory>
#include <atomic>
#include <cstdint>

// Message Payloads
struct NewOrderPayload {
    uint64_t order_id;
    uint32_t symbol;
    double price;
    uint32_t quantity;
    uint8_t side;
};

struct CancelOrderPayload {
    uint64_t order_id;
    uint32_t symbol;
};

struct PoisonPillPayload {};

using Message = std::variant<NewOrderPayload, CancelOrderPayload, PoisonPillPayload>;

// Thread-Safe Mailbox Queue with Blocking & Condition Variable (No Spin Burn)
class Mailbox {
private:
    std::queue<Message> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
    const size_t capacity;

public:
    explicit Mailbox(size_t cap = 1024) : capacity(cap) {}

    // Push with backpressure (blocks if queue full, no silent drops!)
    void push(Message msg) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return queue.size() < capacity; });
        queue.push(std::move(msg));
        cv.notify_one();
    }

    // Blocking pop
    Message pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        Message msg = std::move(queue.front());
        queue.pop();
        cv.notify_one();
        return msg;
    }
};

// Base Abstract Actor Class
class Actor {
protected:
    Mailbox mailbox;
    std::thread worker_thread;
    std::atomic<bool> running{true};

    virtual void process_message(const Message& msg) = 0;

    void run() {
        while (running) {
            Message msg = mailbox.pop();
            if (std::holds_alternative<PoisonPillPayload>(msg)) {
                running = false;
            }
            process_message(msg);
        }
    }

public:
    Actor() = default;
    virtual ~Actor() {
        if (worker_thread.joinable()) {
            worker_thread.join();
        }
    }

    void start() {
        worker_thread = std::thread(&Actor::run, this);
    }

    void send(Message msg) {
        mailbox.push(std::move(msg));
    }

    void join() {
        if (worker_thread.joinable()) worker_thread.join();
    }
};

// Worker Actor handling partition of Order Book
class OrderBookWorkerActor : public Actor {
private:
    uint32_t worker_id;
    uint64_t processed_count{0}; // Isolated state! No mutex needed here.

protected:
    void process_message(const Message& msg) override {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NewOrderPayload>) {
                processed_count++;
                // Isolated state update, price-time priority processing goes here
            } else if constexpr (std::is_same_v<T, CancelOrderPayload>) {
                processed_count++;
            } else if constexpr (std::is_same_v<T, PoisonPillPayload>) {
                std::cout << "[Worker " << worker_id << "] Shutdown. Total Processed: " 
                          << processed_count << "\n";
            }
        }, msg);
    }

public:
    explicit OrderBookWorkerActor(uint32_t id) : worker_id(id) {}
};

// Order Router Actor: Routes messages based on symbol % worker_count
class OrderRouterActor : public Actor {
private:
    std::vector<std::unique_ptr<OrderBookWorkerActor>> workers;

protected:
    void process_message(const Message& msg) override {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, NewOrderPayload>) {
                uint32_t target = arg.symbol % workers.size();
                workers[target]->send(arg);
            } else if constexpr (std::is_same_v<T, CancelOrderPayload>) {
                uint32_t target = arg.symbol % workers.size();
                workers[target]->send(arg);
            } else if constexpr (std::is_same_v<T, PoisonPillPayload>) {
                for (auto& worker : workers) {
                    worker->send(PoisonPillPayload{});
                }
            }
        }, msg);
    }

public:
    OrderRouterActor(size_t num_workers) {
        for (size_t i = 0; i < num_workers; ++i) {
            workers.push_back(std::make_unique<OrderBookWorkerActor>(static_cast<uint32_t>(i)));
            workers.back()->start();
        }
    }
};

// Producer function to test concurrent dispatch load
void producer_task(OrderRouterActor& router, uint32_t producer_id, int num_orders) {
    for (int i = 0; i < num_orders; ++i) {
        NewOrderPayload order;
        order.order_id = (static_cast<uint64_t>(producer_id) << 32) | i;
        order.symbol = static_cast<uint32_t>(i % 10); // Symbols 0..9 hashed across workers
        order.price = 100.0 + (i % 50);
        order.quantity = 10;
        order.side = i % 2;

        router.send(order);
    }
}

int main() {
    const size_t NUM_WORKERS = 4;
    const int NUM_PRODUCERS = 4;
    const int ORDERS_PER_PRODUCER = 10000;

    OrderRouterActor router(NUM_WORKERS);
    router.start();

    std::vector<std::thread> producers;
    producers.reserve(NUM_PRODUCERS);

    // Multi-Producer Stress Test
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer_task, std::ref(router), static_cast<uint32_t>(i), ORDERS_PER_PRODUCER);
    }

    for (auto& t : producers) {
        t.join();
    }

    // Graceful Shutdown
    router.send(PoisonPillPayload{});
    router.join();
    return 0;
}