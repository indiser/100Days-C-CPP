#include "BufferPoolManager.hpp"
#include "PageGuard.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <random>

PageGuard FetchGuard(BufferPoolManager &bpm, int page_id) {
    Page *p = bpm.FetchPage(page_id);
    return PageGuard(&bpm, p);
}

void TestPinPreventsEviction() {
    std::remove("cpp_pin.db");
    BufferPoolManager bpm("cpp_pin.db", 3); // Capacity = 3

    int p1 = bpm.AllocPage(); // 1
    int p2 = bpm.AllocPage(); // 2
    int p3 = bpm.AllocPage(); // 3

    // Pin page 1 via guard
    PageGuard g1 = FetchGuard(bpm, p1);
    int p1_frame_before = bpm.GetFrameId(p1);
    assert(p1_frame_before != -1);

    // Unpin 2 and 3
    {
        PageGuard g2 = FetchGuard(bpm, p2);
        PageGuard g3 = FetchGuard(bpm, p3);
    }

    // Force evictions with 2 new pages
    int p4 = bpm.AllocPage();
    int p5 = bpm.AllocPage();
    {
        PageGuard g4 = FetchGuard(bpm, p4);
        PageGuard g5 = FetchGuard(bpm, p5);
    }

    // Assert Page 1 NEVER got evicted from its frame
    int p1_frame_after = bpm.GetFrameId(p1);
    assert(p1_frame_after == p1_frame_before);

    // Assert Page 2 OR 3 got evicted
    assert(bpm.GetFrameId(p2) == -1 || bpm.GetFrameId(p3) == -1);

    std::cout << "[PASS] C++ Frame-Lock Pin Eviction Safety Assert\n";
    std::remove("cpp_pin.db");
}

void TestCleanVsDirtyPwriteCount() {
    std::remove("cpp_metric.db");
    BufferPoolManager bpm("cpp_metric.db", 5);

    // Alloc and dirty 5 pages
    for (int i = 0; i < 5; ++i) {
        int pid = bpm.AllocPage();
        PageGuard g = FetchGuard(bpm, pid);
        g.MarkDirty();
    }

    uint64_t baseline_writes = bpm.GetPwriteCount();

    // Fetch 5 clean pages beyond capacity to force evictions
    for (int i = 100; i < 105; ++i) {
        PageGuard g = FetchGuard(bpm, i);
        // Leave clean (don't mark dirty)
    }

    // Force one more clean eviction
    PageGuard overflow = FetchGuard(bpm, 999);

    // Clean evictions must NOT increase pwrite count
    assert(bpm.GetPwriteCount() == baseline_writes + 5); 
    std::cout << "[PASS] C++ Clean vs Dirty Pwrite Count Metric\n";
    std::remove("cpp_metric.db");
}

void TestRandomThrashing() {
    std::remove("cpp_thrash.db");
    BufferPoolManager bpm("cpp_thrash.db", 10);

    for (int i = 0; i < 30; ++i) {
        bpm.AllocPage();
    }

    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> dist(1, 30);

    for (int i = 0; i < 500; ++i) {
        int target = dist(rng);
        PageGuard g = FetchGuard(bpm, target);
        g.GetData()[0] = static_cast<uint8_t>(target);
        g.MarkDirty();
    }

    std::cout << "[PASS] C++ Random Thrashing Stress Test Passed\n";
    std::remove("cpp_thrash.db");
}

void TestCrashMidWriteIntegrity() {
    std::remove("cpp_crash.db");
    BufferPoolManager *bpm = new BufferPoolManager("cpp_crash.db");

    int pid = bpm->AllocPage();
    {
        PageGuard g = FetchGuard(*bpm, pid);
        std::strcpy(reinterpret_cast<char *>(g.GetData()), "UNCOMMITTED_CRASH_DATA");
        g.MarkDirty();
    } // Unpinned as dirty, but NO explicit flush performed.

    // Simulate sudden process crash (destructor bypassed, memory dropped)
    bpm->DestroyNoFlush();
    delete bpm;

    // Reopen database from disk
    BufferPoolManager bpm_recovered("cpp_crash.db");
    PageGuard recovered_guard = FetchGuard(bpm_recovered, pid);
    
    // Unflushed dirty data must NOT exist on disk
    assert(std::strcmp(reinterpret_cast<const char *>(recovered_guard.GetData()), "UNCOMMITTED_CRASH_DATA") != 0);
    
    std::cout << "[PASS] C++ Mid-Write Crash Safety (Unflushed Dirty Loss Confirmed)\n";
    std::remove("cpp_crash.db");
}

int main() {
    TestPinPreventsEviction();
    TestCleanVsDirtyPwriteCount();
    TestRandomThrashing();
    TestCrashMidWriteIntegrity();
    return 0;
}