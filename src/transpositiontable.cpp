#include "transpositiontable.h"

bool TranspositionTable::lookUp(uint64_t key, int depth, int alpha, int beta, int& outScore, Move& outMove) {
    Entry& e = table[key & (tableSize-1)];
    if(key != e.key) {
        return false;
    }
    if(e.depth >= depth) {
        outMove = e.move;
        if(e.flag == EXACT) {
            outScore = e.score;
            return true;
        }
        else if(e.flag == LOWER && e.score >= beta) {
            outScore = e.score;
            return true;
        }
        else if(e.flag == UPPER && e.score <= alpha) {
            outScore = e.score;
            return true;
        }
    }
    return false;
}

void TranspositionTable::push(uint64_t key, int depth, int alpha, int beta, int score, int ply, Move& move) {
    Entry& e = table[key & (tableSize-1)];
    if(score > MATE_SCORE) score += ply;
    if(score < -MATE_SCORE) score -= ply;
    if(e.key == key && depth < e.depth) return;
    e.key = key;
    e.depth = depth;
    e.score = score;
    e.move = move;
    if(score <= alpha) {
        e.flag = UPPER;
    }
    else if(score >= beta) {
        e.flag = LOWER;
    }
    else {
        e.flag = EXACT;
    }
}