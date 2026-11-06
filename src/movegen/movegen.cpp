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

struct CastlingRule {
    Color side;
    int required_right;
    Square king_from_square;
    Square king_to_square;
    Square king_transit_square;
    Square rook_from_square;
    Bitboard squares_that_must_be_empty;
    MoveFlag flag;
};

constexpr CastlingRule CASTLING_RULES[] = {
    {WHITE, WHITE_KINGSIDE,  E1, G1, F1, H1, square_bitboard(F1) | square_bitboard(G1), KINGSIDE_CASTLE},
    {WHITE, WHITE_QUEENSIDE, E1, C1, D1, A1, square_bitboard(B1) | square_bitboard(C1) | square_bitboard(D1), QUEENSIDE_CASTLE},
    {BLACK, BLACK_KINGSIDE,  E8, G8, F8, H8, square_bitboard(F8) | square_bitboard(G8), KINGSIDE_CASTLE},
    {BLACK, BLACK_QUEENSIDE, E8, C8, D8, A8, square_bitboard(B8) | square_bitboard(C8) | square_bitboard(D8), QUEENSIDE_CASTLE},
};

// The king may not castle out of, through or into check. The rook may pass an attacked square.
void generate_castling_moves(const Board& board, MoveList& moves) {
    const Color side = board.side_to_move();
    const Color enemy = ~side;

    for (const CastlingRule& rule : CASTLING_RULES) {
        if (rule.side != side || !(board.castling_rights() & rule.required_right)) continue;
        if (board.occupied() & rule.squares_that_must_be_empty) continue;
        if (board.piece_on(rule.king_from_square) != make_piece(side, KING)) continue;
        if (board.piece_on(rule.rook_from_square) != make_piece(side, ROOK)) continue;
        if (board.is_square_attacked(rule.king_from_square, enemy)) continue;
        if (board.is_square_attacked(rule.king_transit_square, enemy)) continue;
        if (board.is_square_attacked(rule.king_to_square, enemy)) continue;

        moves.add(Move(rule.king_from_square, rule.king_to_square, rule.flag));
    }
}

}

void generate_pseudo_legal_moves(const Board& board, MoveList& moves) {
    generate_castling_moves(board, moves);
    generate_pawn_moves(board, moves);
    for (PieceType piece_type : {KNIGHT, BISHOP, ROOK, QUEEN, KING})
        generate_piece_moves(board, piece_type, moves);
}

void generate_legal_moves(Board& board, MoveList& moves) {
    MoveList pseudo_legal_moves;
    generate_pseudo_legal_moves(board, pseudo_legal_moves);

    const Color mover = board.side_to_move();
    for (Move move : pseudo_legal_moves) {
        board.make_move(move);
        if (!board.is_square_attacked(board.king_square(mover), ~mover)) moves.add(move);
        board.unmake_move();
    }
}
