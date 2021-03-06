﻿#pragma once

#include <tuple>
#include "vectors.h"

namespace LGraphics
{
    bool isPointInPolygon(int npol, float* xp, float* yp, fvect2 p);
    fvect2 pointOnScreenToGlCoords(fvect2 screenSize, fvect2 point);
    fvect2 glCoordsToScreenCoords(fvect2 screenSize, fvect2 val);

    float xOnScreenToGlCoord(fvect2 screenSize, float x);
    float yOnScreenToGlCoord(fvect2 screenSize, float y);

    float xGlCoordToScreenCoord(fvect2 screenSize, float x);
    float yGlCoordToScreenCoord(fvect2 screenSize, float y);
    std::tuple<fvect2, fvect2> getMoveAndSizeByCoords(fvect2 topLeft, fvect2 topRight, fvect2 bottomRight, fvect2 bottomLeft);

    fvect3 rgbToFloat(unsigned char r, unsigned char g, unsigned char b);
}