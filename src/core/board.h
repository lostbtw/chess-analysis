#pragma once

#include <string>
#include <vector>

#include "core/bitboard.h"
#include "core/move.h"
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

    // The move must be pseudo-legal in the current position. Legality is not checked here.
    void make_move(Move move);
    void unmake_move();

private:
    // Everything unmake_move() cannot recompute from the move itself.
    struct UndoRecord {
        Move move;
        Piece captured_piece;
        int castling_rights;
        Square en_passant_square;
        int halfmove_clock;
    };

    void clear();

    Bitboard pieces_by_type_[COLOR_COUNT][PIECE_TYPE_COUNT];
    Bitboard pieces_by_color_[COLOR_COUNT];
    // Same information as the bitboards, but answers "what stands on e4?" in O(1).
    Piece mailbox_[SQUARE_COUNT];

    Color side_to_move_;
    int castling_rights_;
    Square en_passant_square_;
    int halfmove_clock_;
    int fullmove_number_;

    std::vector<UndoRecord> history_;
};
