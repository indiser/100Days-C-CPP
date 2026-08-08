#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>
#include <unordered_map>
#include <random>
#include <optional>
#include <algorithm>

// ---------- Open-addressing hash map, template over key/value ----------

template <typename K, typename V>
class SymbolTable
{
    enum class State : uint8_t { EMPTY, OCCUPIED, DELETED };

    struct Slot
    {
        K key{};
        V value{};
        State state = State::EMPTY;
    };

    std::vector<Slot> slots_;
    size_t capacity_;
    size_t count_ = 0;
    size_t tombstones_ = 0;

    static constexpr double MAX_LOAD = 0.7;

    static uint32_t hashKey(const K &key)
    {
        // FNV-1a over the key's bytes (works for std::string)
        uint32_t hash = 0x811C9DC5u;
        const uint32_t prime = 0x01000193u;
        for (unsigned char c : key)
        {
            hash ^= c;
            hash *= prime;
        }
        return hash;
    }

    void insertRaw(std::vector<Slot> &slots, size_t capacity, const K &key, const V &value)
    {
        uint32_t mask = static_cast<uint32_t>(capacity - 1);
        uint32_t index = hashKey(key) & mask;
        uint32_t start = index;

        for (;;)
        {
            Slot &s = slots[index];
            if (s.state != State::OCCUPIED)
            {
                s.key = key;
                s.value = value;
                s.state = State::OCCUPIED;
                return;
            }
            index = (index + 1) & mask;
            if (index == start) return; // shouldn't happen, resized before full
        }
    }

    void resize(size_t newCapacity)
    {
        std::vector<Slot> newSlots(newCapacity);
        for (auto &s : slots_)
        {
            if (s.state == State::OCCUPIED)
                insertRaw(newSlots, newCapacity, s.key, s.value);
        }
        slots_ = std::move(newSlots);
        capacity_ = newCapacity;
        tombstones_ = 0;
    }

public:
    explicit SymbolTable(size_t initialCapacity = 16)
        : slots_(initialCapacity), capacity_(initialCapacity) {}

    bool insert(const K &key, const V &value)
    {
        double load = static_cast<double>(count_ + tombstones_ + 1) / capacity_;
        if (load > MAX_LOAD) resize(capacity_ * 2);

        uint32_t mask = static_cast<uint32_t>(capacity_ - 1);
        uint32_t index = hashKey(key) & mask;
        uint32_t start = index;
        long firstTombstone = -1;

        for (;;)
        {
            Slot &s = slots_[index];

            if (s.state == State::EMPTY)
            {
                size_t dest = (firstTombstone != -1) ? static_cast<size_t>(firstTombstone) : index;
                Slot &d = slots_[dest];
                bool wasTombstone = (d.state == State::DELETED);
                d.key = key;
                d.value = value;
                d.state = State::OCCUPIED;
                count_++;
                if (wasTombstone) tombstones_--;
                return true;
            }

            if (s.state == State::DELETED)
            {
                if (firstTombstone == -1) firstTombstone = index;
            }
            else if (s.key == key)
            {
                s.value = value; // update
                return true;
            }

            index = (index + 1) & mask;
            if (index == start) return false;
        }
    }

    std::optional<V> lookup(const K &key) const
    {
        uint32_t mask = static_cast<uint32_t>(capacity_ - 1);
        uint32_t index = hashKey(key) & mask;
        uint32_t start = index;

        for (;;)
        {
            const Slot &s = slots_[index];
            if (s.state == State::EMPTY) return std::nullopt;
            if (s.state == State::OCCUPIED && s.key == key) return s.value;
            index = (index + 1) & mask;
            if (index == start) return std::nullopt;
        }
    }

    bool remove(const K &key)
    {
        uint32_t mask = static_cast<uint32_t>(capacity_ - 1);
        uint32_t index = hashKey(key) & mask;
        uint32_t start = index;

        for (;;)
        {
            Slot &s = slots_[index];
            if (s.state == State::EMPTY) return false;
            if (s.state == State::OCCUPIED && s.key == key)
            {
                s.state = State::DELETED;
                count_--;
                tombstones_++;
                return true;
            }
            index = (index + 1) & mask;
            if (index == start) return false;
        }
    }

    size_t size() const { return count_; }
    size_t capacity() const { return capacity_; }
};

// ---------- correctness smoke test ----------

static void smokeTest()
{
    SymbolTable<std::string, double> t;

    t.insert("AAPL", 182.50);
    t.insert("NVDA", 721.33);
    t.insert("TSLA", 193.57);
    t.insert("AAPL", 185.00); // update

    if (auto p = t.lookup("AAPL")) std::cout << "AAPL price: " << *p << "\n";
    if (auto p = t.lookup("NVDA")) std::cout << "NVDA price: " << *p << "\n";
    if (!t.lookup("MSFT")) std::cout << "MSFT not found\n";

    t.remove("NVDA");
    if (!t.lookup("NVDA")) std::cout << "NVDA deleted, not found\n";

    for (int i = 0; i < 50; i++)
        t.insert("S" + std::to_string(i), i * 1.5);

    std::cout << "capacity after growth: " << t.capacity() << ", count: " << t.size() << "\n";
    if (auto p = t.lookup("AAPL")) std::cout << "AAPL still found after resize: " << *p << "\n";
}

// ---------- benchmark harness ----------

static std::vector<std::string> makeSymbols(size_t n)
{
    std::vector<std::string> syms;
    syms.reserve(n);
    for (size_t i = 0; i < n; i++)
        syms.push_back("SYM" + std::to_string(i));
    return syms;
}

template <typename Fn>
static double timeIt(Fn &&fn, int reps = 5)
{
    using clock = std::chrono::steady_clock;
    std::vector<double> samples;
    for (int r = 0; r < reps; r++)
    {
        auto start = clock::now();
        fn();
        auto end = clock::now();
        samples.push_back(std::chrono::duration<double, std::micro>(end - start).count());
    }
    double mean = 0;
    for (double s : samples) mean += s;
    mean /= samples.size();
    return mean; // microseconds
}

static void benchmark()
{
    const size_t N = 100'000;
    auto symbols = makeSymbols(N);

    std::cout << "\n--- benchmark: N=" << N << " ---\n";

    // custom table insert
    double customInsert = timeIt([&]() {
        SymbolTable<std::string, double> t(16);
        for (size_t i = 0; i < N; i++) t.insert(symbols[i], (double)i);
    });

    // unordered_map insert
    double umapInsert = timeIt([&]() {
        std::unordered_map<std::string, double> m;
        for (size_t i = 0; i < N; i++) m[symbols[i]] = (double)i;
    });

    // pre-fill for lookup benchmark
    SymbolTable<std::string, double> t;
    std::unordered_map<std::string, double> m;
    for (size_t i = 0; i < N; i++)
    {
        t.insert(symbols[i], (double)i);
        m[symbols[i]] = (double)i;
    }

    std::mt19937 rng(42);
    std::shuffle(symbols.begin(), symbols.end(), rng);

    double customLookup = timeIt([&]() {
        volatile double sink = 0;
        for (auto &s : symbols) sink += *t.lookup(s);
    });

    double umapLookup = timeIt([&]() {
        volatile double sink = 0;
        for (auto &s : symbols) sink += m.at(s);
    });

    std::cout << "insert  custom: " << customInsert << " us   unordered_map: " << umapInsert << " us\n";
    std::cout << "lookup  custom: " << customLookup << " us   unordered_map: " << umapLookup << " us\n";
}

int main()
{
    smokeTest();
    benchmark();
    return 0;
}