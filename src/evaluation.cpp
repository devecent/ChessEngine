#include "evaluation.h"
#include "piecesquaretable.h"

using namespace std;

const int PawnValue = 100;
const int KnightValue = 300;
const int BishopValue = 320;
const int RookValue = 500;
const int QueenValue = 900;

int Evaluation::evaluate() {
    int whiteEval = 0;
    int blackEval = 0;
    whiteEval += countMaterial(1);
    whiteEval += pieceSquareEvaluation(1);
    blackEval += countMaterial(0);
    blackEval += pieceSquareEvaluation(0);
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







