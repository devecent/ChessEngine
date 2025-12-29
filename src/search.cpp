#include "search.h"
#include "movegen.h"
#include <iostream>
#include "evaluation.h"
#include <cmath>
#include <chrono>


using namespace std;

int Search::thinkTime(int whiteTime, int blackTime, int whiteInc, int blackInc) {
    int myTime = board.whiteToPlay ? whiteTime : blackTime;
    int myIncrement = board.whiteToPlay ? whiteInc : blackInc;
    double think = (double) myTime/40.0;
    if(myTime > myIncrement*2) {
        think += myIncrement* 0.8;
    }
    double minThink = min(50.0, myTime * 0.25);
    return (int)ceil(max(minThink,think));
}


Move Search::findBestMove(int timeLimitMs) {
    Move bestMove;
    auto startTime = chrono::steady_clock::now();
    for(int depth = 1; depth <= maxDepth; depth++) {
        int bestEval = -1000000;
        Move currentBestMove;

        MoveGen movegen(board);
        vector<Move> moves = movegen.generateMoves(board.whiteToPlay);

        for(Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth-1, -1000000, 1000000, 0);
            board.undoMove(move);

            if(eval > bestEval) {
                bestEval = eval;
                currentBestMove = move;
            }

            auto now = chrono::steady_clock::now();
            int elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if(elapsed >= timeLimitMs) {
                goto done;
            }
        }
        bestMove = currentBestMove;

        auto now = chrono::steady_clock::now();
        int elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
        if(elapsed >= timeLimitMs) break;
    }

done:
    return bestMove;
}


int Search::negamax(int depth, int alpha, int beta, int ply) {
    if(depth == 0) {
        return Evaluation(board).evaluate();
    }
    int maxEval = -1000000;
    MoveGen movegen(board);
    vector<Move> moves = movegen.generateMoves(board.whiteToPlay);
    if(moves.empty()) {
        int kingsq = board.whiteToPlay ? board.whitePieces[5][0] : board.blackPieces[5][0];
        if(board.isSquareAttacked(kingsq,!board.whiteToPlay)) {
            return -1000000+ply;
        }
        return 0;
    }
    for(Move& move : moves) {
        board.makeMove(move);
        int eval = -negamax(depth-1, -beta, -alpha, ply+1);
        board.undoMove(move);
        maxEval = max(maxEval,eval);
        alpha = max(alpha,eval);
        if(alpha >= beta) break;
    }
    return maxEval;
}