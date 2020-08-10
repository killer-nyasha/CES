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
    size_t unit = Field::empty;
    FieldPos from, to;

    bool ate = false;
    bool willBecomeKing_ = false;
    Turn(){}

    Turn(size_t unit, FieldPos from, FieldPos to, bool ate, bool willBecomeKing_)
        :unit(unit),from(from),to(to),ate(ate),willBecomeKing_(willBecomeKing_){}

public:

    bool willBecomeKing() const { return willBecomeKing_; }
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

        prevTurn.unit = bChecker;
    }
    
    size_t getField(const size_t x, const size_t y) const
    {
        return getBitsByPos(x, y).to_ulong();
    }

    void setField(const size_t x, const size_t y, const size_t val)
    {
        std::bitset<3> bits(val);
        (*this)(x,y,1) = bits[2];
        (*this)(x, y, 2) = bits[1];
        (*this)(x, y, 3) = bits[0];
    }

    std::vector<Turn> getVariants() const
    {
        std::vector<Turn> res;

        // если в прошлом ходе шашка съела другую, то нужно проверить
        // не может ли она съесть ещё
        if (prevTurn.ate)
        {
            res = beatTurnVariants(prevTurn.to.x, prevTurn.to.y);
            if (res.size())
                return res;
        }

        else
        {
            // проверяем не может ли шашка цвета ходящего побить кого-то
            // (бой обязательный, поэтому проверяется в первую очередь)
            for (size_t i = 1; i <= 8; ++i)
                for (size_t j = 1; j <= 8; ++j)
                    if ((i + j) % 2 == 0 && isUnit(i, j) && getColor(getField(i, j)) != getColor(prevTurn.unit))
                    {
                        auto turns = beatTurnVariants(i, j);
                        res.insert(res.end(), turns.begin(), turns.end());
                    }
            if (res.size())
                return res;
        }

        for (size_t i = 1; i <= 8; ++i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0 && isUnit(i, j) && getColor(getField(i, j)) != getColor(prevTurn.unit))
                {
                    auto turns = turnVariants(i, j);
                    res.insert(res.end(), turns.begin(), turns.end());
                }
        return res;
    }

    void doTurn(Turn turn)
    {
        prevTurn = turn;
        setField(turn.from.x, turn.from.y, Field::empty);
        if (!turn.willBecomeKing()) setField(turn.to.x, turn.to.y, turn.unit);
        else setField(turn.to.x, turn.to.y, turn.unit + 2);
    }
    
    typename std::bitset<X * Y / 2 * 3>::reference operator()(size_t x, size_t y, size_t bit = 1)
    {
        return board[(X * (x - 1) + (y - 1)) / 2 * 3 + bit - 1];
    }

private:

    std::bitset<3> getBitsByPos(const size_t x, const size_t y) const
    {
        return std::bitset<3>(board[(X * (x - 1) + (y - 1)) / 2 * 3] * 4 + board[(X * (x - 1) + (y - 1)) / 2 * 3 + 1] * 2 + board[(X * (x - 1) + (y - 1)) / 2 * 3 + 2]);
    }

    bool outOfRange(const size_t x, const size_t y) const
    {
        return !(x >= 1 && x <= 8 && y >= 1 && y <= 8);
    }

    bool isUnit(const size_t x, const size_t y) const
    {
        return getField(x, y) != empty;
    }

    bool isChecker(const size_t x, const size_t y) const
    {
        return getField(x, y) == wChecker || getField(x, y) == bChecker;
    }

    bool isKing(const size_t x, const size_t y) const
    {
        return getField(x, y) == wKing || getField(x, y) == bKing;
    }

    bool isWhite(const size_t unit) const
    {
        return unit == wChecker || unit == wKing;
    }

    bool isBlack(const size_t unit) const
    {
        return unit == bChecker || unit == bKing;
    }

    bool getColor(const size_t unit) const
    {
        return !isWhite(unit);
    }

    bool getColor(const size_t x, const size_t y) const
    {
        return !isWhite(getField(x,y));
    }

    bool isEmpty(const size_t x, const size_t y) const
    {
        return getField(x, y) == Field::empty;
    }

    std::vector<Turn> beatTurnVariants(const size_t x, const size_t y) const
    {
        std::vector<Turn> res;
        size_t unit = getField(x, y);
        if (isChecker(x,y))
        {
            if ((!outOfRange(x - 1, y - 1) ? isUnit(x - 1, y - 1) : false() && (!outOfRange(x - 2, y - 2) ? !isUnit(x - 2, y - 2) : false())) && getColor(x-1,y-1)!=getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x - 2,y - 2 }, true, false));
            if ((!outOfRange(x - 1, y + 1) ? isUnit(x - 1, y + 1) : false() && (!outOfRange(x - 2, y + 2) ? !isUnit(x - 2, y + 2) : false())) && getColor(x - 1, y + 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x - 2,y + 2 }, true, false));
            if ((!outOfRange(x + 1, y - 1) ? isUnit(x + 1, y - 1) : false() && (!outOfRange(x + 2, y - 2) ? !isUnit(x + 2, y - 2) : false())) && getColor(x + 1, y - 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x + 2,y - 2 }, true, false));
            if ((!outOfRange(x + 1, y + 1) ? isUnit(x + 1, y + 1) : false() && (!outOfRange(x + 2, y + 2) ? !isUnit(x + 2, y + 2) : false())) && getColor(x + 1, y + 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x + 2,y + 2 }, true, false));
        }

        else if (isKing(x,y))
        {
            // v pizdu...
        }
        return res;
    }

    std::vector<Turn> turnVariants(const size_t x, const size_t y) const
    {
        std::vector<Turn> res;
        size_t unit = getField(x, y);
        if (isChecker(x, y))
        {
            if (!outOfRange(x + 1, y - 1) && isEmpty(x + 1, y - 1))
                res.push_back(Turn(unit, { x,y }, { x + 1,y - 1 }, false, false));
            if (!outOfRange(x + 1, y + 1) && isEmpty(x + 1, y + 1))
                res.push_back(Turn(unit, { x,y }, { x + 1,y + 1 }, false, false));
        }

        else if (isKing(x, y))
        {
            // v pizdu...
        }
        return res;
    }

    template <typename T>
    void push_back(std::vector<T>& to, std::vector<T>& from)
    {
        for (auto o : from)
            to.push_back(from);
    }

    std::bitset<X*Y/2* 3> board;
    Turn prevTurn;
};