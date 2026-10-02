#include "point.h"

#include <math.h>

// M_PI not defined in any math.h across platforms
#define PI 3.14159265358979323846

point_t point_rotate_and_translate(point_t pt, int deg, point_t offset) {
    // In math the Y axis goes up
    // Here it goes down, so we gotta invert the sign
    double rad = -(PI * deg) / 180.0;
    point_t out;
    out.x = round(pt.x * cos(rad) - pt.y * sin(rad)) + offset.x;
    out.y = round(pt.x * sin(rad) + pt.y * cos(rad)) + offset.y;
    return out;
}
