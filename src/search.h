#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"


class Search {

    public:

    Search(Board& b) : board(b){}
    
    Move findBestMove(int time);
    int thinkTime(int whiteTime, int blackTime, int whiteInc, int blackInc);

    private:

    Board& board;
    int maxDepth = 40;
    int negamax(int depth, int alpha, int beta, int ply);

    

};


#endif