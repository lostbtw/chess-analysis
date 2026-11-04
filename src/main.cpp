#include <iostream>
#include <string>

#include "core/board.h"

// Temporary driver: `chess [FEN]` prints the position. UCI will replace this later.
int main(int argc, char** argv) {
    std::string fen = START_FEN;
    if (argc > 1) {
        fen.clear();
        for (int i = 1; i < argc; ++i) {
            if (i > 1) fen += ' ';
            fen += argv[i];
        }
    }

    Board board;
    if (!board.set_fen(fen)) {
        std::cerr << "Invalid FEN: " << fen << '\n';
        return 1;
    }
    std::cout << board.to_string();
    std::cout << "\nWhite pieces:\n" << bitboard_to_string(board.pieces(WHITE));
    return 0;
}
