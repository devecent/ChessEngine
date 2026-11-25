#include "uci.h"
#include <iostream>
#include <sstream>
#include <vector>
#include "move.h"
#include "movegen.h"


void UCI::loop() {
    std::string line;
    while(std::getline(std::cin,line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "uci") handleUCI();
        else if (token == "isready") handleIsReady();
        else if (token == "ucinewgame") handleNewGame();
        else if (token == "position") handlePosition(line);
        else if (token == "go") handleGo(line);
        else if (token == "quit") break;
    }
}

void UCI::handleUCI() {
    std::cout << "id name Devecent" << std::endl;
    std::cout << "id author Deven Nguyen" << std::endl;
    std::cout << "uciok" << std::endl;
}

void UCI::handleIsReady() {
    std::cout << "readyok" << std::endl;
}

void UCI::handleNewGame() {
    board = Board();
}


void UCI::handlePosition(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    iss >> token;
    if (token == "startpos") {
        board = Board();
    } else if (token == "fen") {
        std::string fen;
        for(int i = 0; i < 6; ++i) {
            std::string field;
            if(!(iss >> field)) return;
            if(i != 0) fen += ' ';
            fen += field;
        }
        board.loadFEN(fen);
    }
    // apply moves if present; stop at the first one that isn't legal here
    // rather than letting a malformed or illegal move corrupt the board
    while (iss >> token && token != "moves") {}
    while (iss >> token) {
        Move move;
        if (!findLegalMove(token, move)) {
            std::cout << "info string ignoring illegal move " << token << std::endl;
            break;
        }
        board.makeMove(move);
    }
}

bool UCI::findLegalMove(const std::string& uci, Move& out) {
    for (const Move& move : MoveGen(board).generateMoves(board.whiteToPlay)) {
        if (move.toUci() == uci) {
            out = move;
            return true;
        }
    }
    return false;
}

void UCI::handleGo(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
     int wtime = 0, btime = 0, winc = 0, binc = 0, movestogo = 40;
    while (iss >> token) {
        if(token == "wtime") {
            iss >> wtime;
        }
        else if(token == "btime") {
            iss >> btime;
        }
        else if(token == "winc") {
            iss >> winc;
        }
        else if(token == "binc") {
            iss >> binc;
        }
        else if(token == "movestogo") {
            iss >> movestogo;
        }
    }
    uint16_t openingMove = openingBook.getMove(board.zobristKey);
    Move bookMove;
    // a hash collision or bad book entry could give an illegal move, so only
    // play the book move if it's legal in this position
    if(openingMove != 0 && findLegalMove(polyglotMoveToUCI(openingMove), bookMove)) {
        std::cout << "bestmove " << bookMove.toUci() << std::endl;
    }
    else {
        Move bestMove = search.findBestMove(search.thinkTime(wtime, btime, winc, binc));
        std::cout << "bestmove " << bestMove.toUci() << std::endl;
    }
}

std::string UCI::polyglotMoveToUCI(uint16_t move) {
     int toFile = move  & 0x7;
    int toRank  = (move >> 3)  & 0x7;
    int fromFile = (move >> 6)  & 0x7;
    int fromRank = (move >> 9)  & 0x7;
    int promo = (move >> 12) & 0x7;
    //polyglot for some reason encodes as king to rook and not king to the square its going
    if (fromRank == 0 && fromFile == 4) {
        if (toFile == 0) toFile = 2;
        else if (toFile == 7) toFile = 6;
    }
    if (fromRank == 7 && fromFile == 4) {
        if (toFile == 0) toFile = 2;
        else if (toFile == 7) toFile = 6;
    }

    char fromFileChar = 'a' + fromFile;
    char fromRankChar = '1' + fromRank;
    char toFileChar   = 'a' + toFile;
    char toRankChar   = '1' + toRank;

    std::string uci;
    uci += fromFileChar;
    uci += fromRankChar;
    uci += toFileChar;
    uci += toRankChar;

    if (promo) {
        char promoChar = 'q'; // default
        switch (promo) {
            case 1: promoChar = 'n'; break;
            case 2: promoChar = 'b'; break;
            case 3: promoChar = 'r'; break;
            case 4: promoChar = 'q'; break;
        }
        uci += promoChar;
    }

    return uci;
}