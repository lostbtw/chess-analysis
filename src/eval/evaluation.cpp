#include "eval/evaluation.h"

#include <array>

#include "eval/piece_square_tables.h"

namespace {

constexpr std::array<int, PIECE_TYPE_COUNT> MATERIAL_VALUES = {100, 320, 330, 500, 900, 0};

// The tables are written from White's side with rank 8 first, so White reads them upside down.
// Flipping the rank of a square is XOR with 56. Black uses the square number directly,
// because the table seen from Black's side is the same table mirrored vertically.
int table_index(Color color, Square square) { return color == WHITE ? (square ^ 56) : square; }

}

EvaluationBreakdown evaluate_breakdown(const Board& board) {
    EvaluationBreakdown breakdown = {0, 0};

    for (Color color : {WHITE, BLACK}) {
        const int sign = (color == WHITE) ? 1 : -1;
        for (int piece_type = PAWN; piece_type < PIECE_TYPE_COUNT; ++piece_type) {
            Bitboard pieces = board.pieces(color, PieceType(piece_type));
            while (pieces) {
                const Square square = pop_least_significant_bit(pieces);
                breakdown.material += sign * MATERIAL_VALUES[piece_type];
                breakdown.piece_square += sign * PIECE_SQUARE_TABLES[piece_type][table_index(color, square)];
            }
        }
    }
    return breakdown;
}

int evaluate(const Board& board) {
    const int score_for_white = evaluate_breakdown(board).total();
    return board.side_to_move() == WHITE ? score_for_white : -score_for_white;
}
