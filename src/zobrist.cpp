#include "zobrist.h"

using namespace std;



uint64_t Zobrist::castling(int castlingRights) {
    return castlingKeys[castlingRights];
}

uint64_t Zobrist::piece(int piece, int sq) {
    return RANDOM_ARRAY[64 * MAP_HASH_PIECE[piece-1] + sq];
}

uint64_t Zobrist::enpassant(int sq) {
    int file = sq%8;
    return RANDOM_ARRAY[file+772];
}

uint64_t Zobrist::side() {
    return RANDOM_ARRAY[780];
}
