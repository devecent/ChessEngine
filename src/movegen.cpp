#include "movegen.h"
#include "boardHelpers.h"
#include "attacks.h"


void MoveGen::generatePawnMoves(int sq, std::vector<Move>& moves, bool white) {
    int rank = sq/8;
    int file = sq%8;
    int forward = white ? 8 : -8;
    int startRank = white ? 1 : 6;
    int oneForward = sq+forward;
    int twoForward = sq+2*forward;
    int promotionRank = white ? 7 : 0;
    auto& b = board.board;
    if(oneForward >= 0 && oneForward < 64 && !b[oneForward]) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward, EMPTY, WKNIGHT, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WBISHOP, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WROOK, false});
                moves.push_back(Move{sq, oneForward, EMPTY, WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward, EMPTY, BKNIGHT, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BBISHOP, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BROOK, false});
                moves.push_back(Move{sq, oneForward, EMPTY, BQUEEN, false});
            }
        }
        else if(!tacticalOnly) {
            moves.push_back(Move{sq, oneForward, EMPTY, EMPTY, false});
        }
        if(!tacticalOnly && rank == startRank && twoForward >= 0 && twoForward < 64 && !b[twoForward]) {
            moves.push_back(Move{sq, twoForward, EMPTY, EMPTY, false});
        }
    }
    //left diagonal capture
    if(file > 0 && oneForward >= 0 && oneForward < 64 && b[oneForward-1] && isWhite(b[oneForward-1]) != white) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WKNIGHT, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WBISHOP, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WROOK, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BKNIGHT, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BBISHOP, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BROOK, false});
                moves.push_back(Move{sq, oneForward-1, b[oneForward-1], BQUEEN, false});
            }
        }
        else {
            moves.push_back(Move{sq, oneForward-1, b[oneForward-1], EMPTY, false});
        }
    }
    //right diagonal capture
    if(file < 7 && oneForward >= 0 && oneForward < 64 && b[oneForward+1] && isWhite(b[oneForward+1]) != white) {
        if(rank + forward/8 == promotionRank) {
            if(white) {
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WKNIGHT, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WBISHOP, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WROOK, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], WQUEEN, false});
            }
            else {
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BKNIGHT, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BBISHOP, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BROOK, false});
                moves.push_back(Move{sq, oneForward+1, b[oneForward+1], BQUEEN, false});
            }
        }
        else {
            moves.push_back(Move{sq, oneForward+1, b[oneForward+1], EMPTY, false});
        }
    }
    if(board.enpassantSquare != -1 && file < 7 && board.enpassantSquare == sq+1 && isWhite(b[sq+1]) == !white) {
        moves.push_back(Move{sq, board.enpassantSquare+forward, b[board.enpassantSquare], EMPTY, false});
    }
    if(board.enpassantSquare != -1 && file > 0 && board.enpassantSquare == sq-1 && isWhite(b[sq-1]) == !white) {
        moves.push_back(Move{sq, board.enpassantSquare+forward, b[board.enpassantSquare], EMPTY, false});
    }
}

// Slides from sq along SlideDirs[first .. first+3], stopping at the first
// occupied square (captured if it's an enemy piece). Quiet moves are skipped
// when tacticalOnly is set.
static void generateSlides(const int (&b)[64], int sq, int first, std::vector<Move>& moves, bool white, bool tacticalOnly) {
    for(int d = first; d < first + 4; d++) {
        const int offset = attacks::SlideDirs[d].offset;
        int curr = sq;
        for(int n = attacks::StepsToEdge[sq][d]; n > 0; --n) {
            curr += offset;
            if(b[curr]) {
                if(isWhite(b[curr]) != white) {
                    moves.push_back(Move{sq, curr, b[curr], EMPTY, false});
                }
                break;
            }
            if(!tacticalOnly) moves.push_back(Move{sq, curr, EMPTY, EMPTY, false});
        }
    }
}

void MoveGen::generateBishopMoves(int sq, std::vector<Move>& moves, bool white) {
    generateSlides(board.board, sq, attacks::FirstDiagonal, moves, white, tacticalOnly);
}

void MoveGen::generateRookMoves(int sq, std::vector<Move>& moves, bool white) {
    generateSlides(board.board, sq, attacks::FirstOrthogonal, moves, white, tacticalOnly);
}

void MoveGen::generateKingMoves(int sq, std::vector<Move>& moves, bool white) {
    auto& b = board.board;
    const attacks::SquareList& targets = attacks::King[sq];
    for(int i = 0; i < targets.count; i++) {
        const int to = targets.squares[i];
        if(b[to] && isWhite(b[to]) == white) continue;
        if(!b[to] && tacticalOnly) continue;
        moves.push_back(Move{sq, to, b[to], EMPTY, false});
    }
}

void MoveGen::generateQueenMoves(int sq, std::vector<Move>& moves, bool white) {
    generateRookMoves(sq,moves,white);
    generateBishopMoves(sq,moves,white);
}

void MoveGen::generateKnightMoves(int sq, std::vector<Move>& moves, bool white) {
    auto& b = board.board;
    const attacks::SquareList& targets = attacks::Knight[sq];
    for(int i = 0; i < targets.count; i++) {
        const int dest = targets.squares[i];
        if(!b[dest]) {
            if(!tacticalOnly) moves.push_back(Move{sq, dest, EMPTY, EMPTY, false});
        }
        else if(isWhite(b[dest]) != white) {
            moves.push_back(Move{sq, dest, b[dest], EMPTY, false});
        }
    }
}


void MoveGen::generateCastlingMoves(std::vector<Move>& moves, bool white) {
    auto& b = board.board;
    int castling = board.castlingRights;
    if(white) {
        if( (castling & WHITE_KINGSIDE) && b[7] == WROOK && b[5] == EMPTY && b[6] == EMPTY && !board.isSquareAttacked(4,!white) && !board.isSquareAttacked(5,!white) && !board.isSquareAttacked(6,!white)) {
            moves.push_back(Move{4, 6, EMPTY, EMPTY, true});
        }
        if( (castling & WHITE_QUEENSIDE) && b[0] == WROOK  && b[1] == EMPTY && b[2] == EMPTY && b[3] == EMPTY && !board.isSquareAttacked(4,!white) && !board.isSquareAttacked(3,!white) && !board.isSquareAttacked(2,!white)) {
            moves.push_back(Move{4,2,EMPTY, EMPTY,true});
        }
    }
    else {
        if( (castling & BLACK_KINGSIDE) && b[63] == BROOK && b[61] == EMPTY && b[62] == EMPTY && !board.isSquareAttacked(60,!white) && !board.isSquareAttacked(61,!white) && !board.isSquareAttacked(62,!white)) {
            moves.push_back(Move{60 ,62, EMPTY, EMPTY, true});
        }
        if( (castling & BLACK_QUEENSIDE) && b[56] == BROOK && b[57] == EMPTY && b[58] == EMPTY && b[59] == EMPTY && !board.isSquareAttacked(60,!white) && !board.isSquareAttacked(59,!white) && !board.isSquareAttacked(58,!white)) {
            moves.push_back(Move{60, 58, EMPTY, EMPTY, true});
        }
    }
}


std::vector<Move> MoveGen::generatePseudoLegalMoves(bool whiteToMove) {
    std::vector<Move> moves;
    moves.reserve(256);
    auto& pieces = whiteToMove ? board.whitePieces : board.blackPieces;
    for(int sq : pieces[0]) {
        generatePawnMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[1]) {
        generateKnightMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[2]) {
        generateBishopMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[3]) {
        generateRookMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[4]) {
        generateQueenMoves(sq,moves,whiteToMove);
    }
    for(int sq : pieces[5]) {
        generateKingMoves(sq,moves,whiteToMove);
    }
    if(!tacticalOnly) generateCastlingMoves(moves,whiteToMove);
    return moves;
}

MoveGen::LegalityContext MoveGen::legalityContext(bool whiteToMove) {
    LegalityContext ctx;
    ctx.ownKing = whiteToMove ? board.whitePieces[5][0] : board.blackPieces[5][0];
    ctx.inCheck = board.isSquareAttacked(ctx.ownKing, !whiteToMove);
    return ctx;
}

bool MoveGen::isLegal(const Move& move, const LegalityContext& ctx, bool whiteToMove) {
    // Shortcut: when not in check, a non-king, non-en-passant move can only
    // expose the king if the moving piece is pinned, and a pinned piece must
    // share a rank, file or diagonal with its king. Anything else is legal
    // without the make/undo below.
    if(!ctx.inCheck && move.from() != ctx.ownKing && !board.isEnPassant(move)) {
        const int dr = move.from() / 8 - ctx.ownKing / 8, df = move.from() % 8 - ctx.ownKing % 8;
        if(dr != 0 && df != 0 && dr != df && dr != -df) return true;
    }
    board.makeMove(move);
    const int kingSquare = whiteToMove ? board.whitePieces[5][0] : board.blackPieces[5][0];
    const bool legal = !board.isSquareAttacked(kingSquare, !whiteToMove);
    board.undoMove(move);
    return legal;
}

void MoveGen::filterLegal(std::vector<Move>& moves, bool whiteToMove) {
    // Filter in place (stable, keeps generation order).
    const LegalityContext ctx = legalityContext(whiteToMove);
    size_t legal = 0;
    for(const Move move : moves) {
        if(isLegal(move, ctx, whiteToMove)) moves[legal++] = move;
    }
    moves.resize(legal);
}

std::vector<Move> MoveGen::generateMoves(bool whiteToMove) {
    std::vector<Move> moves = generatePseudoLegalMoves(whiteToMove);
    filterLegal(moves, whiteToMove);
    return moves;
}

std::vector<Move> MoveGen::generateTacticalMoves(bool whiteToMove) {
    tacticalOnly = true;
    std::vector<Move> moves = generatePseudoLegalMoves(whiteToMove);
    tacticalOnly = false;
    filterLegal(moves, whiteToMove);
    return moves;
}

bool MoveGen::hasLegalMove(bool whiteToMove) {
    const std::vector<Move> moves = generatePseudoLegalMoves(whiteToMove);
    const LegalityContext ctx = legalityContext(whiteToMove);
    for(const Move& move : moves) {
        if(isLegal(move, ctx, whiteToMove)) return true;
    }
    return false;
}
