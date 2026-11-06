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

void add_promotions(MoveList& moves, Square from_square, Square to_square, bool is_capture) {
    const MoveFlag first_flag = is_capture ? KNIGHT_PROMOTION_CAPTURE : KNIGHT_PROMOTION;
    for (int piece_index = 0; piece_index < 4; ++piece_index)
        moves.add(Move(from_square, to_square, MoveFlag(first_flag + piece_index)));
}

void generate_pawn_moves(const Board& board, MoveList& moves) {
    const Color side = board.side_to_move();
    const Bitboard enemy_pieces = board.pieces(~side);
    const Bitboard occupied_squares = board.occupied();

    const int forward_step = (side == WHITE) ? 8 : -8;
    const int start_rank = (side == WHITE) ? 1 : 6;
    const int rank_before_promotion = (side == WHITE) ? 6 : 1;

    Bitboard pawns = board.pieces(side, PAWN);
    while (pawns) {
        const Square from_square = pop_least_significant_bit(pawns);
        const bool promotes = rank_of(from_square) == rank_before_promotion;

        const Square one_step_square = Square(from_square + forward_step);
        if (!test_bit(occupied_squares, one_step_square)) {
            if (promotes) {
                add_promotions(moves, from_square, one_step_square, false);
            } else {
                moves.add(Move(from_square, one_step_square, QUIET_MOVE));
                const Square two_steps_square = Square(one_step_square + forward_step);
                if (rank_of(from_square) == start_rank && !test_bit(occupied_squares, two_steps_square))
                    moves.add(Move(from_square, two_steps_square, DOUBLE_PAWN_PUSH));
            }
        }

        const Bitboard attacked_squares = pawn_attacks(side, from_square);
        Bitboard captures = attacked_squares & enemy_pieces;
        while (captures) {
            const Square capture_square = pop_least_significant_bit(captures);
            if (promotes) add_promotions(moves, from_square, capture_square, true);
            else moves.add(Move(from_square, capture_square, CAPTURE));
        }

        if (board.en_passant_square() != NO_SQUARE && test_bit(attacked_squares, board.en_passant_square()))
            moves.add(Move(from_square, board.en_passant_square(), EN_PASSANT_CAPTURE));
    }
}

}

void generate_pseudo_legal_moves(const Board& board, MoveList& moves) {
    generate_pawn_moves(board, moves);
    for (PieceType piece_type : {KNIGHT, BISHOP, ROOK, QUEEN, KING})
        generate_piece_moves(board, piece_type, moves);
}
