#include "BufferPoolManager.hpp"

BufferPoolManager::BufferPoolManager(const std::string &filename, size_t capacity)
    : capacity_(capacity), pages_(capacity) {
    fd_ = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd_ < 0) throw std::runtime_error("Failed to open file");

    for (size_t i = 0; i < capacity_; ++i) {
        free_frames_.push_back(i);
    }

    off_t file_len = lseek(fd_, 0, SEEK_END);
    if (file_len == 0) {
        Page *hdr_page = FetchPage(0);
        auto *hdr = reinterpret_cast<DatabaseHeader *>(hdr_page->data);
        hdr->page_size = PAGE_SIZE;
        hdr->total_pages = 1;
        hdr->free_list_head = -1;
        UnpinPage(0, true);
        FlushPage(0);
    }
}

BufferPoolManager::~BufferPoolManager() {
    if (fd_ >= 0) {
        FlushAllPages();
        close(fd_);
    }
}

Page *BufferPoolManager::FetchPage(int page_id) {
    if (page_table_.count(page_id)) {
        size_t frame_id = page_table_[page_id];
        Page &page = pages_[frame_id];
        page.pin_count++;
        TouchLRU(frame_id);
        return &page;
    }

    size_t frame_id;
    if (!free_frames_.empty()) {
        frame_id = free_frames_.front();
        free_frames_.pop_front();
    } else {
        bool evicted = false;
        for (auto it = lru_list_.rbegin(); it != lru_list_.rend(); ++it) {
            size_t fid = *it;
            if (pages_[fid].pin_count == 0) {
                frame_id = fid;
                FlushPage(pages_[frame_id].page_id);
                page_table_.erase(pages_[frame_id].page_id);
                UnlinkLRU(frame_id);
                evicted = true;
                break;
            }
        }
        if (!evicted) return nullptr; // All frames pinned!
    }

    Page &page = pages_[frame_id];
    page.page_id = page_id;
    page.pin_count = 1;
    page.is_dirty = false;

    off_t offset = static_cast<off_t>(page_id) * PAGE_SIZE;
    std::memset(page.data, 0, PAGE_SIZE);
    ssize_t bytes_read = pread(fd_, page.data, PAGE_SIZE, offset);
    (void)bytes_read;

    page_table_[page_id] = frame_id;
    TouchLRU(frame_id);
    return &page;
}

void BufferPoolManager::UnpinPage(int page_id, bool is_dirty) {
    if (!page_table_.count(page_id)) return;
    size_t frame_id = page_table_[page_id];
    Page &page = pages_[frame_id];

    if (is_dirty) page.is_dirty = true;
    if (page.pin_count > 0) page.pin_count--;
}

void BufferPoolManager::FlushPage(int page_id) {
    if (!page_table_.count(page_id)) return;
    size_t frame_id = page_table_[page_id];
    Page &page = pages_[frame_id];

    if (!page.is_dirty) return;

    off_t offset = static_cast<off_t>(page_id) * PAGE_SIZE;
    ssize_t written = pwrite(fd_, page.data, PAGE_SIZE, offset);
    if (written == static_cast<ssize_t>(PAGE_SIZE)) {
        pwrite_count_++;
        page.is_dirty = false;
    }
}

void BufferPoolManager::FlushAllPages() {
    for (const auto &[pid, fid] : page_table_) {
        FlushPage(pid);
    }
}

int BufferPoolManager::AllocPage() {
    Page *hdr_page = FetchPage(0);
    auto *hdr = reinterpret_cast<DatabaseHeader *>(hdr_page->data);

    int new_page_id;
    if (hdr->free_list_head != -1) {
        new_page_id = hdr->free_list_head;
        Page *free_page = FetchPage(new_page_id);
        
        int32_t next_free;
        std::memcpy(&next_free, free_page->data, sizeof(int32_t));
        hdr->free_list_head = next_free;
        
        UnpinPage(new_page_id, false);
    } else {
        new_page_id = hdr->total_pages++;
    }

    UnpinPage(0, true);
    return new_page_id;
}

void BufferPoolManager::FreePage(int page_id) {
    if (page_id <= 0) return;

    Page *hdr_page = FetchPage(0);
    auto *hdr = reinterpret_cast<DatabaseHeader *>(hdr_page->data);

    Page *target = FetchPage(page_id);
    std::memset(target->data, 0, PAGE_SIZE);
    std::memcpy(target->data, &hdr->free_list_head, sizeof(int32_t));

    hdr->free_list_head = page_id;

    UnpinPage(page_id, true);
    UnpinPage(0, true);
}