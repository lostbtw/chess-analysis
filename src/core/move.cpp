#include "core/move.h"

namespace {

void append_square(std::string& text, Square square) {
    text += char('a' + file_of(square));
    text += char('1' + rank_of(square));
}

}

std::string Move::to_uci_string() const {
    std::string text;
    append_square(text, from_square());
    append_square(text, to_square());
    if (is_promotion()) text += "nbrq"[promotion_piece_type() - KNIGHT];
    return text;
}
