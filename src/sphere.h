#pragma once

namespace Sphere
{
struct Point
{
    double latitude;
    double longitude;

    static Point fromDegrees(double latitude, double longitude);
};

double distance(Point lhs, Point rhs);
} // namespace Sphere
