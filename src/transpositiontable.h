#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "board.h"

enum TTFLag : uint8_t {
    EXACT,
    LOWER,
    UPPER
};

struct Entry {
    uint64_t key;
    int score;
    int8_t depth;
    uint8_t flag;
    Move move;
};

class TranspositionTable {

    public:

    TranspositionTable(Board& board) : board(board){
        // just a setsize
        table.resize(tableSize);
    }

    bool lookUp(uint64_t key, int depth, int alpha, int beta, int& outScore, Move& outMove);
    void push(uint64_t key, int depth, int alpha, int beta, int score, int ply, Move& move);

    private:

    Board& board;
    const int tableSize = 1 << 22;
    std::vector<Entry> table;
};



#endif