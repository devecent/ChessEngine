#include "openingbook.h"
#include <fstream>



static uint64_t readU64(std::ifstream& in) {
    uint64_t x = 0;
    for (int i = 0; i < 8; i++) {
        int c = in.get();
        x = (x << 8) | static_cast<uint8_t>(c);
    }
    return x;
}

static uint16_t readU16(std::ifstream& in) {
    int hi = in.get();
    int lo = in.get();
    return static_cast<uint16_t>((static_cast<uint8_t>(hi) << 8) | static_cast<uint8_t>(lo));
}


OpeningBook::OpeningBook(const std::string& file) {
    load(file);
}

void OpeningBook::load(const std::string& file) {
    std::ifstream in(file, std::ios::binary);
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

uint16_t OpeningBook::getMove(uint64_t key) {
    // find() rather than operator[], which would insert an empty entry for
    // every position looked up that isn't in the book.
    auto it = openingBook.find(key);
    if(it == openingBook.end() || it->second.empty()) {
        return 0;
    }
    const std::vector<uint16_t>& moves = it->second;
    std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}


// very similar to https://bitbucket.org/joeyrobert/ceruleanjs_opening_books/src/master/
// i use the openbooks used in this engine
