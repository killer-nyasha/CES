#include "LGApp.h"
#include "LApp.h"
#include "additional.h"
#include "Enums.h"
#include <cmath>

namespace LGraphics
{
    ivect2 LGApp::getPosByCoords(szvect2 screenSize, GLFWwindow* wnd, fvect3 coords)
    {
        //auto mouseGl = pointOnScreenToGlCoords(fvect2(screenSize), { coords.x ,coords.y });
        auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
        auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
        dist.y -= 0.172f;
        return { (int)floor((firstPosition.x + dist.x / 2 - coords.x) / dist.x) + 1 ,(int)floor((firstPosition.y + dist.y / 2 - coords.y) / dist.y) + 1 };
    }

    fvect3 LGApp::getCoordsByPosition(size_t x, size_t y)
    {
        auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
        auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
        dist.y -= 0.172f;
        return { firstPosition.x - (y - 1)* dist.x, firstPosition.y - (x - 1)*dist.y, firstPosition.z - dist.z };
    }

    void LGApp::addChecker(size_t x, size_t y, int color)
    {
        LIButton* checker;
        if (color == Black) checker = new LGraphics::LIButton(&app,"black.png");
        else if (color == White)checker = new LGraphics::LIButton(&app, "white.png");
        else throw std::exception();

        auto animFunf = [&]()
        {
            if (!playerTurn)
            {
                app.getActiveWidget()->breakAnimation();
                return;
            }
            static bool fading = true;
            if (app.getActiveWidget()->getTransparency() <= 0.25f)
                fading = false;
            else if (app.getActiveWidget()->getTransparency() >= 0.85f)
                fading = true;
            if (fading)
                app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
            else if (!fading)
                app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
        };

        auto clickEventf = [&]() { prevActiveWidget = app.getActiveWidget(); };
        auto breakAnimf = [&]() {app.getActiveWidget()->transparency(1.0f); };

        auto initObj = [&](LIButton* w, size_t i, size_t j)
        {
            auto coords = getCoordsByPosition(i, j);
            w->scale(0.09f, 0.16f, 1.0f);
            w->turnOffColor();
            w->setClickEventFunction(clickEventf);
            w->setAnimation(animFunf);
            w->setBreakingAnimation(breakAnimf);
            moveToPos(w, i, j);
        };

        initObj(checker, x, y);
    }

    void LGApp::deleteByPos(size_t x, size_t y)
    {
        auto obj = app.getObjects();
        auto i = getCheckerNumByPos(x, y);
        prevActiveWidget = nullptr;
        app.setActiveWidget(nullptr);
        delete (*obj)[i];
        (*obj).erase((*obj).begin() + i);
    }

    void LGApp::loop()
    {
        app.loop();
    }

    void LGApp::moveToPos(size_t fromX, size_t fromY, size_t toX, size_t toY)
    {
        auto obj = app.getObjects();
        auto i = getCheckerNumByPos(fromX,fromY);
        LWidget* checker = i != -1 ? (*obj)[i] : nullptr;
        moveToPos(checker, toX, toY);
    }

    void LGApp::moveToPos(LWidget* checker, size_t toX, size_t toY)
    {
        checker->move(getCoordsByPosition(toX, toY));
    }

    int LGApp::getCheckerNumByPos(size_t x, size_t y)
    {
        auto obj = app.getObjects();
        for (size_t i = 0; i < obj->size(); ++i)
            if (std::abs((*obj)[i]->getMove().x - getCoordsByPosition(x, y).x) < 0.01 &&
                std::abs((*obj)[i]->getMove().y - getCoordsByPosition(x, y).y) < 0.01 &&
                (*obj)[i]->getScale() != fvect3{ 1.2f, 1.7f, 1.0f })
                return i;
        return -1;
    }

    void LGApp::init()
    {
        board = new LGraphics::LIButton(&app, "d64_01.png");

        board->setClickEventFunction([&]()
        {
            if (!playerTurn) return;
            if (prevActiveWidget && prevActiveWidget != board)
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(app.getWindowHandler(), &mouse_x, &mouse_y);
                auto pos = getPosByCoords(app.getWindowSize(), app.getWindowHandler(), pointOnScreenToGlCoords(app.getWindowSize(),{ (float)mouse_x, (float)mouse_y, 1.0f }));
                if (pos.x >= 1 && pos.x <= 8 && pos.y >= 1 && pos.y <= 8 && (pos.x + pos.y) % 2 == 0)
                {
                    auto coordsFrom = getPosByCoords(app.getWindowSize(), app.getWindowHandler(), prevActiveWidget->getMove());
                    turnTodo.from.x = coordsFrom.y, turnTodo.from.y = coordsFrom.x;
                    turnTodo.to.x = pos.y, turnTodo.to.y = pos.x;
                    app.setActiveWidget(prevActiveWidget);
                }
            }
        }
        );
        board->scale(1.2f, 1.7f, 1.0f);
        board->turnOffColor();

        /*const size_t checkersCount = 12;
        LIButton* white[checkersCount];
        for (size_t i = 0; i < checkersCount; ++i)
            white[i] = new LGraphics::LIButton(&app, nullptr, "white.png", &c);

        LIButton* black[checkersCount];
        for (size_t i = 0; i < checkersCount; ++i)
            black[i] = new LGraphics::LIButton(&app, nullptr, "black.png", &c);

        size_t counter = 0;*/

        //auto animFunf = [&]()
        //{
        //    if (!playerTurn)
        //    {
        //        app.getActiveWidget()->breakAnimation();
        //        return;
        //    }
        //    static bool fading = true;
        //    if (app.getActiveWidget()->getTransparency() <= 0.25f)
        //        fading = false;
        //    else if (app.getActiveWidget()->getTransparency() >= 0.85f)
        //        fading = true;
        //    if (fading)
        //        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
        //    else if (!fading)
        //        app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
        //};

        //auto clickEventf = [&]() { prevActiveWidget = app.getActiveWidget(); };
        //auto breakAnimf = [&]() {app.getActiveWidget()->transparency(1.0f);};

        //auto initObj = [&](LIButton* w,size_t i, size_t j)
        //{
        //    auto coords = getCoordsByPosition(i, j);
        //    w->scale(0.09f, 0.16f, 1.0f);
        //    w->turnOffColor();
        //    w->setClickEventFunction(clickEventf);
        //    w->setAnimation(animFunf);
        //    w->setBreakingAnimation(breakAnimf);
        //    moveToPos(w, i, j);
        //    ++counter;
        //};

        //for (size_t i = 1; i <= 3; ++i)
        //    for (size_t j = 1; j <= 8; ++j)
        //        if ((i + j) % 2 == 0)
        //            initObj(white[counter], i, j);
        //counter = 0;
        //for (size_t i = 8; i >= 6; i--)
        //    for (size_t j = 1; j <= 8; ++j)
        //        if ((i + j) % 2 == 0)
        //            initObj(black[counter], i, j);
    }

    LGApp::LGApp()
    {
        init();
    }

    LGApp::LGApp(std::function<void()> tick)
    {
        init();
    }
}