#include "sphere.h"
#include "utils.h"

#include <cmath>
#include <string>

using namespace std;

namespace
{
constexpr double DegreesToRadiansKoeff = M_PI / 180.0;
constexpr double EarthRadius = 6'371'000;

bool belongsToRange(double n, double lowerBound, double upperBound)
{
    return n >= lowerBound && n <= upperBound;
}
} // namespace

namespace Sphere
{
Point Point::fromDegrees(double latitude, double longitude)
{
    ASSERT_WITH_MESSAGE(
        belongsToRange(latitude, -90.0, 90.0) && belongsToRange(longitude, -180.0, 180.0),
        "incorrect coordinates: "s + to_string(latitude) + ' ' + to_string(longitude));

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
