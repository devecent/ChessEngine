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
    Move bestMove = search.findBestMove(search.thinkTime(wtime, btime, winc, binc));
    std::cout << "bestmove " << bestMove.toUci() << endl;
}