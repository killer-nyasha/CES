#pragma once

#include <bitset>
#include <iostream>
#include <vector>

#include "Enums.h"
#include "Pools.h"

#include "LGApp.h"
#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"
#include "LTextEdit.h"
#include "additional.h"
#include "LGApp.h"
#include <cmath>

template <typename const size_t X, const size_t Y>
class Board;

class Turn
{
    friend Board<8, 8>;
    friend LGraphics::LGApp;

    size_t unit = Field::empty;
    bool willBecomeKing_ = false;
    Turn(size_t unit, szvect2 from, szvect2 to, szvect2 ate, bool willBecomeKing_)
        :unit(unit), from(from), to(to), ate(ate), willBecomeKing_(willBecomeKing_) {}

public:

    szvect2 from = { 0, 0 }, to = { 0, 0 }, ate = { 0, 0 };
    Turn() {}
    bool willBecomeKing() const { return willBecomeKing_; }
};

template <typename const size_t X, const size_t Y>
class Board
{
public:

    Board(LGraphics::LGApp* app)
    {
        //init standart board (russian checkers)
        
        wCheckers = 12, bCheckers = 12;
        for (size_t i = 1; i <= 3; ++i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0)
                    initChecker(i, j, wChecker);

        for (size_t i = 8; i > 5; --i)
            for (size_t j = 1; j <= 8; ++j)
                if ((i + j) % 2 == 0)
                    initChecker(i, j, bChecker);

        prevTurn.unit = bChecker;
    }

    void initChecker(const size_t x, const size_t y, const int checker)
    {
        setField(x, y, checker);
        drawUnit(x, y, getColor(checker));
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

    void drawUnit(const size_t x, const size_t y, const int color)
    {
        app.app.getOpenGlDrawing().lock();
        app.addChecker(x, y, color);
        app.app.getOpenGlDrawing().unlock();
    }

    PoolPointer<std::vector<Turn>> getVariants()
    {
        PoolPointer<std::vector<Turn>> p_res;
        auto& res = *p_res;

        //std::vector<Turn> res;

        // если в прошлом ходе шашка съела другую, то нужно проверить
        // не может ли она съесть ещё
        if (prevTurn.ate.x && prevTurn.ate.y)
        {
            //currentTurn = getColor(prevTurn.unit);
            auto best = beatTurnVariants(prevTurn.to.x, prevTurn.to.y);
            if (best->size())
            {
                currentTurn = getColor(prevTurn.unit);
                return std::move(best);
            }
        }

        //else
            //currentTurn = currentTurn == White ? Black : White;

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

    void doTurn(Turn turn, LGraphics::LGApp* app)
    {
        doTurn(turn);
        setField(turn.from.x, turn.from.y, empty);
        if (turn.ate.x != 0 && turn.ate.y != 0)
            app->deleteByPos(turn.ate.x, turn.ate.y);

        if (!turn.willBecomeKing())
            app->moveToPos(turn.from.x, turn.from.y, turn.to.x, turn.to.y);
        else
            app->moveToPos(turn.from.x, turn.from.y, turn.to.x, turn.to.y);
        app->app.setActiveWidget(nullptr);
            // нужно поменять текстуру на дамку
    }

    void doTurn(Turn turn)
    {
        setField(turn.from.x, turn.from.y, empty);


        if (turn.ate.x != 0 && turn.ate.y != 0)
        {
            getColor(turn.ate.x, turn.ate.y) == White ? wCheckers-- : bCheckers--;
            setField(turn.ate.x, turn.ate.y, empty);
        }

        if (wCheckers == 0) win = Black;
        else if (bCheckers == 0) win = White;

        if (!turn.willBecomeKing())
            setField(turn.to.x, turn.to.y, turn.unit);

        else
        {
            setField(turn.to.x, turn.to.y, turn.unit + 2);
            win = getColor(turn.unit);
        }
        switchTurn();
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

    //void drawBoard()
    //{
    //    system("cls");
    //    std::cout << "\n\n\n\n\n\n\n";
    //    std::cout << "\n                                                    a b c d e f g h";
    //    std::cout << "\n                                                    ---------------";
    //    for (size_t i = 8; i >= 1; --i)
    //    {
    //        std::cout << "\n                                                  ";
    //        std::cout << i << "|";
    //        for (size_t j = 1; j <= 8; ++j)
    //        {
    //            if ((i + j) % 2 == 0) drawObject(getField(i, j));
    //            else std::cout << "  ";
    //        }
    //        std::cout << "|" << i;
    //    }
    //    std::cout << "\n                                                    ---------------";
    //    std::cout << "\n                                                    a b c d e f g h";
    //}

    short boardAnalize()
    {
        short res = 0;

        res += (wCheckers - bCheckers)*checkerValue;

        if (getColor(4, 6) == Black) res -= powerPosValue;
        else if (getColor(4, 6) == White) res += powerPosValue;

        if (getColor(5, 3) == Black) res -= powerPosValue;
        else if (getColor(5, 3) == White) res += powerPosValue;

        if (getColor(1, 1) == Black) res += weakPosValue;
        else if (getColor(1, 1) == White) res -= weakPosValue;

        if (getColor(3, 1) == Black) res += weakPosValue;
        else if (getColor(3, 1) == White) res -= weakPosValue;

        if (getColor(5, 1) == Black) res += weakPosValue;
        else if (getColor(5, 1) == White) res -= weakPosValue;

        if (getColor(7, 1) == Black) res += weakPosValue;
        else if (getColor(7, 1) == White) res -= weakPosValue;

        if (getColor(2, 8) == Black) res += weakPosValue;
        else if (getColor(2, 8) == White) res -= weakPosValue;

        if (getColor(4, 8) == Black) res += weakPosValue;
        else if (getColor(4, 8) == White) res -= weakPosValue;

        if (getColor(6, 8) == Black) res += weakPosValue;
        else if (getColor(6, 8) == White) res -= weakPosValue;

        if (getColor(8, 8) == Black) res += weakPosValue;
        else if (getColor(8, 8) == White) res -= weakPosValue;

        //if (getColor(1, 1) == color) res += weakPosValue;
        //if (getColor(3, 1) == color) res += weakPosValue;
        //if (getColor(5, 1) == color) res += weakPosValue;
        //if (getColor(7, 1) == color) res += weakPosValue;
        //if (getColor(2, 8) == color) res += weakPosValue;
        //if (getColor(4, 8) == color) res += weakPosValue;
        //if (getColor(6, 8) == color) res += weakPosValue;
        //if (getColor(8, 8) == color) res += weakPosValue;
        return res;
        //playerAnalize(
        //etalon = bCheckers * checkerValue + wCheckers * checkerValue + 2 * powerPosValue - 8 * weakPosValue;
        //return playerAnalize(White) - playerAnalize(Black);
    }

    bool whoseTurn() const
    {
        return currentTurn;
    }

private:

    static const short checkerValue;
    static const short powerPosValue;
    static const short weakPosValue;


    void initDrawing()
    {

    }

    float playerAnalize(bool color)
    {
        //float res = 0;
        //color == White ? res += (wCheckers - bCheckers)*checkerValue : res += (bCheckers - wCheckers);
        ////float res = color == Black ? (bCheckers - wCheckers)* checkerValue : wCheckers * checkerValue;
        //if (getColor(4, 6) == color) res += powerPosValue;
        //if (getColor(5, 3) == color) res += powerPosValue;

        //if (getColor(1, 1) == color) res += weakPosValue;
        //if (getColor(3, 1) == color) res += weakPosValue;
        //if (getColor(5, 1) == color) res += weakPosValue;
        //if (getColor(7, 1) == color) res += weakPosValue;
        //if (getColor(2, 8) == color) res += weakPosValue;
        //if (getColor(4, 8) == color) res += weakPosValue;
        //if (getColor(6, 8) == color) res += weakPosValue;
        //if (getColor(8, 8) == color) res += weakPosValue;
        //return res;
    }

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

    void switchTurn()
    {
        currentTurn = currentTurn == Black ? White : Black;
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

    size_t wCheckers = 0, bCheckers = 0, wKing = 0, bKing = 0;

    bool currentTurn = White;
};