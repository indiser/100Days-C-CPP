#include <iostream>
#include <vector>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>

constexpr std::size_t FRAME_SIZE = 256;
constexpr std::size_t QUEUE_CAPACITY = 4;

struct AudioFrame {
    std::array<float, FRAME_SIZE> data{};
    bool is_last{false};
};

template <typename T, std::size_t Capacity>
class BoundedPipelineQueue {
private:
    std::array<T, Capacity> buffer_{};
    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t count_{0};
    
    mutable std::mutex lock_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;

public:
    BoundedPipelineQueue() = default;
    ~BoundedPipelineQueue() = default;

    // Disallow copies
    BoundedPipelineQueue(const BoundedPipelineQueue&) = delete;
    BoundedPipelineQueue& operator=(const BoundedPipelineQueue&) = delete;

    void push(T item) {
        std::unique_lock<std::mutex> ul(lock_);
        not_full_.wait(ul, [this] { return count_ < Capacity; });
        
        buffer_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % Capacity;
        ++count_;
        
        ul.unlock();
        not_empty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> ul(lock_);
        not_empty_.wait(ul, [this] { return count_ > 0; });
        
        T item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % Capacity;
        --count_;
        
        ul.unlock();
        not_full_.notify_one();
        return item;
    }
};

using FrameQueue = BoundedPipelineQueue<AudioFrame, QUEUE_CAPACITY>;

void stage1_producer(FrameQueue& out_q, int total_frames) {
    float phase = 0.0f;
    for (int i = 0; i < total_frames; ++i) {
        AudioFrame frame;
        frame.is_last = false;
        for (std::size_t s = 0; s < FRAME_SIZE; ++s) {
            frame.data[s] = std::sin(phase);
            phase += 0.05f;
        }
        out_q.push(std::move(frame));
    }

    AudioFrame poison_pill;
    poison_pill.is_last = true;
    out_q.push(std::move(poison_pill));
}

void stage2_filter(FrameQueue& in_q, FrameQueue& out_q, float gain) {
    while (true) {
        AudioFrame frame = in_q.pop();
        if (frame.is_last) {
            out_q.push(std::move(frame));
            break;
        }
        for (float& sample : frame.data) {
            sample *= gain;
        }
        out_q.push(std::move(frame));
    }
}

void stage3_consumer(FrameQueue& in_q) {
    int consumed = 0;
    while (true) {
        AudioFrame frame = in_q.pop();
        if (frame.is_last) break;
        ++consumed;
    }
    std::cout << "C++ Pipeline: consumed " << consumed << " frames successfully.\n";
}

int main() {
    FrameQueue q1;
    FrameQueue q2;

    constexpr int TOTAL_FRAMES = 100000;
    constexpr float GAIN = 1.5f;

    std::thread t1(stage1_producer, std::ref(q1), TOTAL_FRAMES);
    std::thread t2(stage2_filter, std::ref(q1), std::ref(q2), GAIN);
    std::thread t3(stage3_consumer, std::ref(q2));

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Clean thread exit.\n";
    return 0;
}