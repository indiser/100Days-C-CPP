#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <string.h>

#define STACK_SIZE 65536
#define MAX_NPCS 3

typedef struct {
    ucontext_t ctx;
    char stack[STACK_SIZE];
    char name[32];
    int done;
} NPC;

ucontext_t ctx_main;
NPC npcs[MAX_NPCS];
int current_npc; // which npc running now, used inside npc_script

void npc_yield() {
    swapcontext(&npcs[current_npc].ctx, &ctx_main);
}

// each NPC's "brain" — fill in real actions
void npc_script() {
    NPC *self = &npcs[current_npc];

    printf("%s: waking up\n", self->name);
    npc_yield();

    printf("%s: moving to point A\n", self->name);
    npc_yield();

    printf("%s: attacking\n", self->name);
    npc_yield();

    printf("%s: done\n", self->name);
    self->done = 1;
    // falls off end here, uc_link sends control back to ctx_main automatically
}

void npc_init(int idx, const char *name) {
    NPC *n = &npcs[idx];
    strcpy(n->name, name);
    n->done = 0;

    getcontext(&n->ctx);
    n->ctx.uc_stack.ss_sp = n->stack;      // stack is a fixed array, use directly, no malloc
    n->ctx.uc_stack.ss_size = STACK_SIZE;
    n->ctx.uc_link = &ctx_main;
    makecontext(&n->ctx, npc_script, 0);   // no args passed, npc_script reads current_npc global
}

int all_done() {
    for (int i = 0; i < MAX_NPCS; i++)
        if (!npcs[i].done) return 0;
    return 1;
}

int main() {
    npc_init(0, "Goblin");
    npc_init(1, "Archer");
    npc_init(2, "Wizard");

    int tick = 0;
    while (!all_done()) {
        printf("--- tick %d ---\n", tick++);
        for (int i = 0; i < MAX_NPCS; i++) {
            if (!npcs[i].done) {
                current_npc = i;
                swapcontext(&ctx_main, &npcs[i].ctx);
            }
        }
    }

    printf("All NPCs finished.\n");
    return 0;
}