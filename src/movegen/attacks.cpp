#include "movegen/attacks.h"

namespace {

using attack_tables_detail::Offset;

constexpr std::array<Offset, 4> BISHOP_DIRECTIONS = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
constexpr std::array<Offset, 4> ROOK_DIRECTIONS = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};

Bitboard sliding_attacks(Square square, Bitboard occupied, const std::array<Offset, 4>& directions) {
    Bitboard attacks = 0;
    for (const Offset& direction : directions) {
        int file = file_of(square) + direction.file;
        int rank = rank_of(square) + direction.rank;
        while (file >= 0 && file <= 7 && rank >= 0 && rank <= 7) {
            Bitboard target = square_bitboard(make_square(file, rank));
            attacks |= target;
            if (occupied & target) break;
            file += direction.file;
            rank += direction.rank;
        }
    }
    return attacks;
}

}  // namespace

Bitboard bishop_attacks(Square square, Bitboard occupied) {
    return sliding_attacks(square, occupied, BISHOP_DIRECTIONS);
}

Bitboard rook_attacks(Square square, Bitboard occupied) {
    return sliding_attacks(square, occupied, ROOK_DIRECTIONS);
}

Bitboard queen_attacks(Square square, Bitboard occupied) {
    return bishop_attacks(square, occupied) | rook_attacks(square, occupied);
}
