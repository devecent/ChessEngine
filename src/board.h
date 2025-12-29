#ifndef BOARD_H
#define BOARD_H
#include <vector>
#include <cstdint>
#include "move.h"
#include "types.h"



class Board {

    public:

    int board[64];
    std::vector<int> whitePieces[6];
    std::vector<int> blackPieces[6];
    int enpassantSquare;
    int castlingRights;
    bool whiteToPlay;
    int squareIndex[64];
    std::vector<Undo> history;
    uint64_t zobristKey;

    Board();
    
    void print();

    void updateEnPassantSquare(const Move& move);
    void updateCastlingRights(const Move& move, int movingPiece);
    bool isSquareAttacked(int sq, bool byWhite);
    void makeMove(const Move& move);
    void undoMove(const Move& move);
    void removeFromPieceList(int sq, int piece);
    void addToPieceList(int sq, int piece);
    void movePieceInList(int piece, int from, int to);
    uint64_t perft(int depth);
    uint64_t perftDivide(int depth);
    bool isEnPassant(const Move& m) const;
    void checkPieceLists();
    void loadFEN(const std::string& fen);
    bool isCheckmate();
    bool isStalemate();
    uint64_t computeZobristKey();
};

#endif