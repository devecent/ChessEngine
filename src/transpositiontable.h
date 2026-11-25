#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <cstdint>
#include <vector>
#include "board.h"
#include "move.h"

enum TTFLag : uint8_t {
    EXACT,
    LOWER,
    UPPER
};

struct Entry {
    uint64_t key;
    int32_t score;
    int8_t depth;
    uint8_t flag;
    uint16_t move;
};
static_assert(sizeof(Entry) == 16, "TT entry should stay 16 bytes");

class TranspositionTable {

    public:

    TranspositionTable(Board& board) : board(board){
        table.resize(tableSize);
    }

    bool lookUp(uint64_t key, int depth, int alpha, int beta, int& outScore, Move& outMove, int ply = 0);
    void push(uint64_t key, int depth, int alpha, int beta, int score, int ply, const Move& move);

    private:

    Board& board;
    static constexpr std::size_t tableSize = std::size_t{1} << 22;
    std::vector<Entry> table;
};



#endif
