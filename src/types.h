#ifndef TYPES_H
#define TYPES_H


#include <string>

enum Piece {
    EMPTY = 0,
    WPAWN = 1, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING,
    BPAWN = 7, BKNIGHT, BBISHOP, BROOK, BQUEEN, BKING
};

struct Undo {
    int capturedPiece;
    int capturedSquare;
    int prevEnpassant;
    int prevCastle;
    uint64_t zobrist;
};

//castling rights
const int WHITE_KINGSIDE  = 1;
const int WHITE_QUEENSIDE = 2;
const int BLACK_KINGSIDE  = 4;
const int BLACK_QUEENSIDE = 8; 


#endif