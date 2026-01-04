#include "evaluation.h"
#include "piecesquaretable.h"
#include <vector>
#include <algorithm>

using namespace std;

const int PawnValue = 100;
const int KnightValue = 300;
const int BishopValue = 320;
const int RookValue = 500;
const int QueenValue = 900;

int Evaluation::evaluate() {
    int whiteEval = 0;
    int blackEval = 0;
    int pieceSquareEvalWhite = pieceSquareEvaluation(1);
    int pieceSquareEvalBlack = pieceSquareEvaluation(0);
    whiteEval += countMaterial(1);
    whiteEval += pieceSquareEvalWhite;
    whiteEval += kingActivity(1);
    whiteEval += kingSafety(1, pieceSquareEvalBlack);
    blackEval += countMaterial(0);
    blackEval += pieceSquareEvalBlack;
    blackEval += kingActivity(0);
    blackEval += kingSafety(0, pieceSquareEvalWhite);
    int sign = board.whiteToPlay ? 1 : -1;
    int eval = whiteEval-blackEval;
    return sign * eval;
}


int Evaluation::countMaterial(bool white) {
    int material = 0;
    auto& pieces = white ? board.whitePieces : board.blackPieces;
    material += PawnValue*pieces[0].size();
    material += KnightValue*pieces[1].size();
    material += BishopValue*pieces[2].size();
    material += RookValue*pieces[3].size();
    material += QueenValue*pieces[4].size();
    return material;
}

int Evaluation::pieceSquareEvaluation(bool white) {
    int earlyPawn = 0;
    int latePawn = 0;
    auto& pieces = white ? board.whitePieces : board.blackPieces;
    float endgameT = white ? blackEndGameT : whiteEndGameT;
    for(int sq : pieces[0]) {
        earlyPawn += readfromTable(PawnTable, sq, white);
        latePawn += readfromTable(PawnsEndTable, sq, white);
    }
    int value = (int)(earlyPawn*(1-endgameT));
    value += (int)(latePawn*endgameT);
    for(int sq : pieces[1]) {
        value += readfromTable(KnightsTable, sq, white);
    }
    for(int sq : pieces[2]) {
        value += readfromTable(BishopsTable, sq, white);
    }
    for(int sq : pieces[3]) {
        value += readfromTable(RooksTable, sq, white);
    }
    for(int sq : pieces[4]) {
        value += readfromTable(QueensTable, sq, white);
    }
    int kingEarly = readfromTable(KingStartTable, pieces[5][0], white);
    int kingLate = readfromTable(KingEndTable, pieces[5][0], white);
    value += (int)(kingEarly*(1-endgameT));
    value += (int)(kingLate*endgameT);
    return value;
}

inline int distance(int sq1, int sq2) {
    int rank1 = sq1 / 8;
    int file1 = sq1 % 8;
    int rank2 = sq2 / 8;
    int file2 = sq2 % 8;
    return abs(rank2 - rank1) + abs(file2 - file1);
}

inline int distanceFromCenter(int sq) {
    int centers[4] = {27, 28, 35, 36};
    int mn = 20;
    for(int i = 0; i < 4; i++) {
        mn = min(mn,distance(centers[i],sq));
    }
    return mn;
}

int Evaluation::kingActivity(bool white) {
    if(countMaterial(white) <= countMaterial(!white) + PawnValue*2) return 0;
    float endgameT = white ? blackEndGameT : whiteEndGameT;
    if(endgameT > 0) return 0;
    int value = 0;
    int friendlyKingSq = white ? board.whitePieces[5][0] : board.blackPieces[5][0];
    int enemyKingSq = white ? board.blackPieces[5][0] : board.whitePieces[5][0];
    value += (14-distance(friendlyKingSq, enemyKingSq)) * 4;
    value += distanceFromCenter(enemyKingSq)*10;
    return (int)value*endgameT;
}

int Evaluation::kingSafety(bool white, int pieceSquareEval) {
    float endgameT = white ? blackEndGameT : whiteEndGameT;
    return 0;
    if(endgameT >= 1) return 0;
    int kingsq = white ? board.whitePieces[5][0] : board.blackPieces[5][0];
    int kingFile = kingsq % 8;
    int friendlyPawn = white ? WPAWN : BPAWN;
    int penalty = 0;
    if(kingFile <= 2 || kingFile >= 5) {
        vector<int>& squares = white ? kingPawnShieldsWhite[kingsq] : kingPawnShieldsBlack[kingsq];
        for(int i = 0; i < squares.size()/2; i++) {
            if(board.board[squares[i]] != friendlyPawn) {
                if(squares.size() > 3 && board.board[squares[i+3]] == friendlyPawn) {
                    penalty += kingPawnShieldScores[i+3];
                }
                else {
                    penalty += kingPawnShieldScores[i];
                }
            }
            penalty *= penalty;
        }
    }
    else {
        float enemyDevelopment = (float)(pieceSquareEval+10)/130.0;
        enemyDevelopment = clamp(enemyDevelopment,0.0f,1.0f);
        penalty += (int)(50*enemyDevelopment);
    }
    float weight = 1-endgameT;
    return (int)-penalty*weight;
}







