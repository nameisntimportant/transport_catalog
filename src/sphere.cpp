#include "sphere.h"

#include <cmath>

using namespace std;

namespace
{
constexpr double DegreesToRadiansKoeff = M_PI / 180.0;
constexpr double EarthRadius = 6'371'000;
} // namespace

namespace Sphere
{
Point Point::fromDegrees(double latitude, double longitude)
{
    return {latitude * DegreesToRadiansKoeff, longitude * DegreesToRadiansKoeff};
}

double distance(Point lhs, Point rhs)
{
    lhs = Point::fromDegrees(lhs.latitude, lhs.longitude);
    rhs = Point::fromDegrees(rhs.latitude, rhs.longitude);
    return acos(sin(lhs.latitude) * sin(rhs.latitude) +
                cos(lhs.latitude) * cos(rhs.latitude) * cos(abs(lhs.longitude - rhs.longitude))) *
           EarthRadius;
}
} // namespace Sphere
