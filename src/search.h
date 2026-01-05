#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"
#include "transpositiontable.h"


class Search {

    public:

    Search(Board& b, TranspositionTable& table) : board(b), transpositionTable(table){}
    
    Move findBestMove(int time);
    int thinkTime(int whiteTime, int blackTime, int whiteInc, int blackInc);

    private:

    Board& board;
    TranspositionTable& transpositionTable;
    int maxDepth = 40;
    int negamax(int depth, int alpha, int beta, int ply);
    void moveOrder(std::vector<Move>& moves, Move& ttMove);

    

};


#endif