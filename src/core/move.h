#pragma once

#include <cstdint>
#include <string>

#include "core/types.h"

// Bit 2 (value 4) is set for captures, bit 3 (value 8) for promotions.
// The two lowest bits of a promotion flag select the piece: knight, bishop, rook, queen.
enum MoveFlag : int {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    KINGSIDE_CASTLE = 2,
    QUEENSIDE_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT_CAPTURE = 5,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION = 9,
    ROOK_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    KNIGHT_PROMOTION_CAPTURE = 12,
    BISHOP_PROMOTION_CAPTURE = 13,
    ROOK_PROMOTION_CAPTURE = 14,
    QUEEN_PROMOTION_CAPTURE = 15
};

// Packed into 16 bits: from square (6 bits), to square (6 bits), flag (4 bits).
class Move {
public:
    constexpr Move() : data_(0) {}
    constexpr Move(Square from_square, Square to_square, MoveFlag flag)
        : data_(static_cast<std::uint16_t>(from_square | (to_square << 6) | (flag << 12))) {}

    constexpr Square from_square() const { return Square(data_ & 0x3F); }
    constexpr Square to_square() const { return Square((data_ >> 6) & 0x3F); }
    constexpr MoveFlag flag() const { return MoveFlag(data_ >> 12); }

    constexpr bool is_capture() const { return (flag() & 4) != 0; }
    constexpr bool is_promotion() const { return (flag() & 8) != 0; }
    constexpr bool is_en_passant() const { return flag() == EN_PASSANT_CAPTURE; }
    constexpr bool is_double_pawn_push() const { return flag() == DOUBLE_PAWN_PUSH; }
    constexpr bool is_castling() const { return flag() == KINGSIDE_CASTLE || flag() == QUEENSIDE_CASTLE; }

    constexpr PieceType promotion_piece_type() const { return PieceType(KNIGHT + (flag() & 3)); }

    constexpr bool operator==(const Move& other) const { return data_ == other.data_; }

    std::string to_uci_string() const;

private:
    std::uint16_t data_;
};
