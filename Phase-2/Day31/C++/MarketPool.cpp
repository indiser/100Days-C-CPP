#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <system_error>

struct Tick {
    std::string symbol;
    double price;
    int id;
};

class WorkerPool {
private:
    std::vector<Tick> queue;
    size_t current_idx{0};
    std::mutex queue_mutex;
    std::mutex print_mutex; // Serializes output stream access
    std::vector<std::thread> workers;

    void worker_loop(size_t id) {
        while (true) {
            Tick t;
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (current_idx >= queue.size()) {
                    break;
                }
                t = queue[current_idx++];
            }
            
            // Format off-stream to minimize print-lock hold time
            std::ostringstream ss;
            ss << "Worker " << id << " processed tick " << t.id 
               << ": " << t.symbol << " @ $" << t.price << "\n";

            {
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << ss.str();
            }
        }
    }

public:
    explicit WorkerPool(std::vector<Tick> ticks, size_t num_workers) 
        : queue(std::move(ticks)) 
    {
        workers.reserve(num_workers);
        try {
            for (size_t i = 0; i < num_workers; ++i) {
                workers.emplace_back(&WorkerPool::worker_loop, this, i);
            }
        } catch (...) {
            join_all();
            throw;
        }
    }

    ~WorkerPool() {
        join_all();
    }

    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;
    WorkerPool(WorkerPool&&) = delete;
    WorkerPool& operator=(WorkerPool&&) = delete;

private:
    void join_all() noexcept {
        for (auto& t : workers) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};

int main() {
    try {
        std::vector<Tick> ticks;
        ticks.reserve(12);
        for (int i = 0; i < 12; ++i) {
            ticks.push_back({"AAPL", 150.0 + i, i + 1});
        }

        WorkerPool pool(std::move(ticks), 4);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "All ticks processed cleanly. Exit.\n";
    return 0;
}