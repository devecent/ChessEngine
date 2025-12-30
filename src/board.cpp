#include "board.h"
#include "movegen.h"
#include "zobrist.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;



Board::Board() {
    for(int i = 0; i < 64; i++) {
        board[i] = EMPTY;
    }
    int backRank[8] = { WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WBISHOP, WKNIGHT, WROOK };
    for(int i = 0; i < 8; i++) {
        board[i] = backRank[i];
        board[i+8] = WPAWN;
        board[i+48] = BPAWN;
        board[i+56] = backRank[i]+6;
    }
    
    for(int i = 0; i < 64; i++) {
        int p = board[i];
        if(p == EMPTY) continue;
        if(p <= WKING) {
            squareIndex[i] = whitePieces[p-1].size();
            whitePieces[p-1].push_back(i);
        }
        else {
            squareIndex[i] = blackPieces[p-7].size();
            blackPieces[p-7].push_back(i);
        }
    }
    history.reserve(256);
    castlingRights = 0b1111;
    whiteToPlay = true;
    zobristKey = 0x463b96181691fc9c;
    enpassantSquare = -1;
}

inline bool isWhite(int piece) {
    return piece >= WPAWN && piece <= WKING;
}

inline bool isPawn(int p) {
    return p == WPAWN || p == BPAWN;
}
inline bool isKnight(int p) {
    return p == WKNIGHT || p == BKNIGHT;
}
inline bool isBishop(int p) {
    return p == WBISHOP || p == BBISHOP;
}
inline bool isRook(int p) {
    return p == WROOK || p == BROOK;
}
inline bool isQueen(int p) {
    return p == WQUEEN || p == BQUEEN;
}
inline bool isKing(int p) {
    return p == WKING || p == BKING;
}

inline int charToPiece(char c) {
    switch(c) {
        case 'P': return WPAWN;
        case 'N': return WKNIGHT;
        case 'B': return WBISHOP;
        case 'R': return WROOK;
        case 'Q': return WQUEEN;
        case 'K': return WKING;
        case 'p': return BPAWN;
        case 'n': return BKNIGHT;
        case 'b': return BBISHOP;
        case 'r': return BROOK;
        case 'q': return BQUEEN;
        case 'k': return BKING;
        default : return EMPTY;
    }
}

inline int squareStringToIndex(const std::string& sq) {
    if(sq == "-") return -1;
    int file = sq[0] - 'a';
    int rank = sq[1] - '1';
    return rank * 8 + file;
}

void Board::updateCastlingRights(const Move& move, int movingPiece) {
    int from = move.from;
    int to   = move.to;

    if (movingPiece == WKING)
        castlingRights &= ~(WHITE_KINGSIDE | WHITE_QUEENSIDE);
    if (movingPiece == BKING)
        castlingRights &= ~(BLACK_KINGSIDE | BLACK_QUEENSIDE);

    if (from == 0 || to == 0)   castlingRights &= ~WHITE_QUEENSIDE;
    if (from == 7 || to == 7)   castlingRights &= ~WHITE_KINGSIDE;
    if (from == 56 || to == 56) castlingRights &= ~BLACK_QUEENSIDE;
    if (from == 63 || to == 63) castlingRights &= ~BLACK_KINGSIDE;
}


void Board::updateEnPassantSquare(const Move& move) {
    int to = move.to;
    int from = move.from;
    int p = board[to];
    enpassantSquare = -1;
    if(isPawn(p)) {
        if(abs(from-to) == 16) {
            enpassantSquare = to;
        }
    }
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

void Board::removeFromPieceList(int sq, int piece) {
    auto& pieces = isWhite(piece) ? 
                 whitePieces[pieceType(piece)] : 
                 blackPieces[pieceType(piece)];
    int idx = squareIndex[sq];
    int lastSq = pieces.back();
    pieces[idx] = lastSq;
    squareIndex[lastSq] = idx;
    squareIndex[sq] = -1;
    pieces.pop_back();
}

void Board::addToPieceList(int sq, int piece) {
    auto& pieces = isWhite(piece) ? 
                 whitePieces[pieceType(piece)] : 
                 blackPieces[pieceType(piece)];
    squareIndex[sq] = pieces.size();
    pieces.push_back(sq);
}

void Board::movePieceInList(int piece, int from, int to) {
    int idx = squareIndex[from];
    auto& pieces = isWhite(piece) ? 
                 whitePieces[pieceType(piece)] : 
                 blackPieces[pieceType(piece)];
    squareIndex[from] = -1;
    squareIndex[to] = idx;
    pieces[idx] = to;
}

bool Board::isEnPassant(const Move& m) const {
    return (m.captured != EMPTY &&
            board[m.from] == (whiteToPlay ? WPAWN : BPAWN) &&
            m.to != m.from + (whiteToPlay ? 8 : -8) &&
            board[m.to] == EMPTY);
}


void Board::makeMove(const Move& move) {
    int from = move.from;
    int to   = move.to;
    int movingPiece = board[from];
    int capturedSq = (isEnPassant(move)) ? enpassantSquare : to;
    history.push_back(Undo{
        move.captured,
        capturedSq,
        enpassantSquare,
        castlingRights,
        zobristKey
    });
    //I highly suspect there's some sort of bug with zobristKey,
    //but I do not know and can't find it if there is
    if(enPassantCapturable()) {
        zobristKey ^= Zobrist::enpassant(enpassantSquare);
    }
    zobristKey ^= Zobrist::side();
    zobristKey ^= Zobrist::castling(castlingRights);
    zobristKey ^= Zobrist::piece(movingPiece,from);
    if(move.captured != EMPTY) {
        zobristKey ^= Zobrist::piece(move.captured, capturedSq);
        removeFromPieceList(capturedSq, move.captured);
        board[capturedSq] = EMPTY;
    }
    
    movePieceInList(movingPiece, from, to);

    
    board[to] = movingPiece;
    board[from] = EMPTY;
    zobristKey ^= Zobrist::piece(board[to],to);

    if (move.castling) {
        int rookFrom, rookTo;
        if (to == 6)      { rookFrom = 7;  rookTo = 5;  }
        else if (to == 2) { rookFrom = 0;  rookTo = 3;  }
        else if (to == 62){ rookFrom = 63; rookTo = 61; }
        else              { rookFrom = 56; rookTo = 59; }

        int rook = board[rookFrom];
        zobristKey ^= Zobrist::piece(rook, rookFrom);

        movePieceInList(rook, rookFrom, rookTo);
        board[rookFrom] = EMPTY;
        board[rookTo] = rook;

        zobristKey ^= Zobrist::piece(rook, rookTo);
    }

    if(move.promotion != EMPTY) {
        removeFromPieceList(to, board[to]);
        zobristKey ^= Zobrist::piece(board[to],to);
        board[to] = move.promotion;
        zobristKey ^= Zobrist::piece(board[to],to);
        addToPieceList(to, move.promotion);
    }

    updateCastlingRights(move, movingPiece);
    zobristKey ^= Zobrist::castling(castlingRights);
    updateEnPassantSquare(move);
    whiteToPlay = !whiteToPlay;
    if(enPassantCapturable()) {
        zobristKey ^= Zobrist::enpassant(enpassantSquare);
    }
}



void Board::undoMove(const Move& move) {
    whiteToPlay = !whiteToPlay;

    int from = move.from;
    int to  = move.to;

    int movingPiece = (move.promotion != EMPTY) ? move.promotion : board[to];

    if(move.promotion != EMPTY) {
        removeFromPieceList(to, move.promotion);
        board[to] = movingPiece = (whiteToPlay ? WPAWN : BPAWN);
        addToPieceList(to, movingPiece);
    }

    movePieceInList(movingPiece, to, from);
    board[from] = movingPiece;
    board[to] = EMPTY;

    if(move.captured != EMPTY) {
        int capturedSq = history.back().capturedSquare;
        board[capturedSq] = move.captured;
        addToPieceList(capturedSq, move.captured);
    }

    if(move.castling) {
        if(to == 6) { movePieceInList(WROOK, 5, 7); board[7] = WROOK; board[5] = EMPTY; }
        else if(to == 2) { movePieceInList(WROOK, 3, 0); board[0] = WROOK; board[3] = EMPTY; }
        else if(to == 62) { movePieceInList(BROOK, 61, 63); board[63] = BROOK; board[61] = EMPTY; }
        else if(to == 58) { movePieceInList(BROOK, 59, 56); board[56] = BROOK; board[59] = EMPTY; }
    }

    enpassantSquare = history.back().prevEnpassant;
    castlingRights  = history.back().prevCastle;
    zobristKey = history.back().zobrist;
    history.pop_back();
}



struct Dir {
    int offset;
    int fileDelta;
};

static constexpr Dir BishopDirs[4] = {
    {  9,  1 },
    {  7, -1 },
    { -7,  1 },
    { -9, -1 }  
};

static constexpr Dir RookDirs[4] = {
    {  8,  0 },
    { -8,  0 },
    {  1,  1 },
    { -1, -1 } 
};

static constexpr int KnightMoves[8][2] = {
    { 2,  1 }, { 2, -1 },
    { 1,  2 }, { 1, -2 },
    {-1,  2 }, {-1, -2 },
    {-2,  1 }, {-2, -1 }
};

static constexpr int KingMoves[8][2] = {
    { 1,  0 }, {-1,  0 },
    { 0,  1 }, { 0, -1 },
    { 1,  1 }, { 1, -1 },
    {-1,  1 }, {-1, -1 }
};


bool Board::isSquareAttacked(int sq, bool byWhite) {
    auto& b = board;
    int r = sq / 8;
    int f = sq % 8;

    int pawnDir = byWhite ? -1 : 1; 
    int attackRank = r + pawnDir;
    if (attackRank >= 0 && attackRank <= 7) {
        if (f > 0) {
            int p = b[attackRank * 8 + (f - 1)];
            if (isPawn(p) && isWhite(p) == byWhite) return true;
        }
        if (f < 7) {
            int p = b[attackRank * 8 + (f + 1)];
            if (isPawn(p) && isWhite(p) == byWhite) return true;
        }
    }

    for (auto& m : KnightMoves) {
        int nr = r + m[0];
        int nf = f + m[1];
        if (nr < 0 || nr > 7 || nf < 0 || nf > 7) continue;
        int p = b[nr * 8 + nf];
        if (isKnight(p) && isWhite(p) == byWhite) return true;
    }

    for (auto& d : BishopDirs) {
        int nf = f;
        int sqi = sq;
        while (true) {
            nf += d.fileDelta;
            sqi += d.offset;
            if (nf < 0 || nf > 7 || sqi < 0 || sqi > 63) break;
            int p = b[sqi];
            if (p) {
                if (isWhite(p) == byWhite && (isBishop(p) || isQueen(p))) return true;
                break; // blocked
            }
        }
    }

    for (auto& d : RookDirs) {
        int nf = f;
        int sqi = sq;
        while (true) {
            nf += d.fileDelta;
            sqi += d.offset;
            if (nf < 0 || nf > 7 || sqi < 0 || sqi > 63) break;
            int p = b[sqi];
            if (p) {
                if (isWhite(p) == byWhite && (isRook(p) || isQueen(p))) return true;
                break; // blocked
            }
        }
    }
    for (auto& m : KingMoves) {
        int nr = r + m[0];
        int nf = f + m[1];
        if (nr < 0 || nr > 7 || nf < 0 || nf > 7) continue;
        int p = b[nr * 8 + nf];
        if (isKing(p) && isWhite(p) == byWhite) return true;
    }

    return false;
}


uint64_t Board::perft(int depth) {
    if(depth == 0) return 1;
    MoveGen movegen(*this);
    uint64_t total = 0;
    
    vector<Move> moves = movegen.generateMoves(whiteToPlay);
    for(const Move& move : moves) {
        makeMove(move);
        total += perft(depth-1);
        undoMove(move);
    } 
    return total;
}

uint64_t Board::perftDivide(int depth) {
    MoveGen movegen(*this);
    vector<Move> moves = movegen.generateMoves(whiteToPlay);

    uint64_t totalNodes = 0;

    for(const Move &move : moves) {
        makeMove(move);
        uint64_t nodes = perft(depth - 1);
        undoMove(move);

        totalNodes += nodes;

        std::cout << move.toString() << ": " << nodes << "\n";
    }

    std::cout << "Total nodes: " << totalNodes << "\n";
    return totalNodes;
}

void Board::loadFEN(const string& fen) {
    fill(board,board+64,EMPTY);
    for(int i = 0; i < 6; i++) {
        whitePieces[i].clear();
        blackPieces[i].clear();
    }
    fill(squareIndex, squareIndex+64, -1);
    castlingRights = 0;
    enpassantSquare = -1;
    istringstream iss(fen);
    string boardPart, activeColor, castling, enpassant;
    int halfmove, fullmove;
    iss >> boardPart >> activeColor >> castling >> enpassant >> halfmove >> fullmove;
    int sq = 56; 
    for(char c : boardPart) {
        if(c == '/') {
            sq -= 16;
            continue;
        }
        if(isdigit(c)) {
            sq += c - '0';
        } else {
            int piece = charToPiece(c);
            board[sq] = piece;
            addToPieceList(sq, piece);
            sq++;
        }
    }
    whiteToPlay = (activeColor == "w");
    for(char c : castling) {
        if(c == 'K') {
            castlingRights |= WHITE_KINGSIDE; 
        }
        else if(c == 'Q') {
            castlingRights |= WHITE_QUEENSIDE;
        }
        else if(c == 'k') {
            castlingRights |= BLACK_KINGSIDE;
        }
        else if(c == 'q') {
            castlingRights |= BLACK_QUEENSIDE;
        }
    }
    if(enpassant != "-") {
        int ep = squareStringToIndex(enpassant) + (whiteToPlay ? -8 : 8);
        enpassantSquare = ep;
    }
    else {
        enpassantSquare = -1;
    }
    zobristKey = computeZobristKey();
}

bool Board::isCheckmate() {
    MoveGen movegen(*this);
    int kingsq = whiteToPlay ? whitePieces[5][0] : blackPieces[5][0];
    return isSquareAttacked(kingsq,!whiteToPlay) && movegen.generateMoves(whiteToPlay).empty();
}

bool Board::isStalemate() {
    MoveGen movegen(*this);
    int kingsq = whiteToPlay ? whitePieces[5][0] : blackPieces[5][0];
    return !isSquareAttacked(kingsq,!whiteToPlay) && movegen.generateMoves(whiteToPlay).empty();
}

uint64_t Board::computeZobristKey() {
    uint64_t hash = 0;
    for(int i = 0; i < 64; i++) {
        if(board[i] != 0) {
            hash ^= Zobrist::piece(board[i],i);
        }
    }
    if(enPassantCapturable()) {
        hash ^= Zobrist::enpassant(enpassantSquare);
    }
    if(whiteToPlay) {
        hash ^= Zobrist::side();
    }
    hash ^= Zobrist::castling(castlingRights);
    return hash;
}

bool Board::enPassantCapturable() {
    if (enpassantSquare == -1) return false;

    int file = enpassantSquare % 8;
    int p = whiteToPlay ? WPAWN : BPAWN;
    if (file > 0 && board[enpassantSquare-1] == p) {
       return true;
    }
    if (file < 7 && board[enpassantSquare+1] == p) {
        return true;
    }
    return false;
}



void Board::print() {
    static string pieceChar = ".PNBRQKpnbrqk";
    cout << "\n a b c d e f g h\n";
    for(int r = 7; r >= 0; r--) {
        cout << r+1 << ' ';
        for(int f = 0; f < 8; f++) {
            cout << pieceChar[board[r*8+f]] << ' ';
        }
        cout << r+1 << '\n';
    }
    cout << " a b c d e f g h\n";
}