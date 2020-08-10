#pragma once

#include <bitset>

template <typename size_t X, size_t Y>
class Board
{
public:

    Board();

private:

    std::bitset<X*Y/2*3> board;
};