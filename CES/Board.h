#pragma once

#include <bitset>
#include <iostream>
#include <vector>

#include "Enums.h"
#include "Pools.h"

template <typename const size_t X, const size_t Y>
class Board;

struct FieldPos
{
    size_t x = 0, y = 0;
};

class Turn
{
    friend Board<8, 8>;
    size_t unit = Field::empty;
    FieldPos from, to, ate;

    bool willBecomeKing_ = false;
    Turn() {}
    Turn(size_t unit, FieldPos from, FieldPos to, FieldPos ate, bool willBecomeKing_)
        :unit(unit), from(from), to(to), ate(ate), willBecomeKing_(willBecomeKing_) {}

public:

    bool willBecomeKing() const { return willBecomeKing_; }

    friend std::ostream& operator<<(std::ostream& cout, const Turn& t)
    {
        cout << t.from.x << t.from.y << " " << t.to.x << t.to.y;
        return cout;
    }
};

template <typename const size_t X, const size_t Y>
class Board
{
public:

    Board()
    {
        //init standart board (russian checkers)

        wCheckers = 2, bCheckers = 2;
        //for (size_t i = 1; i <= 3; ++i)
            //for (size_t j = 1; j <= 8; ++j)
                //if ((i + j) % 2 == 0)
                    //setField(i, j, wChecker);
        setField(1, 3, wChecker);
        //setField(1, 5, wChecker);
        setField(8, 2, bChecker);
        //setField(8, 4, bChecker);
        //for (size_t i = 8; i > 5; --i)
            //for (size_t j = 1; j <= 8; ++j)
                //if ((i + j) % 2 == 0)
                    //setField(i, j, bChecker);

        prevTurn.unit = bChecker;
    }

    size_t getField(const size_t x, const size_t y) const
    {
        return getBitsByPos(x, y).to_ulong();
    }

    void setField(const size_t x, const size_t y, const size_t val)
    {
        std::bitset<3> bits(val);
        (*this)(x, y, 1) = bits[2];
        (*this)(x, y, 2) = bits[1];
        (*this)(x, y, 3) = bits[0];
    }

    PoolPointer<std::vector<Turn>> getVariants() const
    {
        PoolPointer<std::vector<Turn>> p_res;
        auto& res = *p_res;

        //std::vector<Turn> res;

        // если в прошлом ходе шашка съела другую, то нужно проверить
        // не может ли она съесть ещё
        if (prevTurn.ate.x && prevTurn.ate.y)
        {
            auto best = beatTurnVariants(prevTurn.to.x, prevTurn.to.y);
            if (best->size())
                return std::move(best);
        }

        //else
        {
            // проверяем не может ли шашка цвета ходящего побить кого-то
            // (бой обязательный, поэтому проверяется в первую очередь)
            for (size_t i = 1; i <= 8; ++i)
                for (size_t j = 1; j <= 8; ++j)
                    if ((i + j) % 2 == 0 && isUnit(i, j) && getColor(getField(i, j)) != getColor(prevTurn.unit))
                    {
                        auto turns = beatTurnVariants(i, j);
                        res.insert(res.end(), turns->begin(), turns->end());
                    }
            if (res.size())
                return std::move(p_res);
        }

        for (size_t i = 1; i <= 8; ++i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0 && isUnit(i, j) && getColor(getField(i, j)) != getColor(prevTurn.unit))
                {
                    auto turns = turnVariants(i, j, getColor(i, j));
                    res.insert(res.end(), turns->begin(), turns->end());
                }
        return std::move(p_res);
    }

    void doTurn(Turn turn)
    {
        setField(turn.from.x, turn.from.y, empty);
        if (turn.ate.x != 0 && turn.ate.y != 0) getColor(turn.ate.x, turn.ate.y) == White ? wCheckers-- : bCheckers--, setField(turn.ate.x, turn.ate.y, empty);

        if (wCheckers == 0) win = Black;
        else if (bCheckers == 0) win = White;

        if (!turn.willBecomeKing())
            setField(turn.to.x, turn.to.y, turn.unit);
        else
        {
            setField(turn.to.x, turn.to.y, turn.unit + 2);
            win = getColor(turn.unit);
        }
        prevTurn = turn;
    }

    int won()
    {
        return win;
    }

    typename std::bitset<X * Y / 2 * 3>::reference operator()(size_t x, size_t y, size_t bit = 1)
    {
        return board[(X * (x - 1) + (y - 1)) / 2 * 3 + bit - 1];
    }

    void drawBoard()
    {
        system("cls");
        std::cout << "\n\n\n\n\n\n\n";
        std::cout << "\n                                                    abcdefgh";
        std::cout << "\n                                                    --------";
        for (size_t i = 8; i >= 1; --i)
        {
            std::cout << "\n                                                  ";
            std::cout << i << "|";
            for (size_t j = 1; j <= 8; ++j)
            {
                if ((i + j) % 2 == 0) drawObject(getField(i, j));
                else std::cout << "  ";
            }
        }
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
        return !isWhite(getField(x, y));
    }

    bool isEmpty(const size_t x, const size_t y) const
    {
        return getField(x, y) == empty;
    }

    bool willBecomeKing(const size_t x, const size_t y, const size_t unit) const
    {
        return (isBlack(unit) && x == 1) || (isWhite(unit) && x == 8);
    }

    void drawObject(const size_t object) const
    {
        if (object == empty) std::cout << "  ";
        else std::cout << object << " ";
    }


    PoolPointer<std::vector<Turn>> beatTurnVariants(const size_t x, const size_t y) const
    {
        PoolPointer<std::vector<Turn>> p_res;
        auto& res = *p_res;

        size_t unit = getField(x, y);
        if (isChecker(x, y))
        {
            if (!outOfRange(x - 1, y - 1) && !outOfRange(x - 2, y - 2) && isUnit(x - 1, y - 1) && isEmpty(x - 2, y - 2) && getColor(x - 1, y - 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x - 2,y - 2 }, { x - 1,y - 1 }, willBecomeKing(x - 2, y - 2, unit)));

            if (!outOfRange(x - 1, y + 1) && !outOfRange(x - 2, y + 2) && isUnit(x - 1, y + 1) && isEmpty(x - 2, y + 2) && getColor(x - 1, y + 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x - 2,y + 2 }, { x - 1 ,y + 1 }, willBecomeKing(x - 2, y + 2, unit)));

            if (!outOfRange(x + 1, y - 1) && !outOfRange(x + 2, y - 2) && isUnit(x + 1, y - 1) && isEmpty(x + 2, y - 2) && getColor(x + 1, y - 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x + 2,y - 2 }, { x + 1, y - 1 }, willBecomeKing(x + 2, y - 2, unit)));

            if (!outOfRange(x + 1, y + 1) && !outOfRange(x + 2, y + 2) && isUnit(x + 1, y + 1) && isEmpty(x + 2, y + 2) && getColor(x + 1, y + 1) != getColor(unit))
                res.push_back(Turn(unit, { x,y }, { x + 2,y + 2 }, { x + 1, y + 1 }, willBecomeKing(x + 2, y + 2, unit)));
        }

        else if (isKing(x, y))
        {
            // v pizdu...
        }
        return std::move(p_res);
    }

    PoolPointer<std::vector<Turn>> turnVariants(const size_t x, const size_t y, bool black) const
    {
        int isBlack = black ? -1 : 1;

        PoolPointer<std::vector<Turn>> p_res;
        auto& res = *p_res;

        size_t unit = getField(x, y);
        if (isChecker(x, y))
        {
            if (!outOfRange(x + isBlack, y - 1) && isEmpty(x + isBlack, y - 1))
                res.push_back(Turn(unit, { x,y }, { x + isBlack,y - 1 }, { 0,0 }, willBecomeKing(x + isBlack, y - 1, unit)));
            if (!outOfRange(x + isBlack, y + 1) && isEmpty(x + isBlack, y + 1))
                res.push_back(Turn(unit, { x,y }, { x + isBlack,y + 1 }, { 0,0 }, willBecomeKing(x + isBlack, y + 1, unit)));
        }

        else if (isKing(x, y))
        {
            // v pizdu...
        }
        return p_res;
    }

    template <typename T>
    void push_back(std::vector<T>& to, std::vector<T>& from)
    {
        for (auto o : from)
            to.push_back(from);
    }

    std::bitset<X * Y / 2 * 3> board;
    Turn prevTurn;
    int win = -1;

    int wCheckers = 0, bCheckers = 0, wKing = 0, bKing = 0;
};