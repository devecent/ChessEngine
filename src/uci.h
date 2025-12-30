#ifndef UCI_H
#define UCI_h

#include "board.h"
#include "search.h"
#include <string>
#include <cstdint>
#include "openingbook.h"

class UCI {

    public:

    UCI(Board& board, Search& search, OpeningBook& openingBook) : board(board), search(search), openingBook(openingBook){}

    void loop();

    std::string polyglotMoveToUCI(uint16_t move);

    private:

    Board& board;
    Search& search;
    OpeningBook& openingBook;

    void handleUCI();
    void handleIsReady();
    void handleNewGame();
    void handlePosition(const std::string& line);
    void handleGo(const std::string& line);
    
};


#endif