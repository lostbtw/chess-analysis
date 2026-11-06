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

static bool has_move(const std::string& fen, Move expected_move) {
    Board board;
    board.set_fen(fen);
    MoveList moves;
    generate_pseudo_legal_moves(board, moves);
    for (Move move : moves)
        if (move == expected_move) return true;
    return false;
}

int main() {
    check_move_counts(START_FEN, 20, 0);

    check_move_counts("k7/8/8/8/4N3/8/8/K7 w - - 0 1", 11, 0);
    check_move_counts("k7/8/3P4/8/4N3/8/8/K7 w - - 0 1", 11, 0);
    check_move_counts("k7/8/3p4/8/4N3/8/8/K7 w - - 0 1", 11, 1);
    // Rook a1: 7 up the file + b1, c1, d1; king e1: 5 moves.
    check_move_counts("7k/8/8/8/8/8/8/R3K2r w - - 0 1", 15, 0);
    // Rook a1 stops on the enemy pawn a5 and captures it: a2, a3, a4, a5 + b1, c1, d1; king e1: 5 moves.
    check_move_counts("7k/8/8/p7/8/8/8/R3K3 w - - 0 1", 12, 1);
    // Queen d4 has 27 moves on an empty board, minus a1 (own king stands there); king a1: 3 moves.
    check_move_counts("k7/8/8/8/3Q4/8/8/K7 w - - 0 1", 29, 0);

    // Pawn e2 blocked by a knight on e3 cannot move; king a1 has 3 moves.
    check_move_counts("k7/8/8/8/8/4n3/4P3/K7 w - - 0 1", 3, 0);
    // Only the second square is blocked: single step e3 is possible, double step is not.
    check_move_counts("k7/8/8/8/4n3/8/4P3/K7 w - - 0 1", 4, 0);
    // Pawn e2: e3, e4, captures d3 and f3; king a1: 3 moves.
    check_move_counts("k7/8/8/8/8/3p1p2/4P3/K7 w - - 0 1", 7, 2);
    // Pawn e7 promotes on e8 (4 pieces); king a1: 3 moves.
    check_move_counts("k7/4P3/8/8/8/8/8/K7 w - - 0 1", 7, 0);
    // Pawn e7: promotion on e8 (4) and capturing the rook d8 with promotion (4); king a1: 3 moves.
    check_move_counts("k2r4/4P3/8/8/8/8/8/K7 w - - 0 1", 11, 4);
    // Black pawn a7 moves down: a6, a5; black king a8: b7, b8.
    check_move_counts("k7/p7/8/8/8/8/8/7K b - - 0 1", 4, 0);
    // Black pawn e2 promotes on e1 (4); black king a8: a7, b7, b8.
    check_move_counts("k7/8/8/8/8/8/4p3/K7 b - - 0 1", 7, 0);

    Board en_passant_board;
    en_passant_board.set_fen("k7/8/8/3pP3/8/8/8/K7 w - d6 0 1");
    MoveList en_passant_moves;
    generate_pseudo_legal_moves(en_passant_board, en_passant_moves);
    bool found_en_passant = false;
    for (Move move : en_passant_moves) found_en_passant |= (move == Move(E5, D6, EN_PASSANT_CAPTURE));
    CHECK(found_en_passant);
    // e6 push, en passant capture; king a1: 3 moves.
    CHECK(en_passant_moves.size() == 5);

    const Move white_kingside(E1, G1, KINGSIDE_CASTLE);
    const Move white_queenside(E1, C1, QUEENSIDE_CASTLE);
    const Move black_kingside(E8, G8, KINGSIDE_CASTLE);
    const Move black_queenside(E8, C8, QUEENSIDE_CASTLE);

    const std::string both_sides = "r3k2r/8/8/8/8/8/8/R3K2R";
    CHECK(has_move(both_sides + " w KQkq - 0 1", white_kingside));
    CHECK(has_move(both_sides + " w KQkq - 0 1", white_queenside));
    CHECK(has_move(both_sides + " b KQkq - 0 1", black_kingside));
    CHECK(has_move(both_sides + " b KQkq - 0 1", black_queenside));
    CHECK(!has_move(both_sides + " w - - 0 1", white_kingside));
    CHECK(!has_move(both_sides + " w Kkq - 0 1", white_queenside));

    // Pieces between the king and the rooks: knight on g1, queen on d1.
    CHECK(!has_move("r3k2r/8/8/8/8/8/8/R2QK1NR w KQkq - 0 1", white_kingside));
    CHECK(!has_move("r3k2r/8/8/8/8/8/8/R2QK1NR w KQkq - 0 1", white_queenside));

    // Black rook on e4 gives check: no castling out of check.
    CHECK(!has_move("r3k2r/8/8/8/4r3/8/8/R3K2R w KQkq - 0 1", white_kingside));
    CHECK(!has_move("r3k2r/8/8/8/4r3/8/8/R3K2R w KQkq - 0 1", white_queenside));

    // Black rook on f3 attacks f1: kingside is forbidden, queenside is fine.
    CHECK(!has_move("r3k2r/8/8/8/8/5r2/8/R3K2R w KQkq - 0 1", white_kingside));
    CHECK(has_move("r3k2r/8/8/8/8/5r2/8/R3K2R w KQkq - 0 1", white_queenside));

    // Black rook on b3 attacks only b1, which the king does not cross.
    CHECK(has_move("r3k2r/8/8/8/8/1r6/8/R3K2R w KQkq - 0 1", white_queenside));

    // Right is listed, but there is no rook on a1.
    CHECK(has_move("4k3/8/8/8/8/8/8/4K2R w KQ - 0 1", white_kingside));
    CHECK(!has_move("4k3/8/8/8/8/8/8/4K2R w KQ - 0 1", white_queenside));

    if (failures == 0) std::cout << "All move generation tests passed.\n";
    return failures == 0 ? 0 : 1;
}
