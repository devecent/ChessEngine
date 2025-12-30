
#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>


class OpeningBook {
    public:

    OpeningBook(std::string file);
    uint16_t getMove(uint64_t key);
    void load(std::string file);
    

    private:

    std::unordered_map<uint64_t, std::vector<uint16_t>> openingBook;
   
};


#endif