#pragma once

#include <bitset>
#include <iostream>

template <typename const size_t X, const size_t Y>
class Board
{

public:

    enum Field
    {
        empty,
        wChecker,
        bChecker,
        wKing,
        bKing,
    };

    Board()
    {
        board.set(0);
        board.set(2);
    }
    
    size_t getField(const size_t x, const size_t y) const
    {
        return getBitsByPos(x, y).to_ullong();
    }

    size_t setField(const size_t x, const size_t y, const size_t val)
    {

    }

private:

    std::bitset<3> getBitsByPos(const size_t x, const size_t y)
    {
        std::bitset<3> res;
        for (size_t i = 0; i < 3; ++i)
            res.set(i, board[X * (x - 1) + 3 * (y - 1) + i]);
        return res;
    }

    std::bitset<X*Y/2*3> board;
};