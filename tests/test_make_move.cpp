#include <iostream>
#include <string>

#include "core/board.h"

static int failures = 0;

#define CHECK(condition)                                                    \
    do {                                                                    \
        if (!(condition)) {                                                 \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #condition "\n"; \
            ++failures;                                                     \
        }                                                                   \
    } while (0)

// Plays the move, checks the resulting FEN, then undoes it and checks the original FEN is back.
static void check_move(const std::string& fen_before, Move move, const std::string& fen_after) {
    Board board;
    if (!board.set_fen(fen_before)) {
        std::cerr << "FAIL bad test FEN: " << fen_before << '\n';
        ++failures;
        return;
    }
    board.make_move(move);
    if (board.fen() != fen_after) {
        std::cerr << "FAIL " << move.to_uci_string() << " from " << fen_before << "\n  expected " << fen_after
                  << "\n  got      " << board.fen() << '\n';
        ++failures;
    }
    board.unmake_move();
    if (board.fen() != fen_before) {
        std::cerr << "FAIL unmake " << move.to_uci_string() << "\n  expected " << fen_before
                  << "\n  got      " << board.fen() << '\n';
        ++failures;
    }
}

int main() {
    const Move promotion_capture(A7, B8, KNIGHT_PROMOTION_CAPTURE);
    CHECK(promotion_capture.from_square() == A7);
    CHECK(promotion_capture.to_square() == B8);
    CHECK(promotion_capture.is_capture());
    CHECK(promotion_capture.is_promotion());
    CHECK(promotion_capture.promotion_piece_type() == KNIGHT);
    CHECK(promotion_capture.to_uci_string() == "a7b8n");
    CHECK(Move(E2, E4, DOUBLE_PAWN_PUSH).to_uci_string() == "e2e4");
    CHECK(!Move(E2, E4, DOUBLE_PAWN_PUSH).is_capture());
    CHECK(Move(E1, G1, KINGSIDE_CASTLE).is_castling());

    check_move(START_FEN, Move(E2, E4, DOUBLE_PAWN_PUSH),
               "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    check_move("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2", Move(E4, D5, CAPTURE),
               "rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");

    check_move("rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3", Move(E5, D6, EN_PASSANT_CAPTURE),
               "rnbqkbnr/ppp1pppp/3P4/8/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 3");

    check_move("rnbqkbnr/pppp1ppp/8/8/3Pp3/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 3", Move(E4, D3, EN_PASSANT_CAPTURE),
               "rnbqkbnr/pppp1ppp/8/8/8/3p4/PPP1PPPP/RNBQKBNR w KQkq - 0 4");

    const std::string both_castling = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    check_move(both_castling, Move(E1, G1, KINGSIDE_CASTLE), "r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 1");
    check_move(both_castling, Move(E1, C1, QUEENSIDE_CASTLE), "r3k2r/8/8/8/8/8/8/2KR3R b kq - 1 1");
    check_move("r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 1", Move(E8, C8, QUEENSIDE_CASTLE),
               "2kr3r/8/8/8/8/8/8/R4RK1 w - - 2 2");
    check_move("r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 1", Move(E8, G8, KINGSIDE_CASTLE),
               "r4rk1/8/8/8/8/8/8/R4RK1 w - - 2 2");

    check_move(both_castling, Move(H1, H2, QUIET_MOVE), "r3k2r/8/8/8/8/8/7R/R3K3 b Qkq - 1 1");
    check_move(both_castling, Move(H1, H8, CAPTURE), "r3k2R/8/8/8/8/8/8/R3K3 b Qq - 0 1");

    check_move("8/P6k/8/8/8/8/8/K7 w - - 0 1", Move(A7, A8, QUEEN_PROMOTION), "Q7/7k/8/8/8/8/8/K7 b - - 0 1");
    check_move("1r5k/P7/8/8/8/8/8/K7 w - - 0 1", Move(A7, B8, KNIGHT_PROMOTION_CAPTURE),
               "1N5k/8/8/8/8/8/8/K7 b - - 0 1");
    check_move("K7/8/8/8/8/8/p6k/8 b - - 0 1", Move(A2, A1, ROOK_PROMOTION), "K7/8/8/8/8/8/7k/r7 w - - 0 2");

    // Several moves in a row, then everything undone.
    Board board;
    board.set_fen(START_FEN);
    board.make_move(Move(E2, E4, DOUBLE_PAWN_PUSH));
    board.make_move(Move(E7, E5, DOUBLE_PAWN_PUSH));
    board.make_move(Move(G1, F3, QUIET_MOVE));
    board.make_move(Move(B8, C6, QUIET_MOVE));
    CHECK(board.fen() == "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3");
    for (int i = 0; i < 4; ++i) board.unmake_move();
    CHECK(board.fen() == START_FEN);

    if (failures == 0) std::cout << "All make/unmake tests passed.\n";
    return failures == 0 ? 0 : 1;
}
