#include "helpers.h"


bool Helpers::isDoubleEqual(double a, double b) {

    return fabs(a - b) < DBL_EPSILON;
}

bool Helpers::isFloatEqual(float a, float b) {

    return static_cast<float>(fabs(static_cast<double>(a) - static_cast<double>(b))) < FLT_EPSILON;
}

