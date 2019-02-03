#ifndef HEATGRADIENT_H
#define HEATGRADIENT_H

#include <vector>
#include <QDebug>
#include "helpers.h"

class HeatGradient {


private:

    struct ColorPoint {
        float r,g,b;
        float val;
        ColorPoint(float red, float green, float blue, float value) : r(red), g(green), b(blue), val(value) {}
    };
    std::vector<ColorPoint> color;


public:

  explicit  HeatGradient();
  void      addColorPoint( float red, float green, float blue, float value );
  void      clearGradient();
  void      createDefaultHeatMapGradient();
  void      getColorAtValue(const float value, float &red, float &green, float &blue);
};

#endif // HEATGRADIENT_H
