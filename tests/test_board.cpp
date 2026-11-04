// Minimal self-contained tests (no framework): FEN round-trip and basic invariants.

#include <iostream>
#include <string>
#include <vector>

#include "core/board.h"

static int failures = 0;

#define CHECK(condition)                                                    \
    do {                                                                    \
        if (!(condition)) {                                                 \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #condition "\n"; \
            ++failures;                                                     \
        }                                                                   \
    } while (0)

int main() {
    // 1) FEN -> Board -> FEN must give the same string.
    const std::vector<std::string> fens = {
        START_FEN,
        // "Kiwipete", a standard perft test position
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        // position with en passant square
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",
        // endgame, no castling
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    };
    for (const auto& fen : fens) {
        Board board;
        CHECK(board.set_fen(fen));
        CHECK(board.fen() == fen);
    }

    // 2) Start position invariants.
    Board board;
    CHECK(board.set_fen(START_FEN));
    CHECK(count_set_bits(board.occupied()) == 32);
    CHECK(count_set_bits(board.pieces(WHITE, PAWN)) == 8);
    CHECK(board.king_square(WHITE) == E1);
    CHECK(board.king_square(BLACK) == E8);
    CHECK(board.piece_on(A1) == WHITE_ROOK);
    CHECK(board.piece_on(D8) == BLACK_QUEEN);
    CHECK(board.piece_on(E4) == NO_PIECE);
    CHECK(board.side_to_move() == WHITE);
    CHECK(board.castling_rights() == ALL_CASTLING);
    CHECK(board.en_passant_square() == NO_SQUARE);

    // 3) put/remove keep bitboards and mailbox in sync.
    board.remove_piece(E2);
    CHECK(board.piece_on(E2) == NO_PIECE);
    CHECK(count_set_bits(board.pieces(WHITE, PAWN)) == 7);
    board.put_piece(WHITE_PAWN, E4);
    CHECK(board.piece_on(E4) == WHITE_PAWN);
    CHECK(count_set_bits(board.pieces(WHITE)) == 16);

    // 4) Malformed FENs are rejected.
    const std::vector<std::string> invalid_fens = {
        "",
        "not a fen",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1",            // 7 ranks
        "rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",   // bad digit
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1",   // bad side
        "rnbqqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQQBNR w - - 0 1",      // no kings
    };
    for (const auto& fen : invalid_fens) {
        Board invalid_board;
        CHECK(!invalid_board.set_fen(fen));
    }

    if (failures == 0) std::cout << "All board tests passed.\n";
    return failures == 0 ? 0 : 1;
}
