#pragma once

#include <bit>
#include <string>

#include "core/types.h"

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard RANK_1 = 0x00000000000000FFULL;

constexpr Bitboard square_bitboard(Square square) { return 1ULL << square; }

constexpr Bitboard file_bitboard(int file) { return FILE_A << file; }
constexpr Bitboard rank_bitboard(int rank) { return RANK_1 << (8 * rank); }

constexpr bool test_bit(Bitboard bitboard, Square square) { return (bitboard >> square) & 1ULL; }

inline int count_set_bits(Bitboard bitboard) { return std::popcount(bitboard); }

// The bitboard must not be empty.
inline Square least_significant_bit(Bitboard bitboard) { return Square(std::countr_zero(bitboard)); }

inline Square pop_least_significant_bit(Bitboard& bitboard) {
    Square square = least_significant_bit(bitboard);
    bitboard &= bitboard - 1;
    return square;
}

std::string bitboard_to_string(Bitboard bitboard);
