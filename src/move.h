#ifndef MOVE_H
#define MOVE_H

#include "types.h"

class Board;

class Move {

    public:

    Move() : from(0), to(0), captured(EMPTY), promotion(EMPTY), castling(false) {}
    Move(int f, int t, int c, Piece p, bool cast) : from(f), to(t), captured(c), promotion(p), castling(cast) {} 
    int from;
    int to;
    int captured;
    Piece promotion;
    bool castling;

    std::string toString() const;

    static Move fromUci(const std::string& uci, Board& board);


    std::string toUci() const;


};

#endif