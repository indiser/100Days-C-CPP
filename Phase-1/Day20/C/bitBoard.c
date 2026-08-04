#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t Bitboard;

enum {
    P = 0, N = 1, B = 2, R = 3, Q = 4, K = 5,   /* White */
    p = 6, n = 7, b = 8, r = 9, q = 10, k = 11  /* Black */
};

static const char piece_chars[] = "PNBRQKpnbrqk";

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

static inline Bitboard set_bit(Bitboard bb, int square) {
    return bb | (1ULL << square);
}

static inline bool get_bit(Bitboard bb, int square) {
    return (bb >> square) & 1ULL;
}

void init_board(Bitboard boards[12]) {
    for (int i = 0; i < 12; i++) boards[i] = 0ULL;

    boards[P] = 0x000000000000FF00ULL;
    boards[p] = 0x00FF000000000000ULL;

    boards[R] = set_bit(set_bit(0ULL, 0), 7);
    boards[N] = set_bit(set_bit(0ULL, 1), 6);
    boards[B] = set_bit(set_bit(0ULL, 2), 5);
    boards[Q] = set_bit(0ULL, 3);
    boards[K] = set_bit(0ULL, 4);

    boards[r] = set_bit(set_bit(0ULL, 56), 63);
    boards[n] = set_bit(set_bit(0ULL, 57), 62);
    boards[b] = set_bit(set_bit(0ULL, 58), 61);
    boards[q] = set_bit(0ULL, 59);
    boards[k] = set_bit(0ULL, 60);
}

Bitboard get_occupied(const Bitboard boards[12]) {
    Bitboard occ = 0ULL;
    for (int i = 0; i < 12; i++) occ |= boards[i];
    return occ;
}

Bitboard knight_attacks(Bitboard knights) {
    Bitboard l1 = (knights >> 1) & ~FILE_H;
    Bitboard l2 = (knights >> 2) & ~(FILE_H | FILE_G);
    Bitboard r1 = (knights << 1) & ~FILE_A;
    Bitboard r2 = (knights << 2) & ~(FILE_A | FILE_B);

    return (l1 << 16) | (l1 >> 16) |
           (l2 << 8)  | (l2 >> 8)  |
           (r1 << 16) | (r1 >> 16) |
           (r2 << 8)  | (r2 >> 8);
}

void print_board(const Bitboard boards[12], Bitboard overlay) {
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d ", rank + 1);
        for (int file = 0; file < 8; file++) {
            int sq = rank * 8 + file;
            char glyph = '.';
            
            /* Print piece first */
            for (int p_type = 0; p_type < 12; p_type++) {
                if (get_bit(boards[p_type], sq)) {
                    glyph = piece_chars[p_type];
                    break;
                }
            }
            /* Overlay on top if empty, or mark hit */
            if (get_bit(overlay, sq)) {
                glyph = (glyph == '.') ? '*' : glyph; 
            }
            printf("%c ", glyph);
        }
        printf("\n");
    }
    printf("  a b c d e f g h\n");
}

void push_white_pawns(Bitboard boards[12]) {
    Bitboard empty = ~get_occupied(boards);
    Bitboard pushed = (boards[P] << 8) & empty;
    Bitboard movers = pushed >> 8; /* Backtrack source */

    boards[P] &= ~movers; /* Clear sources */
    boards[P] |= pushed;  /* Set targets */
}

void push_black_pawns(Bitboard boards[12]) {
    Bitboard empty = ~get_occupied(boards);
    Bitboard pushed = (boards[p] >> 8) & empty;
    Bitboard movers = pushed << 8; /* Backtrack source */

    boards[p] &= ~movers;
    boards[p] |= pushed;
}

int main(void) {
    Bitboard boards[12];
    init_board(boards);

    printf("--- INITIAL BOARD ---");
    print_board(boards, 0ULL);

    /* Knight Attacks Test */
    Bitboard attacks = knight_attacks(boards[N]);
    printf("\n--- WHITE KNIGHT ATTACKS (*) ---");
    print_board(boards, attacks);

    /* Set Blocker at e3 (sq 20) using black rook */
    boards[r] |= (1ULL << 20);
    printf("\n--- BLOCKER ADDED AT e3 ---");
    print_board(boards, 0ULL);

    /* Push White Pawns */
    push_white_pawns(boards);
    printf("\n--- WHITE PAWNS PUSHED (e2 blocked) ---");
    print_board(boards, 0ULL);

    /* Push Black Pawns */
    push_black_pawns(boards);
    printf("\n--- BLACK PAWNS PUSHED ---");
    print_board(boards, 0ULL);

    return 0;
}