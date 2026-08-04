#include <iostream>
#include <array>
#include <bit>
#include <cstdint>
#include <cassert>

enum Piece : uint8_t {
    P, N, B, R, Q, K,  // White
    p, n, b, r, q, k,  // Black
    NONE
};

class Board {
private:
    std::array<uint64_t, 12> pieces{};

    static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
    static constexpr uint64_t FILE_B = 0x0202020202020202ULL;
    static constexpr uint64_t FILE_G = 0x4040404040404040ULL;
    static constexpr uint64_t FILE_H = 0x8080808080808080ULL;

    static constexpr inline bool get_bit(uint64_t bb, int sq) {
        return (bb >> sq) & 1ULL;
    }

public:
    constexpr Board() { reset(); }

    constexpr void reset() {
        pieces.fill(0ULL);
        pieces[P] = 0x000000000000FF00ULL;
        pieces[p] = 0x00FF000000000000ULL;
        pieces[R] = (1ULL << 0) | (1ULL << 7);
        pieces[N] = (1ULL << 1) | (1ULL << 6);
        pieces[B] = (1ULL << 2) | (1ULL << 5);
        pieces[Q] = (1ULL << 3);
        pieces[K] = (1ULL << 4);
        pieces[r] = (1ULL << 56) | (1ULL << 63);
        pieces[n] = (1ULL << 57) | (1ULL << 62);
        pieces[b] = (1ULL << 58) | (1ULL << 61);
        pieces[q] = (1ULL << 59);
        pieces[k] = (1ULL << 60);
    }

    [[nodiscard]] constexpr uint64_t occupied() const {
        uint64_t occ = 0ULL;
        for (auto bb : pieces) occ |= bb;
        return occ;
    }

    [[nodiscard]] constexpr uint64_t empty() const {
        return ~occupied();
    }

    void push_white_pawns() {
        uint64_t pushed = (pieces[P] << 8) & empty();
        uint64_t movers = pushed >> 8; // Backtrack to source
        
        pieces[P] &= ~movers; // Clear sources
        pieces[P] |= pushed;  // Set targets
    }

    void push_black_pawns() {
        uint64_t pushed = (pieces[p] >> 8) & empty();
        uint64_t movers = pushed << 8; // Backtrack to source
        
        pieces[p] &= ~movers;
        pieces[p] |= pushed;
    }

    static constexpr uint64_t knight_attacks(uint64_t knights) {
        uint64_t l1 = (knights >> 1) & ~FILE_H;
        uint64_t l2 = (knights >> 2) & ~(FILE_H | FILE_G);
        uint64_t r1 = (knights << 1) & ~FILE_A;
        uint64_t r2 = (knights << 2) & ~(FILE_A | FILE_B);

        return (l1 << 16) | (l1 >> 16) |
               (l2 << 8)  | (l2 >> 8)  |
               (r1 << 16) | (r1 >> 16) |
               (r2 << 8)  | (r2 >> 8);
    }

    void print(uint64_t overlay = 0ULL) const {
        constexpr char glyphs[] = "PNBRQKpnbrqk";
        std::cout << "\n";
        for (int rank = 7; rank >= 0; --rank) {
            std::cout << rank + 1 << " ";
            for (int file = 0; file < 8; ++file) {
                int sq = rank * 8 + file;
                char c = '.';
                
                if (get_bit(overlay, sq)) {
                    c = '*'; // Highlight overlay square
                } else {
                    for (int i = 0; i < 12; ++i) {
                        if (get_bit(pieces[i], sq)) {
                            c = glyphs[i];
                            break;
                        }
                    }
                }
                std::cout << c << " ";
            }
            std::cout << "\n";
        }
        std::cout << "  a b c d e f g h\n";
    }

    [[nodiscard]] int total_pieces() const {
        return std::popcount(occupied()); // C++20 instruction intrinsics
    }

    // Place obstacle manually to test push bug fix
    void set_blocker(int sq) {
        pieces[r] |= (1ULL << sq);
    }
};

int main() {
    Board board;

    std::cout << "Total pieces start: " << board.total_pieces();
    board.print();

    // Test Knight Attacks with Overlay
    uint64_t w_knights = 0x0000000000000042ULL; // Initial N at b1, g1
    uint64_t n_attacks = Board::knight_attacks(w_knights);
    std::cout << "\n--- WHITE KNIGHT ATTACKS (*) ---";
    board.print(n_attacks);

    // Test Blocker (place black rook at e3 -> sq 20)
    board.set_blocker(20);
    std::cout << "\n--- ADDED BLOCKER AT e3 ---";
    board.print();

    // Push White Pawns (e2 pawn blocked, others push)
    board.push_white_pawns();
    std::cout << "\n--- WHITE PAWNS PUSHED (e2 stayed put) ---";
    board.print();

    // Push Black Pawns down
    board.push_black_pawns();
    std::cout << "\n--- BLACK PAWNS PUSHED ---";
    board.print();

    std::cout << "Total pieces end: " << board.total_pieces() << "\n";

    constexpr Board compile_time_board;
    static_assert(compile_time_board.occupied() == 0xFFFF00000000FFFFULL, 
              "Compile-time bitboard initialization failed!");
    return 0;
}