#include "core/bitboard.h"

std::string bitboard_to_string(Bitboard bitboard) {
    std::string output;
    for (int rank = 7; rank >= 0; --rank) {
        output += char('1' + rank);
        output += "  ";
        for (int file = 0; file < 8; ++file) {
            output += test_bit(bitboard, make_square(file, rank)) ? "1 " : ". ";
        }
        output += '\n';
    }
    output += "\n   a b c d e f g h\n";
    return output;
}
