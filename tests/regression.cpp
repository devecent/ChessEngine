#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "board.h"
#include "boardHelpers.h"
#include "movegen.h"
#include "piecesquaretable.h"
#define private public
#include "search.h"
#include "evaluation.h"
#include "uci.h"
#undef private

static int checks = 0;
void check(bool condition, const char* message) {
    ++checks;
    if(!condition) throw std::runtime_error(message);
}

bool samePosition(const Board& a, const Board& b) {
    return std::equal(a.board, a.board + 64, b.board)
        && a.whiteToPlay == b.whiteToPlay && a.castlingRights == b.castlingRights
        && a.enpassantSquare == b.enpassantSquare && a.fiftyMoveCounter == b.fiftyMoveCounter
        && a.zobristKey == b.zobristKey && a.history.size() == b.history.size()
        && a.repetitionHistory == b.repetitionHistory;
}

void consistent(Board& b) {
    check(b.zobristKey == b.computeZobristKey(), "incremental hash mismatch");
    int count = 0;
    for(bool white : {false, true}) {
        auto& lists = white ? b.whitePieces : b.blackPieces;
        for(int type = 0; type < 6; ++type) {
            for(size_t i = 0; i < lists[type].size(); ++i) {
                int sq = lists[type][i];
                check(b.squareIndex[sq] == static_cast<int>(i), "piece-list index mismatch");
                check(pieceType(b.board[sq]) == type && isWhite(b.board[sq]) == white, "piece-list mismatch");
                ++count;
            }
        }
    }
    check(count == std::count_if(b.board, b.board + 64, [](int p) { return p != EMPTY; }), "piece count mismatch");
}

void roundTrips(Board& b, int depth) {
    consistent(b);
    if(depth == 0) return;
    Board before = b;
    auto moves = MoveGen(b).generateMoves(b.whiteToPlay);
    check(samePosition(before, b), "legal generation changed position/history");
    for(const auto& m : moves) {
        b.makeMove(m);
        roundTrips(b, depth - 1);
        b.undoMove(m);
        check(samePosition(before, b), "make/undo changed position/history");
        consistent(b);
    }
}

int main() {
    try {
        Board b;
        check(readfromTable(KingStartTable, 6, true) == 30,
              "white g1 king table orientation");
        check(readfromTable(KingStartTable, 62, false) == 30,
              "black g8 king table orientation");
        check(readfromTable(KingStartTable, 6, true) > readfromTable(KingStartTable, 4, true)
              && readfromTable(KingStartTable, 62, false) > readfromTable(KingStartTable, 60, false),
              "middlegame king table does not favor kingside castling squares");
        check(readfromTable(PawnTable, 48, true) == 50
              && readfromTable(PawnTable, 8, false) == 50,
              "seventh-rank pawn table orientation");
        check(readfromTable(PawnsEndTable, 48, true) > readfromTable(PawnsEndTable, 8, true)
              && readfromTable(PawnsEndTable, 8, false) > readfromTable(PawnsEndTable, 48, false),
              "endgame table does not reward pawn advancement");
        for(const auto* table : {&PawnTable, &PawnsEndTable, &KnightsTable, &BishopsTable,
                                 &RooksTable, &QueensTable, &KingStartTable, &KingEndTable}) {
            for(int sq = 0; sq < 64; ++sq) {
                check(readfromTable(*table, sq, true) == readfromTable(*table, sq ^ 56, false),
                      "piece-square tables disagree for mirrored colors");
            }
        }
        check(Evaluation(b).evaluate() == 0, "starting position evaluation is asymmetric");
        roundTrips(b, 2);
        check(b.perft(4) == 197281, "startpos perft(4)");
        b.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        roundTrips(b, 1);
        check(b.perft(3) == 97862, "kiwipete perft(3)");
        b.loadFEN("4k3/P7/8/3pP3/8/8/8/4K3 w - d6 0 1");
        roundTrips(b, 2);

        b = Board();
        for(const char* move : {"g1f3", "g8f6", "f3g1", "f6g8"}) b.makeMove(Move::fromUci(move, b));
        check(b.isRepeated(), "knight cycle repetition missed");
        Board repeated = b;
        MoveGen(b).generateMoves(b.whiteToPlay);
        check(samePosition(b, repeated) && b.isRepeated(), "generation erased repetition");
        auto pawn = Move::fromUci("e2e4", b);
        b.makeMove(pawn);
        check(!b.isRepeated(), "irreversible boundary ignored");
        b.undoMove(pawn);
        check(samePosition(b, repeated), "pawn undo erased repetition");

        b.loadFEN("7k/8/8/8/8/8/8/K7 w - - 99 1");
        check(b.fiftyMoveCounter == 99 && b.history.empty() && b.repetitionHistory.size() == 1, "FEN state reset");
        TranspositionTable tt(b);
        Search search(b, tt);
        OpeningBook book("missing-book.bin");
        UCI uci(b, search, book);
        uci.handlePosition("position fen 7k/8/8/8/8/8/8/K7 w - - 0 1 moves a1b1 h8g8");
        check(b.board[1] == WKING && b.board[62] == BKING && b.whiteToPlay, "FEN move suffix skipped");

        b.loadFEN("7k/6Q1/5K2/8/8/8/8/8 b - - 100 1");
        check(search.negamax(0, -MATE_SCORE, MATE_SCORE, 1) == -MATE_SCORE + 1, "horizon mate missed");
        check(search.findBestMove(0).toUci() == "0000", "terminal null move encoding");
        b.loadFEN("7k/5Q2/5K2/8/8/8/8/8 b - - 0 1");
        check(search.negamax(0, -MATE_SCORE, MATE_SCORE, 1) == 0, "horizon stalemate missed");

        for(const char* fen : {
            "7k/8/8/8/8/8/q7/R6K w - - 0 1",
            "7k/P7/8/8/8/8/8/7K w - - 0 1",
            "7k/8/8/3pP3/8/8/8/7K w - d6 0 1"
        }) {
            b.loadFEN(fen);
            const Board before = b;
            const int staticScore = Evaluation(b).evaluate();
            check(search.negamax(0, -MATE_SCORE, MATE_SCORE, 0) > staticScore,
                  "quiescence missed capture, quiet promotion, or en passant");
            check(samePosition(before, b), "quiescence changed board/history");
            consistent(b);
        }

        // The only reply to this rook check is the quiet king move a1b1.
        b.loadFEN("r7/8/8/8/8/2k5/8/K7 w - - 0 1");
        const Board checkedPosition = b;
        auto evasions = MoveGen(b).generateMoves(b.whiteToPlay);
        check(evasions.size() == 1 && evasions.front().toUci() == "a1b1", "quiet evasion fixture");
        b.makeMove(evasions.front());
        const int evasionScore = -search.quiescence(-MATE_SCORE, MATE_SCORE, 1);
        b.undoMove(evasions.front());
        check(search.quiescence(-MATE_SCORE, MATE_SCORE, 0) == evasionScore,
              "quiescence did not search quiet check evasion");
        check(search.quiescence(-MATE_SCORE, -MATE_SCORE + 100, 0) == evasionScore,
              "quiescence stood pat while in check");
        check(samePosition(checkedPosition, b), "check evasion changed board/history");

        b.loadFEN("7k/8/8/8/8/8/q7/R6K w - - 100 1");
        check(search.quiescence(-MATE_SCORE, MATE_SCORE, 0) == 0, "quiescence ignored fifty-move draw");

        b = Board();
        const Board root = b;
        auto legal = MoveGen(b).generateMoves(b.whiteToPlay);
        auto best = search.findBestMove(0);
        check(std::find(legal.begin(), legal.end(), best) != legal.end(), "zero-budget fallback illegal");
        auto start = std::chrono::steady_clock::now();
        best = search.findBestMove(30);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        check(elapsed < 1000, "recursive deadline not respected");
        check(std::find(legal.begin(), legal.end(), best) != legal.end(), "timed result illegal");
        check(samePosition(root, b), "timeout changed root board/history");
        consistent(b);

        b = Board();
        search.killers = {};
        search.history = {};
        const Move quietA = Move::fromUci("e2e4", b);
        const Move quietB = Move::fromUci("d2d4", b);
        const Move quietC = Move::fromUci("g1f3", b);
        search.recordQuietCutoff(quietA, 3, 4);
        search.recordQuietCutoff(quietB, 2, 4);
        search.recordQuietCutoff(quietB, 2, 4);
        check(search.killers[4][0] == quietB && search.killers[4][1] == quietA,
              "killer slots did not retain two distinct moves");
        std::vector<Move> quietOrder{quietC, quietA, quietB};
        search.moveOrder(quietOrder, Move(), 4);
        check(quietOrder[0] == quietB && quietOrder[1] == quietA, "killer ordering failed");
        search.moveOrder(quietOrder, quietC, 4);
        check(quietOrder[0] == quietC, "TT move did not outrank killers");
        search.moveOrder(quietOrder, Move(), 5);
        check(quietOrder[0] == quietA && quietOrder[1] == quietB, "history ordering failed");
        check(search.history[0][quietA.from][quietA.to] == 0, "history leaked across colors");
        for(int i = 0; i < 10000; ++i) search.recordQuietCutoff(quietA, 40, 200);
        check(search.history[1][quietA.from][quietA.to] <= Search::historyLimit,
              "history scores exceeded bound");
        search.killers = {};
        search.history = {};
        const Board beforeCutoff = b;
        search.negamax(1, -MATE_SCORE, -MATE_SCORE + 100, 3);
        check(search.killers[3][0].from >= 0, "quiet beta cutoff did not train killer");
        const Move trained = search.killers[3][0];
        check(search.history[1][trained.from][trained.to] > 0,
              "quiet beta cutoff did not train history");
        check(samePosition(beforeCutoff, b), "heuristic training changed position");

        b = Board();
        check(search.canNullMove(3, -10000, 1), "null pruning disabled in ordinary middlegame");
        const Board nullRoot = b;
        check(search.negamax(3, -10001, -10000, 1) == -10000,
              "null fail-high did not return beta bound");
        check(samePosition(nullRoot, b), "null cutoff changed board/history");
        check(!search.canNullMove(2, -10000, 1) && !search.canNullMove(3, -10000, 0)
              && !search.canNullMove(3, MATE_SCORE - 1, 1), "null pruning depth/root/mate guards");
        b.loadFEN("7k/8/8/8/8/8/P7/K7 w - - 0 1");
        check(!search.canNullMove(4, -10000, 1), "null pruning allowed pawn ending");
        b.loadFEN("r7/8/8/8/8/2k5/8/K6R w - - 0 1");
        check(!search.canNullMove(4, -10000, 1), "null pruning allowed passing in check");
        b.loadFEN("7k/8/8/3pP3/8/8/8/K6R w - d6 17 1");
        const Board beforeNull = b;
        Board passed = b;
        passed.whiteToPlay = false;
        passed.enpassantSquare = -1;
        passed.zobristKey = passed.computeZobristKey();
        passed.repetitionHistory.assign(1, passed.zobristKey);
        b = passed;
        const int expectedNull = -search.quiescence(-MATE_SCORE, MATE_SCORE, 2);
        b = beforeNull;
        check(search.searchNullMove(0, -MATE_SCORE, MATE_SCORE, 1) == expectedNull,
              "null search did not pass side or expire en passant");
        check(samePosition(beforeNull, b), "null search changed board/history");
        consistent(b);
        search.deadline = std::chrono::steady_clock::now();
        bool nullTimedOut = false;
        try { search.searchNullMove(2, -10000, -9999, 1); }
        catch(...) { nullTimedOut = true; }
        search.deadline = std::chrono::steady_clock::time_point::max();
        check(nullTimedOut && samePosition(beforeNull, b), "null timeout did not restore board/history");
        b.fiftyMoveCounter = 90;
        check(!search.canNullMove(4, -10000, 1), "null pruning near fifty-move draw");

        // Compare root alpha sharing against full-window scores at fixed depth.
        const int savedMaxDepth = search.maxDepth;
        search.maxDepth = 2;
        for(const char* fen : {
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "7k/8/8/8/8/8/q7/R6K w - - 0 1",
            "r6k/Q7/8/8/8/8/8/7K b - - 0 1"
        }) {
            b.loadFEN(fen);
            const Board before = b;
            auto rootMoves = MoveGen(b).generateMoves(b.whiteToPlay);
            int fullWindowBest = -MATE_SCORE;
            for(const auto& move : rootMoves) {
                b.makeMove(move);
                const int value = -search.negamax(1, -MATE_SCORE, MATE_SCORE, 1);
                b.undoMove(move);
                fullWindowBest = std::max(fullWindowBest, value);
            }
            const Move selected = search.findBestMove(5000);
            check(std::find(rootMoves.begin(), rootMoves.end(), selected) != rootMoves.end(),
                  "root alpha sharing selected illegal move");
            b.makeMove(selected);
            const int selectedScore = -search.negamax(1, -MATE_SCORE, MATE_SCORE, 1);
            b.undoMove(selected);
            check(selectedScore == fullWindowBest, "root alpha sharing changed best score");
            check(samePosition(before, b), "fixed-depth root search changed board/history");
        }
        search.maxDepth = savedMaxDepth;

        Move m = legal.front(), out;
        int score = 0;
        tt.push(123, 5, -MATE_SCORE, MATE_SCORE, MATE_SCORE - 8, 3, m);
        check(tt.lookUp(123, 5, -MATE_SCORE, MATE_SCORE, score, out, 7) && score == MATE_SCORE - 12, "winning mate distance normalization");
        tt.push(124, 5, -MATE_SCORE, MATE_SCORE, -MATE_SCORE + 8, 3, m);
        check(tt.lookUp(124, 5, -MATE_SCORE, MATE_SCORE, score, out, 7) && score == -MATE_SCORE + 12, "losing mate distance normalization");
        tt.push(125, 5, -MATE_SCORE, MATE_SCORE - 6, MATE_SCORE - 8, 10, m);
        check(tt.lookUp(125, 5, -MATE_SCORE, MATE_SCORE, score, out, 10), "TT bound classified after normalization");
        out = Move();
        check(!tt.lookUp(123, 6, -MATE_SCORE, MATE_SCORE, score, out) && out == m, "shallow TT ordering move missing");

        b.loadFEN("7k/8/8/8/8/8/8/KQ6 w - - 0 1");
        check(Evaluation(b).kingActivity(true) > 0, "endgame activity disabled");
        for(const char* fen : {"7k/8/8/8/8/8/p7/R6K b - - 0 1", "r6k/8/8/8/8/8/8/Q6K w - - 0 1", "7k/8/8/8/8/8/r7/K7 w - - 0 1"}) {
            b.loadFEN(fen);
            auto moves = MoveGen(b).generateMoves(b.whiteToPlay);
            Move none;
            search.moveOrder(moves, none);
            consistent(b);
        }

        std::istringstream input("uci\nisready\n");
        std::ostringstream output;
        auto* oldIn = std::cin.rdbuf(input.rdbuf());
        auto* oldOut = std::cout.rdbuf(output.rdbuf());
        uci.loop();
        std::cin.rdbuf(oldIn);
        std::cin.clear();
        std::cout.rdbuf(oldOut);
        check(output.str().find("uciok") != std::string::npos && output.str().find("readyok") != std::string::npos, "UCI EOF/handshake");
        std::cout << "PASS: " << checks << " checks; perft, history, FEN/UCI, terminal scores, quiescence, deadlines (" << elapsed << " ms), TT, evaluation, ordering\n";
    } catch(const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
