#ifndef BOARDHELPERS_H
#define BOARDHELPERS_H

#include "types.h"

inline bool isWhite(int piece) {
    return piece >= WPAWN && piece <= WKING;
}

inline bool isPawn(int p) {
    return p == WPAWN || p == BPAWN;
}
inline bool isKnight(int p) {
    return p == WKNIGHT || p == BKNIGHT;
}
inline bool isBishop(int p) {
    return p == WBISHOP || p == BBISHOP;
}
inline bool isRook(int p) {
    return p == WROOK || p == BROOK;
}
inline bool isQueen(int p) {
    return p == WQUEEN || p == BQUEEN;
}
inline bool isKing(int p) {
    return p == WKING || p == BKING;
}

inline int charToPiece(char c) {
    switch(c) {
        case 'P': return WPAWN;
        case 'N': return WKNIGHT;
        case 'B': return WBISHOP;
        case 'R': return WROOK;
        case 'Q': return WQUEEN;
        case 'K': return WKING;
        case 'p': return BPAWN;
        case 'n': return BKNIGHT;
        case 'b': return BBISHOP;
        case 'r': return BROOK;
        case 'q': return BQUEEN;
        case 'k': return BKING;
        default : return EMPTY;
    }
}

inline int squareStringToIndex(const std::string& sq) {
    if(sq == "-") return -1;
    int file = sq[0] - 'a';
    int rank = sq[1] - '1';
    return rank * 8 + file;
}

inline int pieceType(int piece) {
    switch (piece) {
        case WPAWN:
        case BPAWN: return 0;
        case WKNIGHT:
        case BKNIGHT: return 1;
        case WBISHOP:
        case BBISHOP: return 2;
        case WROOK:
        case BROOK: return 3;
        case WQUEEN:
        case BQUEEN: return 4;
        case WKING:
        case BKING: return 5;
        default: return -1;
    }
}



#endif