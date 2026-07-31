#include <cstddef>

class CowString {
private:
    struct StringData {
        char* data;
        size_t length;
        size_t ref_count;
    };
    StringData* buffer_;

    void detach_if_shared(); // Allocates fresh clone if ref_count > 1

public:
    explicit CowString(const char* str = "");
    CowString(const CowString& other);            // Cheap: bump ref_count
    CowString& operator=(const CowString& other); // Cheap: bump ref_count
    ~CowString();

    char operator[](size_t index) const; // Read: no detach
    char& operator[](size_t index);      // Write: calls detach_if_shared()
    
    size_t ref_count() const { return buffer_ ? buffer_->ref_count : 0; }
    const char* c_str() const { return buffer_->data; }
};