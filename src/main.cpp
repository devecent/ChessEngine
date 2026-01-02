#include "movegen.h"
#include "board.h"
#include "search.h"
#include "uci.h"
#include <iostream>
#include <assert.h>


using namespace std;

int main() {
    Board board;
    TranspositionTable transpositionTable(board);
    Search search(board, transpositionTable);
    OpeningBook openingBook("./openingbook/gm2001.bin");
    openingBook.load("./openingbook/komodo.bin");
    openingBook.load("./openingbook/rodent.bin");
    UCI uci(board, search, openingBook);
    uci.loop();
    return 0;
}