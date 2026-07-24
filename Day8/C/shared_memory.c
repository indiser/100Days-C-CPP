#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<memoryapi.h>

// open of the file
void *win32_file_operations(const char *filename, size_t *filesize, HANDLE *hFile, HANDLE *hMap)
{
    // Created File Handler pointer
    *hFile = CreateFileA(filename, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(*hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error opening file: %lu\n", GetLastError());
        return NULL;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(*hFile, &size)) 
    {
        fprintf(stderr, "Error getting file size: %lu\n", GetLastError());
        CloseHandle(*hFile);
        return NULL;
    }
    // Got the file size
    *filesize = (size_t)size.QuadPart;

    if (*filesize == 0)
    {
        fprintf(stderr, "Error broken file: %lu\n", GetLastError());
        CloseHandle(*hFile);
        return NULL;
    }

    // Created The mapper
    *hMap = CreateFileMappingA(*hFile, NULL, PAGE_READONLY, 0, 0, NULL);

    if(*hMap == NULL)
    {
        fprintf(stderr, "Error creating file mapping: %lu\n", GetLastError());
        CloseHandle(*hFile);
        return NULL;
    }

    // Created MapView
    void *data = MapViewOfFile(*hMap, FILE_MAP_READ, 0, 0, 0);

    if(data == NULL)
    {
        fprintf(stderr, "Error mapping view of file: %lu\n", GetLastError());
        CloseHandle(*hMap);
        CloseHandle(*hFile);
        return NULL;
    }

    WIN32_MEMORY_RANGE_ENTRY range;
    range.VirtualAddress = data;
    range.NumberOfBytes = *filesize;
    PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0);

    return data;
}

// close file
void win32_unmap_close(void *data, HANDLE hMap, HANDLE hFile)
{
    if(data) UnmapViewOfFile(data);
    if(hMap) CloseHandle(hMap);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}


// Grep type seaarch using pointer arithmatic
void grep_buffer(const char *buf, size_t size, const char *target)
{
    size_t target_len = strlen(target);
    if(target_len == 0 || size < target_len) return;

    const char *ptr = buf;
    const char *end = buf + size - target_len;
    size_t matches = 0;

    const char *line_start = buf;
    size_t line_no = 1;

    while(ptr <= end)
    {
        if(*ptr == '\n')
        {
            line_no++;
            line_start = ptr + 1;
        }
        if(memcmp(ptr, target, target_len) == 0)
        {
            matches++;
            const char *line_end = memchr(line_start, '\n', (buf + size) - line_start);
            if(!line_end) line_end = buf + size;
            printf("%zu: %.*s\n", line_no, (int)(line_end - line_start), line_start);
            ptr += target_len;
        }
        else ptr++;
    }

    printf("Found '%s' %zu times\n", target, matches);
}


int main(int argc, char *argv[])
{
    const char *filename = "../All's Well That Ends Well.txt";
    const char *target = "king";

    size_t fileSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;

    void *data = win32_file_operations(filename, &fileSize, &hFile, &hMap);

    if(!data) return -1;

    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);

    // fwrite(data, 1, fileSize, stdout);
    QueryPerformanceCounter(&t0);
    grep_buffer(data, fileSize, target);
    QueryPerformanceCounter(&t1);

    printf("mmap grep: %.3f ms\n", (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart);

    win32_unmap_close(data, hMap, hFile);

    return 0;
}