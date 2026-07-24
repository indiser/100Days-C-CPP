#include <windows.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <vector>

class FileBuffer
{
public:
    FileBuffer(const std::string &filename)
    {
        FILE *fp = fopen(filename.c_str(), "rb");
        if(!fp)
            throw std::runtime_error("Error opening file");

        if(fseek(fp, 0, SEEK_END) != 0)
        {
            fclose(fp);
            throw std::runtime_error("Error seeking file");
        }

        long size = ftell(fp);
        if(size <= 0)
        {
            fclose(fp);
            throw std::runtime_error("Error broken file or empty");
        }
        size_ = static_cast<size_t>(size);

        fseek(fp, 0, SEEK_SET);

        buf_.resize(size_);
        size_t read_bytes = fread(buf_.data(), 1, size_, fp);
        fclose(fp);

        if(read_bytes != size_)
            throw std::runtime_error("Error reading full file");
    }

    // copy ok — plain heap buffer, no OS handle
    FileBuffer(const FileBuffer &) = default;
    FileBuffer &operator=(const FileBuffer &) = default;
    FileBuffer(FileBuffer &&) noexcept = default;
    FileBuffer &operator=(FileBuffer &&) noexcept = default;
    ~FileBuffer() = default;

    const char *data() const { return buf_.data(); }
    size_t size() const { return size_; }

private:
    std::vector<char> buf_;
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
        FileBuffer fb(filename);

        LARGE_INTEGER freq, t0, t1;
        QueryPerformanceFrequency(&freq);

        QueryPerformanceCounter(&t0);
        grep_buffer(fb.data(), fb.size(), target);
        QueryPerformanceCounter(&t1);

        printf("read() grep (RAII): %.3f ms\n",
               (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart);
    }
    catch(const std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}