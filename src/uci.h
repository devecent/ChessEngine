#ifndef UCI_H
#define UCI_h

#include "board.h"
#include "search.h"
#include <string>

class UCI {

    public:

    UCI(Board& board, Search& search) : board(board), search(search){}

    void loop();

    private:

    Board& board;
    Search& search;

    void handleUCI();
    void handleIsReady();
    void handleNewGame();
    void handlePosition(const std::string& line);
    void handleGo(const std::string& line);
};


#endif