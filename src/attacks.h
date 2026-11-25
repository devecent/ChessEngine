#ifndef ATTACKS_H
#define ATTACKS_H

#include <array>
#include <cstddef>

// Precomputed, compile-time tables shared by move generation and attack
// detection, so the hot loops don't redo board-edge checks.

namespace attacks {

// The squares a knight or king on a given square can reach.
struct SquareList {
    int count = 0;
    int squares[8] = {};
};

template <std::size_t N>
constexpr std::array<SquareList, 64> makeLeaperTable(const int (&deltas)[N][2]) {
    std::array<SquareList, 64> table{};
    for(int sq = 0; sq < 64; ++sq) {
        for(const auto& d : deltas) {
            const int r = sq / 8 + d[0], f = sq % 8 + d[1];
            if(r >= 0 && r <= 7 && f >= 0 && f <= 7) table[sq].squares[table[sq].count++] = r * 8 + f;
        }
    }
    return table;
}

// {rank delta, file delta}. The order is the order moves are generated in.
inline constexpr int KnightDeltas[8][2] = {
    { 2,  1 }, { 2, -1 }, {-2,  1 }, {-2, -1 },
    { 1,  2 }, { 1, -2 }, {-1,  2 }, {-1, -2 }
};
inline constexpr int KingDeltas[8][2] = {
    { 1,  0 }, {-1,  0 }, { 0,  1 }, { 0, -1 },
    { 1,  1 }, { 1, -1 }, {-1,  1 }, {-1, -1 }
};
inline constexpr auto Knight = makeLeaperTable(KnightDeltas);
inline constexpr auto King = makeLeaperTable(KingDeltas);

// Sliding directions: offset on the 0..63 board plus rank/file step. The first
// four are diagonals (bishop/queen), the last four orthogonals (rook/queen).
struct Dir {
    int offset;
    int rankDelta;
    int fileDelta;
};

inline constexpr Dir SlideDirs[8] = {
    {  9,  1,  1 }, {  7,  1, -1 }, { -7, -1,  1 }, { -9, -1, -1 },
    {  8,  1,  0 }, { -8, -1,  0 }, {  1,  0,  1 }, { -1,  0, -1 }
};
inline constexpr int FirstDiagonal = 0;
inline constexpr int FirstOrthogonal = 4;

// StepsToEdge[sq][d]: how many squares a slider on sq can travel in
// SlideDirs[d] before leaving the board.
inline constexpr auto StepsToEdge = [] {
    std::array<std::array<int, 8>, 64> steps{};
    for(int sq = 0; sq < 64; ++sq) {
        for(int d = 0; d < 8; ++d) {
            int r = sq / 8, f = sq % 8, n = 0;
            while(true) {
                r += SlideDirs[d].rankDelta;
                f += SlideDirs[d].fileDelta;
                if(r < 0 || r > 7 || f < 0 || f > 7) break;
                ++n;
            }
            steps[sq][d] = n;
        }
    }
    return steps;
}();

}

#endif
