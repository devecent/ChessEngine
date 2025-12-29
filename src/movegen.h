#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <vector>
#include <cstdint>
#include "board.h"
#include "types.h"
#include "move.h"



class MoveGen {

    public:

    MoveGen(Board& b) : board(b) {}

    std::vector<Move> generateMoves(bool whiteToMove);

    private: 

    Board& board;
    void generatePawnMoves(int sq, std::vector<Move>& moves, bool white);
    void generateKnightMoves(int sq, std::vector<Move>& moves, bool white);
    void generateRookMoves(int sq, std::vector<Move>& moves, bool white);
    void generateKingMoves(int sq, std::vector<Move>& moves, bool white);
    void generateBishopMoves(int sq, std::vector<Move>& moves, bool white);
    void generateQueenMoves(int sq, std::vector<Move>& moves, bool white);
    void generateCastlingMoves(std::vector<Move>& moves, bool white);
    uint64_t perft(int depth);
    std::vector<Move> generatePseudoLegalMoves(bool whiteToMove);

};

#endif