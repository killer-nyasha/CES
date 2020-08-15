#pragma once

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"
//#include "Board.h"

//struct FieldPos
//{
//    size_t x = 0, y = 0;
//};

namespace LGraphics {

    class LGApp
    {
    public:

        struct TryTurn
        {
            ivect2 to = { 0,0 }, from = {0,0};
        };

        void deleteByPos(size_t x, size_t y);
        void moveToPos(size_t fromX, size_t fromY, size_t toX, size_t toY);
        void moveToPos(LWidgetI* checker, size_t toX, size_t toY);
        void setTick(std::function<void()> func) { app.setTick(func); }
        void loop();

        LGApp();
        LGApp(std::function<void()> tick);

        bool playerTurn = false;
        TryTurn turnTodo;
        LGraphics::LApp app;

    private:

        LWidgetI* prevActiveWidget = nullptr;
        ivect2 getPosByCoords(szvect2 screenSize, GLFWwindow* wnd, fvect3 coords);
        fvect3 getCoordsByPosition(size_t x, size_t y);

        LGraphics::LBaseComponent c;

        LIButton* board;

        int getCheckerNumByPos(size_t x, size_t y);
        void init();
    };

}