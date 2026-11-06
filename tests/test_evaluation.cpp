#include <iostream>
#include <string>

#include "eval/evaluation.h"

static int failures = 0;

#define CHECK(condition)                                                    \
    do {                                                                    \
        if (!(condition)) {                                                 \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #condition "\n"; \
            ++failures;                                                     \
        }                                                                   \
    } while (0)

static EvaluationBreakdown breakdown_of(const std::string& fen) {
    Board board;
    board.set_fen(fen);
    return evaluate_breakdown(board);
}

int main() {
    // Symmetric position: nothing separates the sides.
    EvaluationBreakdown start = breakdown_of(START_FEN);
    CHECK(start.material == 0);
    CHECK(start.piece_square == 0);

    // White has no queen: -900 material; the queen on d1 was worth -5 in the table, so +5 positional.
    EvaluationBreakdown without_queen = breakdown_of("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
    CHECK(without_queen.material == -900);
    CHECK(without_queen.piece_square == 5);
    CHECK(without_queen.total() == -895);

    // A white knight on e4 is worth 320 plus 20 for the square; the kings stand on neutral squares.
    EvaluationBreakdown white_knight = breakdown_of("4k3/8/8/8/4N3/8/8/4K3 w - - 0 1");
    CHECK(white_knight.material == 320);
    CHECK(white_knight.piece_square == 20);

    // The same knight for Black on e5 gives exactly the opposite score.
    EvaluationBreakdown black_knight = breakdown_of("4k3/8/8/4n3/8/8/8/4K3 w - - 0 1");
    CHECK(black_knight.material == -320);
    CHECK(black_knight.piece_square == -20);

    // Pawns: a white pawn on e2 is worth -20 on its square, a white pawn on e4 is worth +20.
    CHECK(breakdown_of("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1").piece_square == -20);
    CHECK(breakdown_of("4k3/8/8/8/4P3/8/8/4K3 w - - 0 1").piece_square == 20);
    CHECK(breakdown_of("4k3/4p3/8/8/8/8/8/4K3 w - - 0 1").piece_square == 20);

    // The score is given from the side to move.
    Board board;
    board.set_fen("4k3/8/8/8/4N3/8/8/4K3 w - - 0 1");
    CHECK(evaluate(board) == 340);
    board.set_fen("4k3/8/8/8/4N3/8/8/4K3 b - - 0 1");
    CHECK(evaluate(board) == -340);

    if (failures == 0) std::cout << "All evaluation tests passed.\n";
    return failures == 0 ? 0 : 1;
}
