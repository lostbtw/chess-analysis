#pragma once

#include "core/board.h"
#include "core/move_list.h"

// Generates moves that follow the movement rules but may leave the own king in check.
void generate_pseudo_legal_moves(const Board& board, MoveList& moves);
