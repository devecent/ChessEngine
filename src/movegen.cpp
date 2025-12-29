#include "movegen.h"
#include <math.h>
#include <iostream>

using namespace std;

inline bool isWhite(int piece) {
    return piece >= WPAWN && piece <= WKING;
}

inline bool isBlack(int piece) {
    return piece >= BPAWN && piece <= BKING;
}

void MoveGen::generatePawnMoves(int sq, vector<Move>& moves, bool white) {
    int rank = sq/8;
    int file = sq%8;
    int forward = white ? 8 : -8;
    int startRank = white ? 1 : 6;
    int oneForward = sq+forward;
    int twoForward = sq+2*forward;
    int promotionRank = white ? 7 : 0;
    auto& b = board.board;
    if(oneForward >= 0 && oneForward < 64 && !b[oneForward]) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward, EMPTY, WKNIGHT, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WBISHOP, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WROOK, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward, EMPTY, BKNIGHT, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BBISHOP, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BROOK, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BQUEEN, false});
            }
        }
        else {
            moves.push_back(Move{sq, oneForward, EMPTY, EMPTY, false});
        }
        if(rank == startRank && twoForward >= 0 && twoForward < 64 && !b[twoForward]) {
            moves.push_back(Move{sq, twoForward, EMPTY, EMPTY, false});
        }
    }
    //left diagonal capture
    if(file > 0 && oneForward >= 0 && oneForward < 64 && b[oneForward-1] && isWhite(b[oneForward-1]) != white) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WKNIGHT, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WBISHOP, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WROOK, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BKNIGHT, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BBISHOP, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BROOK, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BQUEEN, false});
            }
        }
        else {
            moves.push_back(Move{sq, oneForward-1, b[oneForward-1], EMPTY, false});
        }
    }
    //right diagonal capture
    if(file < 7 && oneForward >= 0 && oneForward < 64 && b[oneForward+1] && isWhite(b[oneForward+1]) != white) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WKNIGHT, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WBISHOP, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WROOK, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BKNIGHT, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BBISHOP, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BROOK, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BQUEEN, false});
            }
        }
        else {
            moves.push_back(Move{sq, oneForward+1, b[oneForward+1], EMPTY, false});
        }
    }
    if(board.enpassantSquare != -1 && file < 7 && board.enpassantSquare == sq+1 && isWhite(b[sq+1]) == !white) {
        moves.push_back(Move{sq, board.enpassantSquare+forward, b[board.enpassantSquare], EMPTY, false});
    }
    if(board.enpassantSquare != -1 && file > 0 && board.enpassantSquare == sq-1 && isWhite(b[sq-1]) == !white) {
        moves.push_back(Move{sq, board.enpassantSquare+forward, b[board.enpassantSquare], EMPTY, false});
    }
}

void MoveGen::generateBishopMoves(int sq, vector<Move>& moves, bool white) {
    auto& b = board.board;
    int file = sq%8;
    int directions[4] = {9,7,-7,-9};
    int fileDirections[4] = {1,-1,1,-1};
    for(int i = 0; i < 4; i++) {
        int curr = sq;
        int f = file;
        while(true) {
            curr += directions[i];
            f += fileDirections[i];
            if(curr >= 64 || curr < 0 || f > 7 || f < 0) {
                break;
            }
            if(b[curr]) {
                if(isWhite(b[curr]) != white) {
                    moves.push_back(Move{sq, curr, b[curr], EMPTY, false});
                }
                break;
            }
            else {
                moves.push_back(Move{sq, curr, EMPTY, EMPTY, false});
            }
        }
    }
}

void MoveGen::generateRookMoves(int sq, vector<Move>& moves, bool white) {
    auto& b = board.board;
    int rank = sq/8;
    int file = sq%8;
    int directions[4] = {8,-8,1,-1};
    int rankDirections[4] = {1,-1,0,0};
    int fileDirections[4] = {0,0,1,-1};
    for(int i = 0; i < 4; i++) {
        int curr = sq;
        int f = file;
        int r = rank;
        while(true) {
            curr += directions[i];
            f += fileDirections[i];
            r += rankDirections[i];
            if(curr >= 64 || curr < 0 || f > 7 || f < 0 || r > 7 || r < 0) {
                break;
            }
            if(b[curr]) {
                if(isWhite(b[curr]) != white) {
                    moves.push_back(Move{sq, curr, b[curr], EMPTY, false});
                }
                break;
            }
            else {
                moves.push_back(Move{sq, curr, EMPTY, EMPTY, false});
            }
        }
    }
}

void MoveGen::generateKingMoves(int sq, vector<Move>& moves, bool white) {
    auto& b = board.board;
    int file = sq%8;
    int directions[8] = {8,-8,1,-1,9,7,-7,-9};
    for(int i = 0; i < 8; i++) {
        int to = sq + directions[i];
        if(to < 0 || to >= 64) continue;
        int toFile = to % 8;
        if(abs(toFile-file) > 1) continue;
        if(b[to] && isWhite(b[to]) == white) continue;
        moves.push_back(Move{sq, to, b[to], EMPTY, false});
    }
}

void MoveGen::generateQueenMoves(int sq, vector<Move>& moves, bool white) {
    generateRookMoves(sq,moves,white);
    generateBishopMoves(sq,moves,white);
}

void MoveGen::generateKnightMoves(int sq, vector<Move>& moves, bool white) {
    auto& b = board.board;
    int rank = sq / 8;
    int file = sq % 8;
    int dr[8] = {2, 2, -2, -2, 1, 1, -1, -1};
    int df[8] = {1, -1, 1, -1, 2, -2, 2, -2};
    for(int i = 0; i < 8; i++) {
        int r = rank+dr[i];
        int f = file+df[i];
        if(r < 0 || r > 7 || f < 0 || f > 7) continue;
        int dest = r * 8 + f;
        if(!b[dest]) {
            moves.push_back(Move{sq, dest, EMPTY, EMPTY, false});
        }
        else if(isWhite(b[dest]) != white) {
            moves.push_back(Move{sq, dest, b[dest], EMPTY, false});
        }
    }
}


void MoveGen::generateCastlingMoves(vector<Move>& moves, bool white) {
    auto& b = board.board;
    int castling = board.castlingRights;
    if(white) {
        if( (castling & WHITE_KINGSIDE) && b[7] == WROOK && b[5] == EMPTY && b[6] == EMPTY && !board.isSquareAttacked(4,!white) && !board.isSquareAttacked(5,!white) && !board.isSquareAttacked(6,!white)) {
            moves.push_back(Move{4, 6, EMPTY, EMPTY, true});
        }
        if( (castling & WHITE_QUEENSIDE) && b[0] == WROOK  && b[1] == EMPTY && b[2] == EMPTY && b[3] == EMPTY && !board.isSquareAttacked(4,!white) && !board.isSquareAttacked(3,!white) && !board.isSquareAttacked(2,!white)) {
            moves.push_back(Move{4,2,EMPTY, EMPTY,true});
        }
    }
    else {
        if( (castling & BLACK_KINGSIDE) && b[63] == BROOK && b[61] == EMPTY && b[62] == EMPTY && !board.isSquareAttacked(60,!white) && !board.isSquareAttacked(61,!white) && !board.isSquareAttacked(62,!white)) {
            moves.push_back(Move{60 ,62, EMPTY, EMPTY, true});
        }
        if( (castling & BLACK_QUEENSIDE) && b[56] == BROOK && b[57] == EMPTY && b[58] == EMPTY && b[59] == EMPTY && !board.isSquareAttacked(60,!white) && !board.isSquareAttacked(59,!white) && !board.isSquareAttacked(58,!white)) {
            moves.push_back(Move{60, 58, EMPTY, EMPTY, true});
        }
    }
}


vector<Move> MoveGen::generatePseudoLegalMoves(bool whiteToMove) {
    vector<Move> moves;
    moves.reserve(256);
    auto& pieces = whiteToMove ? board.whitePieces : board.blackPieces;
    for(int sq : pieces[0]) {
        generatePawnMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[1]) {
        generateKnightMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[2]) {
        generateBishopMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[3]) {
        generateRookMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[4]) {
        generateQueenMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[5]) {
        generateKingMoves(sq,moves,whiteToMove);
    }
    generateCastlingMoves(moves,whiteToMove);
    return moves;
}

vector<Move> MoveGen::generateMoves(bool whiteToMove) {
    vector<Move> potentialMoves = generatePseudoLegalMoves(whiteToMove);
    vector<Move> moves;
    moves.reserve(256);
    
    for(const Move& move : potentialMoves) {
        board.makeMove(move);
        int kingSquare = whiteToMove ? board.whitePieces[5][0] : board.blackPieces[5][0];
        if(!board.isSquareAttacked(kingSquare,!whiteToMove)) {
            moves.push_back(move);
        }
        board.undoMove(move);
    }
    return moves;
}

