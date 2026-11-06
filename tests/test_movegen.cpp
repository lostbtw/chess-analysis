#include <iostream>
#include <string>

#include "movegen/movegen.h"

static int failures = 0;

#define CHECK(condition)                                                    \
    do {                                                                    \
        if (!(condition)) {                                                 \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #condition "\n"; \
            ++failures;                                                     \
        }                                                                   \
    } while (0)

static void check_move_counts(const std::string& fen, int expected_moves, int expected_captures) {
    Board board;
    board.set_fen(fen);
    MoveList moves;
    generate_pseudo_legal_moves(board, moves);

    int captures = 0;
    for (Move move : moves) captures += move.is_capture();

    if (static_cast<int>(moves.size()) != expected_moves || captures != expected_captures) {
        std::cerr << "FAIL " << fen << "\n  expected " << expected_moves << " moves, " << expected_captures
                  << " captures; got " << moves.size() << ", " << captures << '\n';
        ++failures;
    }
}

int main() {
    // Pawn moves are not generated yet: only the 4 knight moves exist in the start position.
    check_move_counts(START_FEN, 4, 0);

    check_move_counts("k7/8/8/8/4N3/8/8/K7 w - - 0 1", 11, 0);
    check_move_counts("k7/8/3P4/8/4N3/8/8/K7 w - - 0 1", 10, 0);
    check_move_counts("k7/8/3p4/8/4N3/8/8/K7 w - - 0 1", 11, 1);
    // Rook a1: 7 up the file + b1, c1, d1; king e1: 5 moves (castling is not generated yet).
    check_move_counts("7k/8/8/8/8/8/8/R3K2r w - - 0 1", 15, 0);
    // Rook a1 stops on the enemy pawn a5 and captures it: a2, a3, a4, a5 + b1, c1, d1; king e1: 5 moves.
    check_move_counts("7k/8/8/p7/8/8/8/R3K3 w - - 0 1", 12, 1);
    // Queen d4 has 27 moves on an empty board, minus a1 (own king stands there); king a1: 3 moves.
    check_move_counts("k7/8/8/8/3Q4/8/8/K7 w - - 0 1", 29, 0);

    if (failures == 0) std::cout << "All move generation tests passed.\n";
    return failures == 0 ? 0 : 1;
}
