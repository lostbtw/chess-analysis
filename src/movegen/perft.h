#pragma once

#include <cstdint>
#include <vector>

#include "core/board.h"

struct DivideEntry {
    Move move;
    std::uint64_t nodes;
};

// Number of positions reachable in exactly `depth` plies.
std::uint64_t perft(Board& board, int depth);

// Perft split by the first move, used to find the exact move where a generator bug hides.
std::vector<DivideEntry> perft_divide(Board& board, int depth);
