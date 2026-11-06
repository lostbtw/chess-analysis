#include <chrono>
#include <iostream>
#include <string>

#include "core/board.h"
#include "eval/evaluation.h"
#include "movegen/perft.h"

namespace {

std::string join_arguments(int argc, char** argv, int first_index) {
    std::string joined;
    for (int index = first_index; index < argc; ++index) {
        if (index > first_index) joined += ' ';
        joined += argv[index];
    }
    return joined;
}

int run_perft(int depth, const std::string& fen) {
    Board board;
    if (!board.set_fen(fen)) {
        std::cerr << "Invalid FEN: " << fen << '\n';
        return 1;
    }

    const auto start_time = std::chrono::steady_clock::now();
    const std::vector<DivideEntry> entries = perft_divide(board, depth);
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start_time;

    std::uint64_t total_nodes = 0;
    for (const DivideEntry& entry : entries) {
        std::cout << entry.move.to_uci_string() << ": " << entry.nodes << '\n';
        total_nodes += entry.nodes;
    }
    std::cout << "\nNodes: " << total_nodes << "\nTime: " << elapsed.count() << " s\n";
    if (elapsed.count() > 0) std::cout << "NPS: " << static_cast<std::uint64_t>(total_nodes / elapsed.count()) << '\n';
    return 0;
}

int run_eval(const std::string& fen) {
    Board board;
    if (!board.set_fen(fen)) {
        std::cerr << "Invalid FEN: " << fen << '\n';
        return 1;
    }

    const EvaluationBreakdown breakdown = evaluate_breakdown(board);
    std::cout << "{\"fen\": \"" << board.fen() << "\", \"terms\": ["
              << "{\"name\": \"material\", \"value\": " << breakdown.material << "}, "
              << "{\"name\": \"piece_square_tables\", \"value\": " << breakdown.piece_square << "}], "
              << "\"total\": " << breakdown.total() << "}\n";
    return 0;
}

}

// Usage: chess [FEN]               print the position
//        chess perft DEPTH [FEN]   count nodes, split by first move
//        chess eval [FEN]          evaluation split into terms, as JSON (White's point of view)
int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "eval")
        return run_eval(argc > 2 ? join_arguments(argc, argv, 2) : START_FEN);

    if (argc > 2 && std::string(argv[1]) == "perft") {
        const std::string fen = (argc > 3) ? join_arguments(argc, argv, 3) : START_FEN;
        return run_perft(std::stoi(argv[2]), fen);
    }

    const std::string fen = (argc > 1) ? join_arguments(argc, argv, 1) : START_FEN;
    Board board;
    if (!board.set_fen(fen)) {
        std::cerr << "Invalid FEN: " << fen << '\n';
        return 1;
    }
    std::cout << board.to_string();
    return 0;
}
