#pragma once

#include <bit>
#include <string>

#include "types.h"

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard RANK_1 = 0x00000000000000FFULL;

constexpr Bitboard square_bitboard(Square square) { return 1ULL << square; }

constexpr Bitboard file_bitboard(int file) { return FILE_A << file; }
constexpr Bitboard rank_bitboard(int rank) { return RANK_1 << (8 * rank); }

constexpr bool test_bit(Bitboard bitboard, Square square) { return (bitboard >> square) & 1ULL; }

// Number of set bits (= number of pieces in the set).
inline int count_set_bits(Bitboard bitboard) { return std::popcount(bitboard); }

// Index of the least significant set bit. The bitboard must not be 0.
inline Square least_significant_bit(Bitboard bitboard) { return Square(std::countr_zero(bitboard)); }

// Return the least significant set bit and remove it from the bitboard.
// Typical use: while (bitboard) { Square square = pop_least_significant_bit(bitboard); ... }
inline Square pop_least_significant_bit(Bitboard& bitboard) {
    Square square = least_significant_bit(bitboard);
    bitboard &= bitboard - 1;  // clears the lowest set bit
    return square;
}

// Debug: draw a bitboard as an 8x8 grid (rank 8 on top).
std::string bitboard_to_string(Bitboard bitboard);
