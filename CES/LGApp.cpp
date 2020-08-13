#include "LGApp.h"
#include "LApp.h"
#include "additional.h"
#include <cmath>

namespace LGraphics
{
    ivect2 LGApp::getPosByCoords(szvect2 screenSize, GLFWwindow* wnd, fvect3 coords)
    {
        auto mouseGl = pointOnScreenToGlCoords(fvect2(screenSize), { coords.x ,coords.y });
        auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
        auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
        dist.y -= 0.172f;
        return { (int)floor((firstPosition.x + dist.x / 2 - mouseGl.x) / dist.x) + 1 ,(int)floor((firstPosition.y + dist.y / 2 - mouseGl.y) / dist.y) + 1 };
    }

    fvect3 LGApp::getCoordsByPosition(size_t x, size_t y)
    {
        auto firstPosition = fvect3{ board->getBottomLeftCorner().x + 0.185f, board->getBottomLeftCorner().y + 0.26f, board->getBottomLeftCorner().z };
        auto dist = (board->getBottomLeftCorner() - board->getBottomRightCorner()) / 10.0f;
        dist.y -= 0.172f;
        return { firstPosition.x - (y - 1)* dist.x, firstPosition.y - (x - 1)*dist.y, firstPosition.z - dist.z };
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
        LWidgetI* checker = i != -1 ? (*obj)[i] : nullptr;
        moveToPos(checker, toX, toY);
    }

    void LGApp::moveToPos(LWidgetI* checker, size_t toX, size_t toY)
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
        board = new LGraphics::LIButton(&app, nullptr, "d64_01.png", &c);

        board->setClickEventFunction([&]()
        {
            if (prevActiveWidget && prevActiveWidget != board)
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(app.getWindowHandler(), &mouse_x, &mouse_y);
                auto pos = getPosByCoords(app.getWindowSize(), app.getWindowHandler(), { (float)mouse_x, (float)mouse_y, 1.0f });
                if (pos.x >= 1 && pos.x <= 8 && pos.y >= 1 && pos.y <= 8 && (pos.x + pos.y) % 2 == 0)
                {
                    moveToPos(prevActiveWidget, pos.y, pos.x);
                    app.setActiveWidget(prevActiveWidget);
                }
            }
        }
        );
        board->scale(1.2f, 1.7f, 1.0f);
        board->turnOffColor();

        const size_t checkersCount = 12;
        LIButton* white[checkersCount];
        for (size_t i = 0; i < checkersCount; ++i)
            white[i] = new LGraphics::LIButton(&app, nullptr, "white.png", &c);

        LIButton* black[checkersCount];
        for (size_t i = 0; i < checkersCount; ++i)
            black[i] = new LGraphics::LIButton(&app, nullptr, "black.png", &c);

        size_t counter = 0;

        for (size_t i = 1; i <= 3; ++i)
        {
            for (size_t j = 1; j <= 8; ++j)
            {
                if ((i + j) % 2 == 0)
                {
                    auto coords = getCoordsByPosition(i, j);
                    white[counter]->scale(0.09f, 0.16f, 1.0f);
                    white[counter]->turnOffColor();

                    white[counter]->setClickEventFunction(
                        [&]()
                    {
                        prevActiveWidget = app.getActiveWidget();
                    });

                    white[counter]->setAnimation([&]()
                    {
                        static bool fading = true;
                        if (app.getActiveWidget()->getTransparency() <= 0.25f)
                            fading = false;
                        else if (app.getActiveWidget()->getTransparency() >= 0.85f)
                            fading = true;
                        if (fading)
                            app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
                        else if (!fading)
                            app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
                    });

                    white[counter]->setBreakingAnimation([&]()
                    {
                        app.getActiveWidget()->transparency(1.0f);
                    });

                    moveToPos(white[counter], i, j);
                    ++counter;
                }
            }
        }

        counter = 0;
        for (size_t i = 8; i >= 6; i--)
        {
            for (size_t j = 1; j <= 8; ++j)
            {
                if ((i + j) % 2 == 0)
                {
                    auto coords = getCoordsByPosition(i, j);
                    black[counter]->scale(0.09f, 0.16f, 1.0f);
                    black[counter]->turnOffColor();
                    black[counter]->setClickEventFunction(
                        [&]()
                    {
                        prevActiveWidget = app.getActiveWidget();
                    });

                    black[counter]->setAnimation([&]()
                    {
                        static bool fading = true;
                        if (app.getActiveWidget()->getTransparency() <= 0.25f)
                            fading = false;
                        else if (app.getActiveWidget()->getTransparency() >= 0.85f)
                            fading = true;
                        if (fading)
                            app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() - 0.015f);
                        else if (!fading)
                            app.getActiveWidget()->transparency(app.getActiveWidget()->getTransparency() + 0.015f);
                    });

                    black[counter]->setBreakingAnimation([&]()
                    {
                        app.getActiveWidget()->transparency(1.0f);
                    });

                    moveToPos(black[counter], i, j);
                    ++counter;
                }
            }
        }
    }

    LGApp::LGApp()
    {
        init();
    }

    LGApp::LGApp(std::function<void()> tick)
    {
        app.setTick(tick);
        init();
    }
}