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

    // All legal moves.
    std::vector<Move> generateMoves(bool whiteToMove);
    // Legal captures (en passant included) and promotions (quiet ones included),
    // in the same relative order generateMoves would list them. For quiescence.
    std::vector<Move> generateTacticalMoves(bool whiteToMove);
    // Whether any legal move exists; stops at the first one found.
    bool hasLegalMove(bool whiteToMove);

    private:

    struct LegalityContext {
        int ownKing;
        bool inCheck;
    };

    Board& board;
    bool tacticalOnly = false;  // generators skip quiet moves while set
    LegalityContext legalityContext(bool whiteToMove);
    bool isLegal(const Move& move, const LegalityContext& ctx, bool whiteToMove);
    void filterLegal(std::vector<Move>& moves, bool whiteToMove);
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