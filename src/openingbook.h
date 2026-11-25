
#ifndef OPENINGBOOK_H
#define OPENINGBOOK_H

#include <string>
#include <vector>
#include <cstdint>
#include <random>
#include <unordered_map>


class OpeningBook {
    public:

    explicit OpeningBook(const std::string& file);
    uint16_t getMove(uint64_t key);
    void load(const std::string& file);


    private:

    std::unordered_map<uint64_t, std::vector<uint16_t>> openingBook;
    std::mt19937 rng{std::random_device{}()};

};


#endif
