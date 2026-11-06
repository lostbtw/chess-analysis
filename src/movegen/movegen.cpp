#include "movegen/movegen.h"

#include "movegen/attacks.h"

namespace {

void generate_piece_moves(const Board& board, PieceType piece_type, MoveList& moves) {
    const Color side = board.side_to_move();
    const Bitboard own_pieces = board.pieces(side);
    const Bitboard enemy_pieces = board.pieces(~side);
    const Bitboard occupied_squares = board.occupied();

    Bitboard movers = board.pieces(side, piece_type);
    while (movers) {
        const Square from_square = pop_least_significant_bit(movers);
        Bitboard targets = piece_attacks(piece_type, from_square, occupied_squares) & ~own_pieces;
        while (targets) {
            const Square to_square = pop_least_significant_bit(targets);
            moves.add(Move(from_square, to_square, test_bit(enemy_pieces, to_square) ? CAPTURE : QUIET_MOVE));
        }
    }
}

}

void generate_pseudo_legal_moves(const Board& board, MoveList& moves) {
    for (PieceType piece_type : {KNIGHT, BISHOP, ROOK, QUEEN, KING})
        generate_piece_moves(board, piece_type, moves);
}
