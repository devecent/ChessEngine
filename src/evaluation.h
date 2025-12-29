#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"
#include "move.h"

class Evaluation {

    public:

    Evaluation(Board& b) : board(b) {
        endgameT = endgameWeight();
    }
    int evaluate();

    private:

    Board& board;
    float endgameT;
    int countMaterial(bool white);

    float endgameWeight() {
        int phase = 0;
        const int QUEEN_PHASE = 4;
        const int ROOK_PHASE = 2;
        const int BISHOP_PHASE = 1;
        const int KNIGHT_PHASE = 1;
        phase += KNIGHT_PHASE*(board.whitePieces[1].size()+board.blackPieces[1].size());
        phase += BISHOP_PHASE*(board.whitePieces[2].size()+board.blackPieces[2].size());
        phase += ROOK_PHASE*(board.whitePieces[3].size()+board.blackPieces[3].size());
        phase += QUEEN_PHASE*(board.whitePieces[4].size()+board.blackPieces[4].size());
        phase = std::min(phase,24);
        return 1 - (float) phase / 24.0f;
    }

    int pieceSquareEvaluation(bool white);

};


#endif