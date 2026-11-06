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
    const std::vector<std::string> fens = {
        START_FEN,
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    };
    for (const auto& fen : fens) {
        Board board;
        CHECK(board.set_fen(fen));
        CHECK(board.fen() == fen);
    }

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

    board.remove_piece(E2);
    CHECK(board.piece_on(E2) == NO_PIECE);
    CHECK(count_set_bits(board.pieces(WHITE, PAWN)) == 7);
    board.put_piece(WHITE_PAWN, E4);
    CHECK(board.piece_on(E4) == WHITE_PAWN);
    CHECK(count_set_bits(board.pieces(WHITE)) == 16);

    const std::vector<std::string> invalid_fens = {
        "",
        "not a fen",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1",
        "rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1",
        "rnbqqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQQBNR w - - 0 1",
    };
    for (const auto& fen : invalid_fens) {
        Board invalid_board;
        CHECK(!invalid_board.set_fen(fen));
    }

    Board attack_board;
    attack_board.set_fen(START_FEN);
    CHECK(attack_board.is_square_attacked(E3, WHITE));
    CHECK(!attack_board.is_square_attacked(E4, WHITE));
    CHECK(attack_board.is_square_attacked(F6, BLACK));
    CHECK(!attack_board.is_square_attacked(E5, BLACK));

    attack_board.set_fen("4k3/8/8/8/4r3/8/4P3/4K3 w - - 0 1");
    CHECK(attack_board.is_square_attacked(E3, BLACK));
    CHECK(attack_board.is_square_attacked(E2, BLACK));
    CHECK(!attack_board.is_square_attacked(E1, BLACK));

    attack_board.set_fen("4k3/8/8/8/8/5n2/8/4K3 w - - 0 1");
    CHECK(attack_board.is_square_attacked(E1, BLACK));
    CHECK(attack_board.is_square_attacked(D2, BLACK));
    CHECK(!attack_board.is_square_attacked(E2, BLACK));

    attack_board.set_fen("4k3/8/8/8/8/8/3p4/4K3 w - - 0 1");
    CHECK(attack_board.is_square_attacked(E1, BLACK));
    CHECK(!attack_board.is_square_attacked(D1, BLACK));

    if (failures == 0) std::cout << "All board tests passed.\n";
    return failures == 0 ? 0 : 1;
}
