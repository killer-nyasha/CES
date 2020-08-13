#pragma once

#include "LApp.h"
#include "LIRectangle.h"
#include "LIButton.h"
#include "LBaseComponent.h"

namespace LGraphics {

    class LGApp
    {
    public:

        void deleteByPos(size_t x, size_t y);
        void moveToPos(size_t fromX, size_t fromY, size_t toX, size_t toY);
        void moveToPos(LWidgetI* checker, size_t toX, size_t toY);
        void setTick(std::function<void()> func) { app.setTick(func); }
        void loop();
        LGApp();
        LGApp(std::function<void()> tick);

    private:

        LWidgetI* prevActiveWidget = nullptr;
        ivect2 getPosByCoords(szvect2 screenSize, GLFWwindow* wnd, fvect3 coords);
        fvect3 getCoordsByPosition(size_t x, size_t y);

        LGraphics::LApp app;
        LGraphics::LBaseComponent c;

        LIButton* board;

        int getCheckerNumByPos(size_t x, size_t y);
        void init();
    };

}