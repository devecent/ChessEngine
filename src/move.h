#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <string>
#include "types.h"

class Board;

// A move packed into 32 bits:
//   bits  0-5   from square
//   bits  6-11  to square
//   bits 12-14  promotion piece type (0 none, 1 knight, 2 bishop, 3 rook, 4 queen)
//   bit  15     castling
//   bits 16-19  captured piece (a Piece value, EMPTY if none)
// The low 16 bits identify the move (see key()); the captured piece is extra
// information carried along for make/undo and move ordering.
// A default-constructed Move is the null move: from == to == 0 is never legal.
class Move {

    public:

    constexpr Move() = default;
    constexpr Move(int from, int to, int captured, Piece promotion, bool castling)
        : data(static_cast<uint32_t>(from)
             | static_cast<uint32_t>(to) << 6
             | promotionType(promotion) << 12
             | (castling ? 1u : 0u) << 15
             | static_cast<uint32_t>(captured) << 16) {}

    constexpr int from() const { return data & 0x3F; }
    constexpr int to() const { return (data >> 6) & 0x3F; }
    constexpr int captured() const { return (data >> 16) & 0xF; }
    constexpr bool castling() const { return (data >> 15) & 1; }
    constexpr bool isCapture() const { return captured() != EMPTY; }
    constexpr bool isPromotion() const { return ((data >> 12) & 0x7) != 0; }
    constexpr bool isQuiet() const { return !isCapture() && !isPromotion(); }
    constexpr bool isNull() const { return key() == 0; }

    // Promotions always land on the last rank, so the colour comes from the
    // destination square.
    constexpr Piece promotion() const {
        const uint32_t type = (data >> 12) & 0x7;
        if(type == 0) return EMPTY;
        return static_cast<Piece>(type + (to() >= 56 ? WPAWN : BPAWN));
    }

    // The 16 bits that identify the move (everything except the captured piece).
    constexpr uint16_t key() const { return static_cast<uint16_t>(data); }
    static constexpr Move fromKey(uint16_t key) { Move m; m.data = key; return m; }

    // Moves compare equal when they're the same move, regardless of what the
    // captured-piece bits say (a TT move doesn't carry them).
    constexpr bool operator==(const Move& other) const { return key() == other.key(); }

    std::string toString() const;
    std::string toUci() const;
    static Move fromUci(const std::string& uci, Board& board);

    private:

    // Piece -> promotion type: knight 1, bishop 2, rook 3, queen 4, else 0.
    static constexpr uint32_t promotionType(Piece p) {
        if(p == EMPTY) return 0;
        return static_cast<uint32_t>(p <= WKING ? p - WPAWN : p - BPAWN);
    }

    uint32_t data = 0;
};

static_assert(sizeof(Move) == 4, "Move should stay 4 bytes");

#endif
