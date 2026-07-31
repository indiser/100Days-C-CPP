#include <iostream>
#include <cstring>
#include <cstddef>
#include <cassert>

class CowString {
private:
    struct StringData {
        char* data;
        size_t length;
        size_t ref_count;

        StringData(const char* str) {
            length = std::strlen(str);
            data = new char[length + 1];
            std::memcpy(data, str, length + 1);
            ref_count = 1;
        }

        ~StringData() {
            delete[] data;
        }
    };

    StringData* buffer_;

    void detach_if_shared() {
        if (!buffer_) return;
        if (buffer_->ref_count > 1) {
            StringData* new_buf = new StringData(buffer_->data);
            buffer_->ref_count--;
            buffer_ = new_buf;
        }
    }

public:
    explicit CowString(const char* str = "") {
        buffer_ = new StringData(str);
    }

    CowString(const CowString& other) : buffer_(other.buffer_) {
        if (buffer_) {
            buffer_->ref_count++;
        }
    }

    CowString& operator=(const CowString& other) {
        if (this != &other) {
            if (buffer_) {
                buffer_->ref_count--;
                if (buffer_->ref_count == 0) {
                    delete buffer_;
                }
            }
            buffer_ = other.buffer_;
            if (buffer_) {
                buffer_->ref_count++;
            }
        }
        return *this;
    }

    ~CowString() {
        if (buffer_) {
            buffer_->ref_count--;
            if (buffer_->ref_count == 0) {
                delete buffer_;
            }
        }
    }

    char operator[](size_t index) const {
        assert(buffer_ && index < buffer_->length);
        return buffer_->data[index];
    }

    char& operator[](size_t index) {
        assert(buffer_ && index < buffer_->length);
        detach_if_shared();
        return buffer_->data[index];
    }

    size_t ref_count() const { return buffer_ ? buffer_->ref_count : 0; }
    size_t length() const { return buffer_ ? buffer_->length : 0; }
    const char* c_str() const { return buffer_ ? buffer_->data : ""; }
};

int main() {
    CowString s1("hello");
    CowString s2 = s1;

    std::cout << "s1: " << s1.c_str() << " (refs: " << s1.ref_count() << ")\n";
    std::cout << "s2: " << s2.c_str() << " (refs: " << s2.ref_count() << ")\n";

    // Read via const reference -> call const operator[] -> no detach
    const CowString& s2_const = s2;
    std::cout << "s2[0] read: " << s2_const[0] << "\n";
    assert(s1.ref_count() == 2); // PASS

    // Write via mutable operator[] -> call non-const operator[] -> detach
    s2[0] = 'J';

    std::cout << "\nAfter write to s2[0]:\n";
    std::cout << "s1: " << s1.c_str() << " (refs: " << s1.ref_count() << ")\n";
    std::cout << "s2: " << s2.c_str() << " (refs: " << s2.ref_count() << ")\n";
    assert(s1.c_str() != s2.c_str());

    return 0;
}