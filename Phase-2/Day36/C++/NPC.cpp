#include <iostream>
#include <coroutine>
#include <string>
#include <vector>
#include <optional>

// ---- minimal Generator<T> wrapping C++20 coroutine machinery ----
template <typename T>
struct Generator {
    struct promise_type {
        T current_value;
        bool done = false;

        Generator get_return_object() {
            return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        void return_void() { done = true; }
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;
    handle_type h;

    explicit Generator(handle_type h_) : h(h_) {}
    Generator(Generator&& other) noexcept : h(other.h) { other.h = nullptr; }
    Generator(const Generator&) = delete;
    ~Generator() { if (h) h.destroy(); }

    bool finished() {
        return h.done();
    }

    // resume coroutine one step, return current yielded value if any
    std::optional<T> next() {
        if (!h || h.done()) return std::nullopt;
        h.resume();
        if (h.done()) return std::nullopt;
        return h.promise().current_value;
    }
};

// ---- NPC script as a coroutine, co_yield each action ----
Generator<std::string> npc_script(std::string name) {
    co_yield name + ": waking up";
    co_yield name + ": moving to point A";
    co_yield name + ": attacking";
    co_yield name + ": done";
    co_return;
}

int main() {
    std::vector<Generator<std::string>> npcs;
    npcs.push_back(npc_script("Goblin"));
    npcs.push_back(npc_script("Archer"));
    npcs.push_back(npc_script("Wizard"));

    int tick = 0;
    bool any_active = true;

    while (any_active) {
        std::cout << "--- tick " << tick++ << " ---\n";
        any_active = false;
        for (auto& npc : npcs) {
            if (!npc.finished()) {
                auto val = npc.next();
                if (val) {
                    std::cout << *val << "\n";
                    any_active = true;
                }
            }
        }
    }

    std::cout << "All NPCs finished.\n";
    return 0;
}