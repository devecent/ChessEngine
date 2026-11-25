#include "move.h"
#include "board.h"
#include <cstdlib>
#include <string>


std::string Move::toString() const {
    return toUci();
}

Move Move::fromUci(const std::string& uci, Board& board) {

    auto validSquare = [](char file, char rank) {
        return file >= 'a' && file <= 'h' && rank >= '1' && rank <= '8';
    };
    if ((uci.length() != 4 && uci.length() != 5)
        || !validSquare(uci[0], uci[1]) || !validSquare(uci[2], uci[3])) {
        return Move();
    }

    int from = (uci[0] - 'a') + (uci[1] - '1') * 8;
    int to   = (uci[2] - 'a') + (uci[3] - '1') * 8;

    int movingPiece = board.board[from];
    Piece promotion = EMPTY;
    bool castling = false;

    // Detect promotion
    if (uci.length() == 5) {
        char p = uci[4];
        if (board.whiteToPlay) {
            switch (p) {
                case 'q': promotion = WQUEEN; break;
                case 'r': promotion = WROOK;  break;
                case 'b': promotion = WBISHOP;break;
                case 'n': promotion = WKNIGHT;break;
                default: break;
            }
        } else {
            switch (p) {
                case 'q': promotion = BQUEEN; break;
                case 'r': promotion = BROOK;  break;
                case 'b': promotion = BBISHOP;break;
                case 'n': promotion = BKNIGHT;break;
                default: break;
            }
        }
    }

    if (movingPiece == WKING || movingPiece == BKING) {
        if(std::abs(to % 8 - from % 8) == 2) castling = true;
    }
    int captured = board.board[to];
    if (movingPiece == WPAWN || movingPiece == BPAWN) {
        if (captured == EMPTY && (from % 8 != to % 8)) {
            captured = board.whiteToPlay ? BPAWN : WPAWN;
        }
    }
    return Move(from, to, captured, promotion, castling);
}


std::string Move::toUci() const {
    if(isNull()) return "0000";
    std::string uci;
    uci += static_cast<char>('a' + from() % 8);
    uci += static_cast<char>('1' + from() / 8);
    uci += static_cast<char>('a' + to() % 8);
    uci += static_cast<char>('1' + to() / 8);

    switch (promotion()) {
        case WQUEEN: case BQUEEN:   uci += 'q'; break;
        case WROOK:  case BROOK:    uci += 'r'; break;
        case WBISHOP:case BBISHOP:  uci += 'b'; break;
        case WKNIGHT:case BKNIGHT:  uci += 'n'; break;
        default: break;
    }

    return uci;
}
