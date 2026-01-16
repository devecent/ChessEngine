#include "uci.h"
#include <iostream>
#include <sstream>
#include <vector>
#include "move.h"

using namespace std;

void UCI::loop() {
    string line;
    while(true) {
        getline(cin,line);
        istringstream iss(line);
        string token;
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
    cout << "id name Devecent" << endl;
    cout << "id author Deven Nguyen" << endl;
    cout << "uciok" << endl;
}

void UCI::handleIsReady() {
    cout << "readyok" << endl;
}

void UCI::handleNewGame() {
    board = Board();
}


void UCI::handlePosition(const std::string& line) {
    istringstream iss(line);
    string token;
    iss >> token;

    iss >> token;
    if (token == "startpos") {
        board = Board();
    } else if (token == "fen") {
        string fen;
        getline(iss, fen);
        if(fen[0] == ' ') {
            fen.erase(0,1);
        }
        board.loadFEN(fen);
    }
    // apply moves if present
    while (iss >> token && token != "moves") {}
    while (iss >> token) {
        Move move = Move::fromUci(token, board);
        board.makeMove(move);
    }
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
    int openingMove = openingBook.getMove(board.zobristKey);
    if(openingMove != 0) {
        cout << "bestmove " << polyglotMoveToUCI(openingMove) << endl;
    }
    else {
        Move bestMove = search.findBestMove(search.thinkTime(wtime, btime, winc, binc));
        cout << "bestmove " << bestMove.toUci() << endl;
    }
}

string UCI::polyglotMoveToUCI(uint16_t move) {
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

    string uci;
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