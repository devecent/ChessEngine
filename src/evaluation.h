#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"
#include "move.h"

class Evaluation {

    public:

    Evaluation(Board& b) : board(b) {
        whiteEndGameT = endgameWeight(1);
        blackEndGameT = endgameWeight(0);
    }
    int evaluate();

    private:

    Board& board;
    float blackEndGameT;
    float whiteEndGameT;
    int countMaterial(bool white);

    float endgameWeight(bool white) {
        int phase = 0;
        const int QUEEN_PHASE = 4;
        const int ROOK_PHASE = 2;
        const int BISHOP_PHASE = 1;
        const int KNIGHT_PHASE = 1;
        auto& pieces = white ? board.whitePieces : board.blackPieces;
        phase += KNIGHT_PHASE*pieces[1].size();
        phase += BISHOP_PHASE*pieces[2].size();
        phase += ROOK_PHASE*pieces[3].size();
        phase += QUEEN_PHASE*pieces[4].size();
        phase = std::min(phase,24);
        return 1 - (float) phase / 24.0f;
    }

    int pieceSquareEvaluation(bool white);
    int kingActivity(bool white);

};


#endif