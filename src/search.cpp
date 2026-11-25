#include "search.h"
#include "movegen.h"
#include "evaluation.h"
#include "boardHelpers.h"
#include "zobrist.h"
#include <algorithm>
#include <cmath>

using namespace std;

namespace {
struct SearchTimeout {};

class ScopedMove {
public:
    ScopedMove(Board& b, const Move& m) : board(b), move(m) { board.makeMove(move); }
    ~ScopedMove() { board.undoMove(move); }
    ScopedMove(const ScopedMove&) = delete;
    ScopedMove& operator=(const ScopedMove&) = delete;
private:
    Board& board;
    const Move& move;
};

class ScopedNullMove {
public:
    explicit ScopedNullMove(Board& b)
        : board(b), enpassant(b.enpassantSquare), key(b.zobristKey) {
        savedRepetition.push_back(0);
        if(board.enPassantCapturable()) board.zobristKey ^= Zobrist::enpassant(enpassant);
        board.enpassantSquare = -1;
        board.whiteToPlay = !board.whiteToPlay;
        board.zobristKey ^= Zobrist::side();
        savedRepetition[0] = board.zobristKey;
        board.repetitionHistory.swap(savedRepetition);
    }
    ~ScopedNullMove() {
        board.repetitionHistory.swap(savedRepetition);
        board.whiteToPlay = !board.whiteToPlay;
        board.enpassantSquare = enpassant;
        board.zobristKey = key;
    }
    ScopedNullMove(const ScopedNullMove&) = delete;
    ScopedNullMove& operator=(const ScopedNullMove&) = delete;
private:
    Board& board;
    int enpassant;
    uint64_t key;
    vector<uint64_t> savedRepetition;
};
}

int Search::thinkTime(int whiteTime, int blackTime, int whiteInc, int blackInc) {
    int myTime = board.whiteToPlay ? whiteTime : blackTime;
    int myIncrement = board.whiteToPlay ? whiteInc : blackInc;
    double think = static_cast<double>(myTime) / 40.0;
    if(myTime > myIncrement * 2) think += myIncrement * 0.8;
    double minThink = min(50.0, myTime * 0.25);
    return static_cast<int>(ceil(max(minThink, think)));
}

Move Search::findBestMove(int timeLimitMs) {
    killers = {};
    history = {};
    deadline = chrono::steady_clock::now() + chrono::milliseconds(max(0, timeLimitMs));
    vector<Move> moves = MoveGen(board).generateMoves(board.whiteToPlay);
    if(moves.empty()) {
        deadline = chrono::steady_clock::time_point::max();
        return Move();
    }
    Move bestMove = moves.front();
    try {
        for(int depth = 1; depth <= maxDepth; ++depth) {
            moveOrder(moves, bestMove, 0);
            auto previousBest = find(moves.begin(), moves.end(), bestMove);
            if(previousBest != moves.end()) iter_swap(moves.begin(), previousBest);
            int bestEval = -MATE_SCORE;
            Move currentBestMove = moves.front();
            for(const Move& move : moves) {
                int eval;
                {
                    ScopedMove madeMove(board, move);
                    eval = -negamax(depth - 1, -MATE_SCORE, -bestEval, 1);
                }
                if(eval > bestEval) {
                    bestEval = eval;
                    currentBestMove = move;
                }
            }
            bestMove = currentBestMove;
        }
    } catch(const SearchTimeout&) {
    }
    deadline = chrono::steady_clock::time_point::max();
    return bestMove;
}

bool Search::canNullMove(int depth, int beta, int ply) {
    if(depth < 3 || ply == 0 || abs(beta) >= MATE_SCORE - 1000
       || board.fiftyMoveCounter >= 90) return false;
    const auto& pieces = board.whiteToPlay ? board.whitePieces : board.blackPieces;
    if(pieces[3].empty() && pieces[4].empty() && pieces[1].size() + pieces[2].size() < 2)
        return false;
    if(board.isSquareAttacked(pieces[5][0], !board.whiteToPlay)) return false;
    return Evaluation(board).evaluate() >= beta;
}

int Search::searchNullMove(int depth, int alpha, int beta, int ply) {
    ScopedNullMove passed(board);
    return -negamax(depth, -beta, -alpha, ply + 1, false);
}

int Search::negamax(int depth, int alpha, int beta, int ply, bool allowNull) {
    if(depth <= 0) return quiescence(alpha, beta, ply);
    if(chrono::steady_clock::now() >= deadline) throw SearchTimeout{};
    vector<Move> moves = MoveGen(board).generateMoves(board.whiteToPlay);
    if(moves.empty()) {
        int kingsq = board.whiteToPlay ? board.whitePieces[5][0] : board.blackPieces[5][0];
        return board.isSquareAttacked(kingsq, !board.whiteToPlay) ? -MATE_SCORE + ply : 0;
    }
    if(board.fiftyMoveCounter >= 100 || board.isRepeated()) return 0;

    int score;
    Move ttMove;
    if(allowNull && transpositionTable.lookUp(board.zobristKey, depth, alpha, beta, score, ttMove, ply)) return score;
    if(allowNull && canNullMove(depth, beta, ply)) {
        const int reduction = 2 + depth / 6;
        if(searchNullMove(depth - 1 - reduction, beta - 1, beta, ply) >= beta)
            return beta;
    }
    const int originalAlpha = alpha;
    int maxEval = -MATE_SCORE;
    moveOrder(moves, ttMove, ply);
    Move bestMove = moves.front();
    for(const Move& move : moves) {
        int eval;
        {
            ScopedMove madeMove(board, move);
            eval = -negamax(depth - 1, -beta, -alpha, ply + 1, allowNull);
        }
        if(eval > maxEval) {
            bestMove = move;
            maxEval = eval;
        }
        alpha = max(alpha, eval);
        if(alpha >= beta) {
            if(move.captured == EMPTY && move.promotion == EMPTY)
                recordQuietCutoff(move, depth, ply);
            break;
        }
    }
    if(allowNull) transpositionTable.push(board.zobristKey, depth, originalAlpha, beta, maxEval, ply, bestMove);
    return maxEval;
}

int Search::quiescence(int alpha, int beta, int ply) {
    if(chrono::steady_clock::now() >= deadline) throw SearchTimeout{};
    vector<Move> moves = MoveGen(board).generateMoves(board.whiteToPlay);
    const int kingSquare = board.whiteToPlay ? board.whitePieces[5][0] : board.blackPieces[5][0];
    const bool inCheck = board.isSquareAttacked(kingSquare, !board.whiteToPlay);
    if(moves.empty()) return inCheck ? -MATE_SCORE + ply : 0;
    if(board.fiftyMoveCounter >= 100 || board.isRepeated()) return 0;

    int best = -MATE_SCORE;
    if(!inCheck) {
        best = Evaluation(board).evaluate();
        if(best >= beta) return best;
        alpha = max(alpha, best);
        moves.erase(remove_if(moves.begin(), moves.end(), [](const Move& move) {
            return move.captured == EMPTY && move.promotion == EMPTY;
        }), moves.end());
    }

    Move noTTMove;
    moveOrder(moves, noTTMove, ply);
    for(const Move& move : moves) {
        int score;
        {
            ScopedMove madeMove(board, move);
            score = -quiescence(-beta, -alpha, ply + 1);
        }
        best = max(best, score);
        alpha = max(alpha, score);
        if(alpha >= beta) break;
    }
    return best;
}

void Search::recordQuietCutoff(const Move& move, int depth, int ply) {
    if(ply >= 0 && ply < killerPlies && !(move == killers[ply][0])) {
        killers[ply][1] = killers[ply][0];
        killers[ply][0] = move;
    }
    int& value = history[board.whiteToPlay ? 1 : 0][move.from][move.to];
    const int boundedDepth = clamp(depth, 1, 32);
    const int bonus = boundedDepth * boundedDepth;
    value += bonus - value * bonus / historyLimit;
    value = min(value, historyLimit);
}

void Search::moveOrder(vector<Move>& moves, const Move& ttMove, int ply) {
    constexpr int ttScore = 100000000;
    constexpr int promotion = 6000000;
    constexpr int pieceValue[6] = {100, 300, 320, 500, 900, 0};
    vector<pair<int, Move>> scored;
    scored.reserve(moves.size());
    for(const Move& move : moves) {
        int total = 0;
        if(move == ttMove) {
            total = ttScore;
        } else {
            if(move.captured != EMPTY) {
                total += 2000000 + pieceValue[pieceType(move.captured)] * 10
                       - pieceValue[pieceType(board.board[move.from])];
            }
            if(move.promotion != EMPTY) total += promotion;
            if(move.captured == EMPTY && move.promotion == EMPTY) {
                if(ply >= 0 && ply < killerPlies && move == killers[ply][0]) total = 1000000;
                else if(ply >= 0 && ply < killerPlies && move == killers[ply][1]) total = 999999;
                else total = history[board.whiteToPlay ? 1 : 0][move.from][move.to];
            }
        }
        scored.emplace_back(total, move);
    }
    sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    for(size_t i = 0; i < moves.size(); ++i) moves[i] = scored[i].second;
}
