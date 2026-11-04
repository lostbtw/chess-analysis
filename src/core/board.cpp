#include "core/board.h"

#include <cctype>
#include <sstream>

namespace {
const char PIECE_CHARACTERS[] = "PNBRQKpnbrqk";  // index = Piece enum value

int piece_from_character(char character) {
    for (int piece = 0; piece < PIECE_COUNT; ++piece)
        if (PIECE_CHARACTERS[piece] == character) return piece;
    return -1;
}
}

Board::Board() { clear(); }

void Board::clear() {
    for (int color = 0; color < COLOR_COUNT; ++color) {
        pieces_by_color_[color] = 0;
        for (int piece_type = 0; piece_type < PIECE_TYPE_COUNT; ++piece_type)
            pieces_by_type_[color][piece_type] = 0;
    }
    for (int square = 0; square < SQUARE_COUNT; ++square) mailbox_[square] = NO_PIECE;
    side_to_move_ = WHITE;
    castling_rights_ = NO_CASTLING;
    en_passant_square_ = NO_SQUARE;
    halfmove_clock_ = 0;
    fullmove_number_ = 1;
}

void Board::put_piece(Piece piece, Square square) {
    Bitboard square_mask = square_bitboard(square);
    pieces_by_type_[color_of(piece)][type_of(piece)] |= square_mask;
    pieces_by_color_[color_of(piece)] |= square_mask;
    mailbox_[square] = piece;
}

void Board::remove_piece(Square square) {
    Piece piece = mailbox_[square];
    if (piece == NO_PIECE) return;
    Bitboard square_mask = square_bitboard(square);
    pieces_by_type_[color_of(piece)][type_of(piece)] &= ~square_mask;
    pieces_by_color_[color_of(piece)] &= ~square_mask;
    mailbox_[square] = NO_PIECE;
}

bool Board::set_fen(const std::string& fen) {
    clear();
    std::istringstream stream(fen);
    std::string placement_text, side_text, castling_text, en_passant_text;
    int halfmove_clock = 0, fullmove_number = 1;

    if (!(stream >> placement_text >> side_text >> castling_text >> en_passant_text)) { clear(); return false; }
    // The two counters are optional in many FEN strings found in the wild.
    if (stream >> halfmove_clock) {
        if (!(stream >> fullmove_number)) fullmove_number = 1;
    }

    // 1) Piece placement: ranks from 8 down to 1, files a..h, '/' between ranks.
    int rank = 7, file = 0;
    for (char character : placement_text) {
        if (character == '/') {
            if (file != 8 || rank == 0) { clear(); return false; }
            --rank;
            file = 0;
        } else if (std::isdigit(static_cast<unsigned char>(character))) {
            int empty_squares = character - '0';
            if (empty_squares < 1 || empty_squares > 8 || file + empty_squares > 8) { clear(); return false; }
            file += empty_squares;
        } else {
            int piece = piece_from_character(character);
            if (piece < 0 || file >= 8) { clear(); return false; }
            put_piece(Piece(piece), make_square(file, rank));
            ++file;
        }
    }
    if (rank != 0 || file != 8) { clear(); return false; }

    // Each side must have exactly one king, otherwise king_square() is meaningless.
    if (count_set_bits(pieces_by_type_[WHITE][KING]) != 1 || count_set_bits(pieces_by_type_[BLACK][KING]) != 1) {
        clear();
        return false;
    }

    // 2) Side to move.
    if (side_text == "w") side_to_move_ = WHITE;
    else if (side_text == "b") side_to_move_ = BLACK;
    else { clear(); return false; }

    // 3) Castling rights.
    if (castling_text != "-") {
        for (char character : castling_text) {
            switch (character) {
                case 'K': castling_rights_ |= WHITE_KINGSIDE;  break;
                case 'Q': castling_rights_ |= WHITE_QUEENSIDE; break;
                case 'k': castling_rights_ |= BLACK_KINGSIDE;  break;
                case 'q': castling_rights_ |= BLACK_QUEENSIDE; break;
                default: clear(); return false;
            }
        }
    }

    // 4) En passant target square.
    if (en_passant_text != "-") {
        if (en_passant_text.size() != 2 || en_passant_text[0] < 'a' || en_passant_text[0] > 'h'
            || (en_passant_text[1] != '3' && en_passant_text[1] != '6')) {
            clear();
            return false;
        }
        en_passant_square_ = make_square(en_passant_text[0] - 'a', en_passant_text[1] - '1');
    }

    halfmove_clock_ = halfmove_clock;
    fullmove_number_ = fullmove_number;
    return true;
}

std::string Board::fen() const {
    std::string output;
    for (int rank = 7; rank >= 0; --rank) {
        int empty_squares = 0;
        for (int file = 0; file < 8; ++file) {
            Piece piece = mailbox_[make_square(file, rank)];
            if (piece == NO_PIECE) {
                ++empty_squares;
            } else {
                if (empty_squares) { output += char('0' + empty_squares); empty_squares = 0; }
                output += PIECE_CHARACTERS[piece];
            }
        }
        if (empty_squares) output += char('0' + empty_squares);
        if (rank > 0) output += '/';
    }

    output += (side_to_move_ == WHITE) ? " w " : " b ";

    if (castling_rights_ == NO_CASTLING) {
        output += '-';
    } else {
        if (castling_rights_ & WHITE_KINGSIDE)  output += 'K';
        if (castling_rights_ & WHITE_QUEENSIDE) output += 'Q';
        if (castling_rights_ & BLACK_KINGSIDE)  output += 'k';
        if (castling_rights_ & BLACK_QUEENSIDE) output += 'q';
    }

    output += ' ';
    if (en_passant_square_ == NO_SQUARE) {
        output += '-';
    } else {
        output += char('a' + file_of(en_passant_square_));
        output += char('1' + rank_of(en_passant_square_));
    }

    output += ' ' + std::to_string(halfmove_clock_) + ' ' + std::to_string(fullmove_number_);
    return output;
}

std::string Board::to_string() const {
    std::string output;
    for (int rank = 7; rank >= 0; --rank) {
        output += char('1' + rank);
        output += "  ";
        for (int file = 0; file < 8; ++file) {
            Piece piece = mailbox_[make_square(file, rank)];
            output += (piece == NO_PIECE) ? '.' : PIECE_CHARACTERS[piece];
            output += ' ';
        }
        output += '\n';
    }
    output += "\n   a b c d e f g h\n\n";
    output += "FEN: " + fen() + '\n';
    return output;
}
