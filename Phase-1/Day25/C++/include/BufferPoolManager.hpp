#ifndef BUFFER_POOL_MANAGER_HPP
#define BUFFER_POOL_MANAGER_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

constexpr size_t PAGE_SIZE = 4096;
constexpr size_t CACHE_CAPACITY = 10;

struct DatabaseHeader {
    uint32_t page_size;
    uint32_t total_pages;
    int32_t free_list_head;
};

struct Page {
    int page_id = -1;
    uint8_t data[PAGE_SIZE]{};
    int pin_count = 0;
    bool is_dirty = false;
};

class BufferPoolManager {
private:
    int fd_;
    size_t capacity_;
    uint64_t pwrite_count_ = 0;

    std::vector<Page> pages_;
    std::list<size_t> free_frames_;

    std::unordered_map<int, size_t> page_table_;
    std::list<size_t> lru_list_;
    std::unordered_map<size_t, std::list<size_t>::iterator> lru_map_;

    void UnlinkLRU(size_t frame_id) {
        if (lru_map_.count(frame_id)) {
            lru_list_.erase(lru_map_[frame_id]);
            lru_map_.erase(frame_id);
        }
    }

    void TouchLRU(size_t frame_id) {
        UnlinkLRU(frame_id);
        lru_list_.push_front(frame_id);
        lru_map_[frame_id] = lru_list_.begin();
    }

public:
    explicit BufferPoolManager(const std::string &filename, size_t capacity = CACHE_CAPACITY);
    ~BufferPoolManager();

    BufferPoolManager(const BufferPoolManager &) = delete;
    BufferPoolManager &operator=(const BufferPoolManager &) = delete;

    Page *FetchPage(int page_id);
    void UnpinPage(int page_id, bool is_dirty);
    void FlushPage(int page_id);
    void FlushAllPages();

    int AllocPage();
    void FreePage(int page_id);

    void DestroyNoFlush() {
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
        page_table_.clear();
        lru_list_.clear();
        lru_map_.clear();
        pages_.clear();
    }

    int GetFrameId(int page_id) const {
        auto it = page_table_.find(page_id);
        if (it != page_table_.end()) return static_cast<int>(it->second);
        return -1; // Not in cache
    }

    uint64_t GetPwriteCount() const { return pwrite_count_; }
};

#endif