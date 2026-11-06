#pragma once

#include <array>
#include <cstddef>

#include "core/move.h"

// Fixed capacity avoids heap allocation. No legal chess position has more than 218 moves.
class MoveList {
public:
    static constexpr std::size_t CAPACITY = 256;

    void add(Move move) { moves_[size_++] = move; }
    std::size_t size() const { return size_; }
    Move operator[](std::size_t index) const { return moves_[index]; }

    const Move* begin() const { return moves_.data(); }
    const Move* end() const { return moves_.data() + size_; }

private:
    std::array<Move, CAPACITY> moves_;
    std::size_t size_ = 0;
};
