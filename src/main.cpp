#include "movegen.h"
#include "board.h"
#include "search.h"
#include "uci.h"
#include <iostream>


using namespace std;

int main() {
    Board board;
    Search search(board);
    UCI uci(board, search);

    uci.loop();
    return 0;
}