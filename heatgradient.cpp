#include "heatgradient.h"


HeatGradient::HeatGradient() {

    createDefaultHeatMapGradient();
}


void HeatGradient::addColorPoint( float red, float green, float blue, float value ) {

    for(unsigned int i=0; i < color.size(); i++) {
      if ( value < color[i].val ) {

        color.insert(color.begin()+i, ColorPoint(red,green,blue,value));
        return;
      }
    }
    color.push_back(ColorPoint(red, green, blue, value));
}


void HeatGradient::clearGradient() {

    color.clear();
}


void HeatGradient::createDefaultHeatMapGradient() {

    color.clear();
    color.push_back(ColorPoint(0, 0, 0, 0.0f));      // black
    color.push_back(ColorPoint(0, 0, 1, 0.1f));      // blue
    color.push_back(ColorPoint(0, 1, 1, 0.3f));     // cyan
    color.push_back(ColorPoint(0, 1, 0, 0.5f));      // green
    color.push_back(ColorPoint(1, 1, 0, 0.7f));     // yellow
    color.push_back(ColorPoint(1, 0, 0, 0.9f));      // red
    color.push_back(ColorPoint(1, 1, 1, 1.0f));      // white
}


void HeatGradient::getColorAtValue(const float value, float &red, float &green, float &blue) {

    if (color.size()==0) return;

    for(unsigned int i=0; i < color.size(); i++) {

        ColorPoint &currC = color[i];
        if (value < currC.val) {

            ColorPoint &prevC  = color[ static_cast<unsigned long long>(std::max(0, static_cast<int>(i-1))) ];
            float valueDiff    = (prevC.val - currC.val);
            float fractBetween = (Helpers::isFloatEqual(valueDiff, 0.0f)) ? 0 : (value - currC.val) / valueDiff;
            red   = (prevC.r - currC.r)*fractBetween + currC.r;
            green = (prevC.g - currC.g)*fractBetween + currC.g;
            blue  = (prevC.b - currC.b)*fractBetween + currC.b;
            return;
        }
    }
    red   = color.back().r;
    green = color.back().g;
    blue  = color.back().b;
    return;
}
