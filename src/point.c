#include "point.h"
#include <math.h>

#define PI 3.14159265358979323846

point_t rotate_n_move_point(point_t pt, int deg, point_t off) {
    double rad = (PI * deg) / 180.0;
    point_t out;
    out.x = round(pt.x * cos(rad) - pt.y * sin(rad)) + off.x;
    out.y = round(pt.x * sin(rad) + pt.y * cos(rad)) + off.y;
    return out;
}