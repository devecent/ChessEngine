#include "transpositiontable.h"

namespace {
uint16_t packMove(const Move& move) {
    if(move.from < 0 || move.to < 0) return 0;
    unsigned promo = 0;
    switch(move.promotion) {
        case WKNIGHT: case BKNIGHT: promo = 1; break;
        case WBISHOP: case BBISHOP: promo = 2; break;
        case WROOK:   case BROOK:   promo = 3; break;
        case WQUEEN:  case BQUEEN:  promo = 4; break;
        default: break;
    }
    return static_cast<uint16_t>(move.from | (move.to << 6) | (promo << 12) | (move.castling ? 1u << 15 : 0u));
}

Move unpackMove(uint16_t packed) {
    if(packed == 0) return Move();
    const int from = packed & 0x3F;
    const int to = (packed >> 6) & 0x3F;
    const unsigned promo = (packed >> 12) & 0x7;
    // Promotions land on the last rank, so the destination tells us the colour.
    const bool white = to >= 56;
    static constexpr Piece whitePromo[5] = {EMPTY, WKNIGHT, WBISHOP, WROOK, WQUEEN};
    static constexpr Piece blackPromo[5] = {EMPTY, BKNIGHT, BBISHOP, BROOK, BQUEEN};
    const Piece promotion = promo <= 4 ? (white ? whitePromo[promo] : blackPromo[promo]) : EMPTY;
    return Move(from, to, EMPTY, promotion, (packed >> 15) != 0);
}
}

bool TranspositionTable::lookUp(uint64_t key, int depth, int alpha, int beta, int& outScore, Move& outMove, int ply) {
    Entry& e = table[key & (tableSize-1)];
    if(key != e.key) {
        return false;
    }
    outMove = unpackMove(e.move);
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

void TranspositionTable::push(uint64_t key, int depth, int alpha, int beta, int score, int ply, const Move& move) {
    Entry& e = table[key & (tableSize-1)];
    const int searchScore = score;
    if(score >= MATE_SCORE - 1000) score += ply;
    if(score <= -MATE_SCORE + 1000) score -= ply;
    if(e.key == key && depth < e.depth) return;
    e.key = key;
    e.depth = static_cast<int8_t>(depth);
    e.score = score;
    e.move = packMove(move);
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
