#pragma once

#include <cstdint>

using Bitboard = std::uint64_t;

enum Color : int { WHITE, BLACK, COLOR_COUNT };

enum PieceType : int { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_TYPE_COUNT };

// White pieces are 0..5, black pieces 6..11.
enum Piece : int {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    NO_PIECE,
    PIECE_COUNT = 12
};

// square = rank * 8 + file, so A1 = 0, H1 = 7, A2 = 8, ..., H8 = 63.
enum Square : int {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE,
    SQUARE_COUNT = 64
};

enum CastlingRights : int {
    NO_CASTLING = 0,
    WHITE_KINGSIDE  = 1,
    WHITE_QUEENSIDE = 2,
    BLACK_KINGSIDE  = 4,
    BLACK_QUEENSIDE = 8,
    ALL_CASTLING = 15
};

constexpr int file_of(Square square) { return square & 7; }
constexpr int rank_of(Square square) { return square >> 3; }
constexpr Square make_square(int file, int rank) { return Square(rank * 8 + file); }

constexpr Piece make_piece(Color color, PieceType piece_type) { return Piece(color * 6 + piece_type); }
constexpr Color color_of(Piece piece) { return Color(piece / 6); }
constexpr PieceType type_of(Piece piece) { return PieceType(piece % 6); }

constexpr Color operator~(Color color) { return Color(color ^ 1); }
