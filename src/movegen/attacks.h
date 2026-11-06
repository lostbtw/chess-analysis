#pragma once

#include <array>
#include <cstddef>

#include "core/bitboard.h"
#include "core/types.h"

namespace attack_tables_detail {

struct Offset {
    int file;
    int rank;
};

template <std::size_t OFFSET_COUNT>
constexpr Bitboard attacks_from_square(Square square, const std::array<Offset, OFFSET_COUNT>& offsets) {
    Bitboard attacks = 0;
    for (const Offset& offset : offsets) {
        int target_file = file_of(square) + offset.file;
        int target_rank = rank_of(square) + offset.rank;
        if (target_file < 0 || target_file > 7 || target_rank < 0 || target_rank > 7) continue;
        attacks |= square_bitboard(make_square(target_file, target_rank));
    }
    return attacks;
}

// Evaluated at compile time.
template <std::size_t OFFSET_COUNT>
constexpr std::array<Bitboard, SQUARE_COUNT> make_attack_table(const std::array<Offset, OFFSET_COUNT>& offsets) {
    std::array<Bitboard, SQUARE_COUNT> table{};
    for (int square = 0; square < SQUARE_COUNT; ++square)
        table[square] = attacks_from_square(Square(square), offsets);
    return table;
}

constexpr std::array<Offset, 8> KNIGHT_OFFSETS = {{
    {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
}};

constexpr std::array<Offset, 8> KING_OFFSETS = {{
    {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
}};

constexpr std::array<Offset, 2> WHITE_PAWN_OFFSETS = {{{-1, 1}, {1, 1}}};
constexpr std::array<Offset, 2> BLACK_PAWN_OFFSETS = {{{-1, -1}, {1, -1}}};

constexpr auto KNIGHT_ATTACKS = make_attack_table(KNIGHT_OFFSETS);
constexpr auto KING_ATTACKS = make_attack_table(KING_OFFSETS);
constexpr std::array<std::array<Bitboard, SQUARE_COUNT>, COLOR_COUNT> PAWN_ATTACKS = {{
    make_attack_table(WHITE_PAWN_OFFSETS),
    make_attack_table(BLACK_PAWN_OFFSETS)
}};

}  // namespace attack_tables_detail

inline Bitboard knight_attacks(Square square) { return attack_tables_detail::KNIGHT_ATTACKS[square]; }
inline Bitboard king_attacks(Square square) { return attack_tables_detail::KING_ATTACKS[square]; }
inline Bitboard pawn_attacks(Color color, Square square) { return attack_tables_detail::PAWN_ATTACKS[color][square]; }

// A ray stops at the first occupied square and includes it, because an enemy piece there
// can be captured. The caller removes own pieces from the result.
Bitboard bishop_attacks(Square square, Bitboard occupied);
Bitboard rook_attacks(Square square, Bitboard occupied);
Bitboard queen_attacks(Square square, Bitboard occupied);

// Any piece except a pawn, whose attacks depend on its color.
Bitboard piece_attacks(PieceType piece_type, Square square, Bitboard occupied);
