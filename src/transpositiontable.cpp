#include "transpositiontable.h"

bool TranspositionTable::lookUp(uint64_t key, int depth, int alpha, int beta, int& outScore, Move& outMove, int ply) {
    Entry& e = table[key & (tableSize-1)];
    if(key != e.key) {
        return false;
    }
    outMove = e.move;
    int score = e.score;
    if(score >= MATE_SCORE - 1000) score -= ply;
    if(score <= -MATE_SCORE + 1000) score += ply;
    if(e.depth >= depth) {
        if(e.flag == EXACT) {
            outScore = score;
            return true;
        }
        else if(e.flag == LOWER && score >= beta) {
            outScore = score;
            return true;
        }
        else if(e.flag == UPPER && score <= alpha) {
            outScore = score;
            return true;
        }
    }
    return false;
}

void TranspositionTable::push(uint64_t key, int depth, int alpha, int beta, int score, int ply, Move& move) {
    Entry& e = table[key & (tableSize-1)];
    const int searchScore = score;
    if(score >= MATE_SCORE - 1000) score += ply;
    if(score <= -MATE_SCORE + 1000) score -= ply;
    if(e.key == key && depth < e.depth) return;
    e.key = key;
    e.depth = depth;
    e.score = score;
    e.move = move;
    if(searchScore <= alpha) {
        e.flag = UPPER;
    }
    else if(searchScore >= beta) {
        e.flag = LOWER;
    }
    else {
        e.flag = EXACT;
    }
}