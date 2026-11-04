#pragma once
// Board representation: bitboards + a mailbox array.

#include <string>

#include "core/bitboard.h"
#include "core/types.h"

// FEN (Forsyth-Edwards Notation) is a one-line text format describing a chess position.
constexpr const char* START_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class Board {
public:
    Board();
    bool set_fen(const std::string& fen);
    std::string fen() const;
    std::string to_string() const;

    Bitboard pieces(Color color, PieceType piece_type) const { return pieces_by_type_[color][piece_type]; }
    Bitboard pieces(Color color) const { return pieces_by_color_[color]; }
    Bitboard occupied() const { return pieces_by_color_[WHITE] | pieces_by_color_[BLACK]; }
    Piece piece_on(Square square) const { return mailbox_[square]; }
    Square king_square(Color color) const { return least_significant_bit(pieces_by_type_[color][KING]); }

    Color side_to_move() const { return side_to_move_; }
    int castling_rights() const { return castling_rights_; }
    Square en_passant_square() const { return en_passant_square_; }
    int halfmove_clock() const { return halfmove_clock_; }
    int fullmove_number() const { return fullmove_number_; }

    void put_piece(Piece piece, Square square);
    void remove_piece(Square square);

private:
    void clear();

    // pieces_by_type_[color][piece type]: where each kind of piece stands (12 bitboards).
    Bitboard pieces_by_type_[COLOR_COUNT][PIECE_TYPE_COUNT];
    // pieces_by_color_[color]: union of all pieces of that color (kept in sync with pieces_by_type_).
    Bitboard pieces_by_color_[COLOR_COUNT];
    // mailbox_[square]: which piece is on a square. Redundant with the bitboards,
    // but makes "what is on e4?" O(1) (needed for captures, printing, eval).
    Piece mailbox_[SQUARE_COUNT];

    Color side_to_move_;
    int castling_rights_;           // combination of CastlingRights flags
    Square en_passant_square_;      // en passant target square, or NO_SQUARE
    int halfmove_clock_;            // plies since last capture or pawn move (50-move rule)
    int fullmove_number_;           // starts at 1, incremented after Black moves
};
