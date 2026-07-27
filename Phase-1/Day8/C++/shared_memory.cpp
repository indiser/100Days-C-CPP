#include <windows.h>
#include <memoryapi.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <utility>

class MappedFile
{
public:
    MappedFile(const std::string &filename)
    {
        hFile_ = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile_ == INVALID_HANDLE_VALUE)
            throw std::runtime_error("Error opening file: " + std::to_string(GetLastError()));

        LARGE_INTEGER size;
        if(!GetFileSizeEx(hFile_, &size))
        {
            CloseHandle(hFile_);
            throw std::runtime_error("Error getting file size: " + std::to_string(GetLastError()));
        }
        size_ = static_cast<size_t>(size.QuadPart);

        if(size_ == 0)
        {
            CloseHandle(hFile_);
            throw std::runtime_error("Error broken file (size 0)");
        }

        hMap_ = CreateFileMappingA(hFile_, NULL, PAGE_READONLY, 0, 0, NULL);
        if(hMap_ == NULL)
        {
            CloseHandle(hFile_);
            throw std::runtime_error("Error creating file mapping: " + std::to_string(GetLastError()));
        }

        data_ = MapViewOfFile(hMap_, FILE_MAP_READ, 0, 0, 0);
        if(data_ == NULL)
        {
            CloseHandle(hMap_);
            CloseHandle(hFile_);
            throw std::runtime_error("Error mapping view of file: " + std::to_string(GetLastError()));
        }

        WIN32_MEMORY_RANGE_ENTRY range;
        range.VirtualAddress = data_;
        range.NumberOfBytes = size_;
        PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0);
    }

    // no copy — mmap handle can't copy cleanly
    MappedFile(const MappedFile &) = delete;
    MappedFile &operator=(const MappedFile &) = delete;

    // move-only
    MappedFile(MappedFile &&other) noexcept
        : hFile_(other.hFile_), hMap_(other.hMap_), data_(other.data_), size_(other.size_)
    {
        other.hFile_ = INVALID_HANDLE_VALUE;
        other.hMap_ = NULL;
        other.data_ = NULL;
        other.size_ = 0;
    }

    MappedFile &operator=(MappedFile &&other) noexcept
    {
        if(this != &other)
        {
            release();
            hFile_ = other.hFile_;
            hMap_ = other.hMap_;
            data_ = other.data_;
            size_ = other.size_;

            other.hFile_ = INVALID_HANDLE_VALUE;
            other.hMap_ = NULL;
            other.data_ = NULL;
            other.size_ = 0;
        }
        return *this;
    }

    ~MappedFile()
    {
        release();
    }

    const char *data() const { return static_cast<const char *>(data_); }
    size_t size() const { return size_; }

private:
    void release()
    {
        if(data_) UnmapViewOfFile(data_);
        if(hMap_) CloseHandle(hMap_);
        if(hFile_ != INVALID_HANDLE_VALUE) CloseHandle(hFile_);

        data_ = NULL;
        hMap_ = NULL;
        hFile_ = INVALID_HANDLE_VALUE;
        size_ = 0;
    }

    HANDLE hFile_ = INVALID_HANDLE_VALUE;
    HANDLE hMap_ = NULL;
    void *data_ = NULL;
    size_t size_ = 0;
};

void grep_buffer(const char *buf, size_t size, const std::string &target)
{
    size_t target_len = target.size();
    if(target_len == 0 || size < target_len) return;

    const char *ptr = buf;
    const char *end = buf + size - target_len;
    const char *line_start = buf;
    size_t line_no = 1;
    size_t matches = 0;

    while(ptr <= end)
    {
        if(*ptr == '\n')
        {
            line_no++;
            line_start = ptr + 1;
        }
        if(memcmp(ptr, target.c_str(), target_len) == 0)
        {
            matches++;
            const char *line_end = static_cast<const char *>(memchr(line_start, '\n', (buf + size) - line_start));
            if(!line_end) line_end = buf + size;
            printf("%zu: %.*s\n", line_no, (int)(line_end - line_start), line_start);
            ptr += target_len;
        }
        else ptr++;
    }

    printf("Found '%s' %zu times\n", target.c_str(), matches);
}

int main()
{
    const std::string filename = "../All's Well That Ends Well.txt";
    const std::string target = "king";

    try
    {
        MappedFile mf(filename);

        LARGE_INTEGER freq, t0, t1;
        QueryPerformanceFrequency(&freq);

        QueryPerformanceCounter(&t0);
        grep_buffer(mf.data(), mf.size(), target);
        QueryPerformanceCounter(&t1);

        printf("mmap grep (RAII): %.3f ms\n",
               (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart);
    }
    catch(const std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}