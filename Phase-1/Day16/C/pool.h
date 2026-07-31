#include <stddef.h>

typedef struct {
    char **buckets;
    size_t capacity;
    size_t count;
} StringPool;

StringPool* pool_create(size_t capacity);
const char* pool_intern(StringPool *pool, const char *str);
void pool_destroy(StringPool *pool);