#pragma once

#include "core/board.h"

// All values are in centipawns and are given from White's point of view:
// positive means White is better, negative means Black is better.
struct EvaluationBreakdown {
    int material;
    int piece_square;

    int total() const { return material + piece_square; }
};

EvaluationBreakdown evaluate_breakdown(const Board& board);

// Score from the point of view of the side to move, as search needs it.
int evaluate(const Board& board);
