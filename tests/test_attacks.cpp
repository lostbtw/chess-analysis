#include <initializer_list>
#include <iostream>

#include "movegen/attacks.h"

static int failures = 0;

#define CHECK(condition)                                                    \
    do {                                                                    \
        if (!(condition)) {                                                 \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #condition "\n"; \
            ++failures;                                                     \
        }                                                                   \
    } while (0)

static Bitboard bitboard_of(std::initializer_list<Square> squares) {
    Bitboard bitboard = 0;
    for (Square square : squares) bitboard |= square_bitboard(square);
    return bitboard;
}

int main() {
    CHECK(knight_attacks(B1) == bitboard_of({A3, C3, D2}));
    CHECK(knight_attacks(A1) == bitboard_of({B3, C2}));
    CHECK(knight_attacks(H8) == bitboard_of({G6, F7}));
    CHECK(knight_attacks(E4) == bitboard_of({D6, F6, C5, G5, C3, G3, D2, F2}));

    CHECK(king_attacks(A1) == bitboard_of({A2, B1, B2}));
    CHECK(king_attacks(E1) == bitboard_of({D1, F1, D2, E2, F2}));
    CHECK(king_attacks(E4) == bitboard_of({D3, E3, F3, D4, F4, D5, E5, F5}));

    CHECK(pawn_attacks(WHITE, E2) == bitboard_of({D3, F3}));
    CHECK(pawn_attacks(WHITE, A2) == bitboard_of({B3}));
    CHECK(pawn_attacks(WHITE, H7) == bitboard_of({G8}));
    CHECK(pawn_attacks(BLACK, E7) == bitboard_of({D6, F6}));
    CHECK(pawn_attacks(BLACK, H7) == bitboard_of({G6}));
    CHECK(pawn_attacks(WHITE, E8) == 0);
    CHECK(pawn_attacks(BLACK, E1) == 0);

    int knight_total = 0, king_total = 0;
    for (int square = 0; square < SQUARE_COUNT; ++square) {
        knight_total += count_set_bits(knight_attacks(Square(square)));
        king_total += count_set_bits(king_attacks(Square(square)));
    }
    CHECK(knight_total == 336);
    CHECK(king_total == 420);

    CHECK(rook_attacks(A1, 0) == ((file_bitboard(0) | rank_bitboard(0)) & ~square_bitboard(A1)));
    CHECK(count_set_bits(rook_attacks(E4, 0)) == 14);

    // Rook with blockers: the ray includes the blocking square and stops there.
    CHECK(rook_attacks(A1, bitboard_of({A4, D1})) == bitboard_of({A2, A3, A4, B1, C1, D1}));
    CHECK(rook_attacks(D4, bitboard_of({D6, D2, B4, G4})) ==
          bitboard_of({D5, D6, D3, D2, C4, B4, E4, F4, G4}));

    CHECK(bishop_attacks(C1, 0) == bitboard_of({B2, A3, D2, E3, F4, G5, H6}));
    CHECK(bishop_attacks(C1, bitboard_of({E3})) == bitboard_of({B2, A3, D2, E3}));
    CHECK(count_set_bits(bishop_attacks(E4, 0)) == 13);

    CHECK(count_set_bits(queen_attacks(D4, 0)) == 27);
    CHECK(queen_attacks(D4, bitboard_of({D5, F6})) ==
          (rook_attacks(D4, bitboard_of({D5, F6})) | bishop_attacks(D4, bitboard_of({D5, F6}))));

    int rook_total = 0, bishop_total = 0;
    for (int square = 0; square < SQUARE_COUNT; ++square) {
        rook_total += count_set_bits(rook_attacks(Square(square), 0));
        bishop_total += count_set_bits(bishop_attacks(Square(square), 0));
    }
    CHECK(rook_total == 896);
    CHECK(bishop_total == 560);

    if (failures == 0) std::cout << "All attack tests passed.\n";
    return failures == 0 ? 0 : 1;
}
