#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"
#include "transpositiontable.h"
#include <chrono>
#include <array>


class Search {

    public:

    Search(Board& b, TranspositionTable& table) : board(b), transpositionTable(table){}
    
    Move findBestMove(int time);
    int thinkTime(int whiteTime, int blackTime, int whiteInc, int blackInc);

    private:

    Board& board;
    TranspositionTable& transpositionTable;
    int maxDepth = 40;
    static constexpr int killerPlies = 128;
    static constexpr int historyLimit = 16384;
    std::array<std::array<Move, 2>, killerPlies> killers{};
    std::array<std::array<std::array<int, 64>, 64>, 2> history{};
    std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::time_point::max();
    int negamax(int depth, int alpha, int beta, int ply, bool allowNull = true);
    int searchNullMove(int depth, int alpha, int beta, int ply);
    bool canNullMove(int depth, int beta, int ply);
    int quiescence(int alpha, int beta, int ply);
    void moveOrder(std::vector<Move>& moves, const Move& ttMove, int ply = 0);
    void recordQuietCutoff(const Move& move, int depth, int ply);

    

};


#endif
