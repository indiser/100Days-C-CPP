#ifndef PAGE_GUARD_HPP
#define PAGE_GUARD_HPP

#include "BufferPoolManager.hpp"

class PageGuard {
private:
    BufferPoolManager *bpm_ = nullptr;
    Page *page_ = nullptr;
    bool is_dirty_ = false;

public:
    PageGuard() = default;
    PageGuard(BufferPoolManager *bpm, Page *page) : bpm_(bpm), page_(page) {}

    ~PageGuard() {
        Drop();
    }

    // Move-only semantics
    PageGuard(const PageGuard &) = delete;
    PageGuard &operator=(const PageGuard &) = delete;

    PageGuard(PageGuard &&other) noexcept {
        bpm_ = other.bpm_;
        page_ = other.page_;
        is_dirty_ = other.is_dirty_;
        other.bpm_ = nullptr;
        other.page_ = nullptr;
        other.is_dirty_ = false;
    }

    PageGuard &operator=(PageGuard &&other) noexcept {
        if (this != &other) {
            Drop();
            bpm_ = other.bpm_;
            page_ = other.page_;
            is_dirty_ = other.is_dirty_;
            other.bpm_ = nullptr;
            other.page_ = nullptr;
            other.is_dirty_ = false;
        }
        return *this;
    }

    void MarkDirty() { is_dirty_ = true; }

    uint8_t *GetData() { return page_ ? page_->data : nullptr; }
    int GetPageId() const { return page_ ? page_->page_id : -1; }

    void Drop() {
        if (bpm_ && page_) {
            bpm_->UnpinPage(page_->page_id, is_dirty_);
            bpm_ = nullptr;
            page_ = nullptr;
            is_dirty_ = false;
        }
    }
};

#endif