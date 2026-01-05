#include "search.h"
#include "movegen.h"
#include <iostream>
#include "evaluation.h"
#include <cmath>
#include <chrono>
#include <algorithm>


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
        int bestEval = -MATE_SCORE;
        Move currentBestMove;
        MoveGen movegen(board);
        vector<Move> moves = movegen.generateMoves(board.whiteToPlay);
        if(bestMove.from != -1) {
            for(int i = 0; i < (int) moves.size(); i++) {
                if(moves[i] == bestMove) {
                    swap(moves[0],moves[i]);
                    break;
                }
            }
        }
        for(Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(depth-1, -1000000, 1000000, 0);
            board.undoMove(move);
            if(eval > bestEval || currentBestMove.from == -1) {
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
    if(board.fiftyMoveCounter >= 100 || board.isRepeated()) return 0;
    if(depth == 0) {
        return Evaluation(board).evaluate();
    }
    int score;
    Move ttMove;
    if(transpositionTable.lookUp(board.zobristKey, depth, alpha, beta, score, ttMove)) {
        return score;
    }
    int ogalpha = alpha;
    int maxEval = -MATE_SCORE;
    MoveGen movegen(board);
    vector<Move> moves = movegen.generateMoves(board.whiteToPlay);
    if(moves.empty()) {
        int kingsq = board.whiteToPlay ? board.whitePieces[5][0] : board.blackPieces[5][0];
        if(board.isSquareAttacked(kingsq,!board.whiteToPlay)) {
            return -MATE_SCORE+ply;
        }
        return 0;
    }
    moveOrder(moves, ttMove);
    Move bestMove;
    for(Move& move : moves) {
        board.makeMove(move);
        int eval = -negamax(depth-1, -beta, -alpha, ply+1);
        board.undoMove(move);
        if(eval > maxEval) {
            bestMove = move;
            maxEval = eval;
        }
        alpha = max(alpha,eval);
        if(alpha >= beta) break;
    }
    transpositionTable.push(board.zobristKey, depth, ogalpha, beta, maxEval, ply, bestMove);
    return maxEval;
}


inline int pieceType(int piece) {
    switch (piece) {
        case WPAWN:
        case BPAWN: return 0;
        case WKNIGHT:
        case BKNIGHT: return 1;
        case WBISHOP:
        case BBISHOP: return 2;
        case WROOK:
        case BROOK: return 3;
        case WQUEEN:
        case BQUEEN: return 4;
        case WKING:
        case BKING: return 5;
        default: return -1;
    }
}

void Search::moveOrder(vector<Move>& moves, Move& ttMove) {
    const int million = 1000000;
    const int ttScore = 100 * million;
    const int capture = 8 * million;
    const int promotion = 6 * million;
    int pieceValue[5] = {100, 300, 320, 500, 900};
    vector<pair<int,Move>> temp(moves.size());
    for(int i = 0; i < moves.size(); i++) {
        Move& move = moves[i];
        int total = 0;
        if(move == ttMove) {
            temp[i] = {ttScore, move};
            continue;
        }
        if(move.captured != EMPTY) {
            total += pieceValue[pieceType(move.captured)] * 10 - pieceValue[board.board[move.from]];
        }
        if(move.promotion != EMPTY) {
            total += promotion;
        }
        temp[i] = {total, move};
    }
    sort(temp.begin(), temp.end(), [](const pair<int,Move>& a, const pair<int,Move>& b) {
        return a.first > b.first;
    });
    for(int i = 0; i < moves.size(); i++) {
        moves[i] = temp[i].second;
    }
}