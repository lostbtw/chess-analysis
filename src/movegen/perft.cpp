#include "movegen/perft.h"

#include "movegen/movegen.h"

std::uint64_t perft(Board& board, int depth) {
    if (depth == 0) return 1;

    MoveList moves;
    generate_legal_moves(board, moves);
    if (depth == 1) return moves.size();

    std::uint64_t nodes = 0;
    for (Move move : moves) {
        board.make_move(move);
        nodes += perft(board, depth - 1);
        board.unmake_move();
    }
    return nodes;
}

std::vector<DivideEntry> perft_divide(Board& board, int depth) {
    std::vector<DivideEntry> entries;
    if (depth == 0) return entries;

    MoveList moves;
    generate_legal_moves(board, moves);
    for (Move move : moves) {
        board.make_move(move);
        entries.push_back({move, perft(board, depth - 1)});
        board.unmake_move();
    }
    return entries;
}
