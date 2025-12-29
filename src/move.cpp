#include "move.h"
#include "board.h"


using namespace std;


std::string Move::toString() const {
        auto squareToString = [](int sq) -> std::string {
            char file = 'a' + (sq % 8);
            char rank = '1' + (sq / 8);
            return std::string() + file + rank;
        };

        std::string str = squareToString(from) + squareToString(to);

        if(promotion != EMPTY) {
            char promoChar = '?';
            switch(promotion) {
                case WKNIGHT: case BKNIGHT: promoChar = 'n'; break;
                case WBISHOP: case BBISHOP: promoChar = 'b'; break;
                case WROOK:   case BROOK:   promoChar = 'r'; break;
                case WQUEEN:  case BQUEEN:  promoChar = 'q'; break;
                default: promoChar = '?'; 
            }
            str += promoChar;
        }

        return str;
    }

   Move Move::fromUci(const std::string& uci, Board& board) {

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
        if (abs(to - from) == 2) castling = true;
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
    int fromFile = from % 8;
    int fromRank = from / 8;
    int toFile   = to % 8;
    int toRank   = to / 8;

    std::string uci;
    uci += ('a' + fromFile);
    uci += ('1' + fromRank);
    uci += ('a' + toFile);
    uci += ('1' + toRank);

    if (promotion != EMPTY) {
        switch (promotion) {
            case WQUEEN: case BQUEEN:   uci += 'q'; break;
            case WROOK:  case BROOK:    uci += 'r'; break;
            case WBISHOP:case BBISHOP:  uci += 'b'; break;
            case WKNIGHT:case BKNIGHT:  uci += 'n'; break;
            default: break;
        }
    }

    return uci;
}