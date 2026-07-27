#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void grep_buffer(const char *buf, size_t size, const char *target)
{
    size_t target_len = strlen(target);
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

    FILE *fp = fopen(filename, "rb");
    if(!fp)
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    if(fileSize <= 0)
    {
        fprintf(stderr, "Error broken file or empty\n");
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc((size_t)fileSize);
    if(!buf)
    {
        fprintf(stderr, "Error allocating buffer\n");
        fclose(fp);
        return -1;
    }

    size_t read_bytes = fread(buf, 1, (size_t)fileSize, fp);
    fclose(fp);

    if(read_bytes != (size_t)fileSize)
    {
        fprintf(stderr, "Error reading full file\n");
        free(buf);
        return -1;
    }

    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&t0);
    grep_buffer(buf, (size_t)fileSize, target);
    QueryPerformanceCounter(&t1);

    printf("read() grep: %.3f ms\n", (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / freq.QuadPart);

    free(buf);
    return 0;
}