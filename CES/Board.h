#pragma once

#include <bitset>
#include <iostream>
#include <vector>

#include "Enums.h"

template <typename const size_t X, const size_t Y>
class Board;

struct FieldPos
{
    size_t x, y;
};

class Turn
{
    friend Board<8, 8>;
    FieldPos from, to;
    Turn(){}
};

template <typename const size_t X, const size_t Y>
class Board
{
public:

    Board()
    {
        //init standart board (russian checkers)

        for (size_t i = 1; i <= 3; ++i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0)
                    setField(i, j, wChecker);

        for (size_t i = 8; i > 5; --i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0)
                    setField(i, j, bChecker);
    }
    
    size_t getField(const size_t x, const size_t y) const
    {
        return getBitsByPos(x, y).to_ulong();
    }

    void setField(const size_t x, const size_t y, const size_t val)
    {
        std::bitset<3> bits(val);
        board[(X* (x - 1) + (y - 1)) / 2*3] = bits[2];
        board[(X * (x - 1) + (y - 1)) / 2*3 + 1] = bits[1];
        board[(X * (x - 1) + (y - 1)) / 2*3 + 2] = bits[0];
    }

    std::vector<Turn> getVariants()
    {

    }

    void doTurn(Turn turn)
    {

    }

private:

    std::bitset<3> getBitsByPos(const size_t x, const size_t y) const
    {
        return std::bitset<3>(board[(X * (x - 1) + (y - 1))/2*3] * 4 + board[(X* (x - 1) + (y - 1))/2*3 + 1] * 2 + board[(X * (x - 1) + (y - 1))/2*3 + 2]);
    }

    std::bitset<X*Y/2*3> board;
};