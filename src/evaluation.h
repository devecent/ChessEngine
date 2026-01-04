#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"
#include "move.h"

class Evaluation {

    public:

    Evaluation(Board& b) : board(b) {
        whiteEndGameT = endgameWeight(1);
        blackEndGameT = endgameWeight(0);
        kingPawnShieldsWhite = precomputeKingPawnShields(1);
        kingPawnShieldsBlack = precomputeKingPawnShields(0);
    }
    int evaluate();

    private:

    Board& board;
    float blackEndGameT;
    float whiteEndGameT;
    std::vector<std::vector<int>> kingPawnShieldsWhite;
    std::vector<std::vector<int>> kingPawnShieldsBlack;
    int kingPawnShieldScores[6] = { 4, 7, 4, 3, 6, 3 };

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
        phase = std::min(phase,12);
        return 1 - (float) phase / 12.0f;
    }
    int pieceSquareEvaluation(bool white);
    int kingActivity(bool white);
    int kingSafety(bool white, int pieceSquareEval);
    std::vector<std::vector<int>> precomputeKingPawnShields(bool white) {
        std::vector<std::vector<int>> kingPawnShields(64);
        for(int sq = 0; sq < 64; sq++) {
            int file = sq % 8;
            int rank = sq / 8;
            int forward = white ? 1 : -1;
            for(int dr = 1; dr <= 2; dr++) {
                for(int df = -1; df <= 1; df++) {
                    int f = file + df;
                    int r = rank + forward*dr;
                    if(f < 0 || f > 7 || r < 0 || r > 7) continue;
                    kingPawnShields[sq].push_back(r * 8 + f);
                }
            }
        }
        return kingPawnShields;
    }
};


#endif