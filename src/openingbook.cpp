#include "openingbook.h"
#include <fstream>
#include <iostream>
#include <random>

using namespace std;


static uint64_t readU64(ifstream& in) {
    uint64_t x = 0;
    for (int i = 0; i < 8; i++) {
        int c = in.get();
        x = (x << 8) | c;
    }
    return x;
}

static uint16_t readU16(ifstream& in) {
    int hi = in.get();
    int lo = in.get();
    return (hi << 8) | lo;
}


OpeningBook::OpeningBook(string file) {
    load(file);
}

void OpeningBook::load(string file) {
    ifstream in(file, ios::binary);
    if(!in) return;
    while(true) {
        uint64_t key = readU64(in);
        if(!in) break;
        uint16_t move = readU16(in);
        if(!in) break;
        in.ignore(6);
        openingBook[key].push_back(move);
    }
}

mt19937 rng(std::random_device{}());

uint16_t OpeningBook::getMove(uint64_t key) {
    vector<uint16_t>& moves = openingBook[key];
    if(moves.empty()) {
        return 0;
    }
    uniform_int_distribution<int> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}


// very similar to https://bitbucket.org/joeyrobert/ceruleanjs_opening_books/src/master/
// i use the openbooks used in this engine