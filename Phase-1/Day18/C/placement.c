#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

typedef struct {
    int id;
    float x, y;
} Entity;

void entity_init(Entity* e, int id, float x, float y) {
    e->id = id;
    e->x = x;
    e->y = y;
}

void entity_destroy(Entity* e) {
    // Cleanup internal resources if any
    e->id = -1;
}

int main(void) {
    alignas(alignof(Entity)) uint8_t arena[sizeof(Entity) * 4];
    
    // Construct inside raw buffer
    Entity* e0 = (Entity*)&arena[0 * sizeof(Entity)];
    entity_init(e0, 101, 1.0f, 2.0f);

    printf("C Entity 0: ID=%d POS=(%.1f, %.1f)\n", e0->id, e0->x, e0->y);

    entity_destroy(e0);
    return 0;
}