#include "sphere.h"
#include "log.h"
#include "random"
#include "testRunner.h"

using namespace std;

namespace
{
constexpr double EarthRadius = 6'371'000;
constexpr auto EquatorLength = 2 * M_PI * EarthRadius;

double getRandomLa()
{
    mt19937 gen;
    uniform_real_distribution<> unif(-90.0, 90.0);
    return unif(gen);
}

double getRandomLo()
{
    mt19937 gen;
    uniform_real_distribution<> unif(-180.0, 180.0);
    return unif(gen);
}
} // namespace

namespace Sphere
{
namespace Tests
{
void testOrthodromicDistance()
{
    ASSERT_DOUBLE_EQUAL(distance({145.1539, -139.398}, {-77.1804, -139.55}), 15307721.3517);
    ASSERT_DOUBLE_EQUAL(distance({-34.1539, 100.398}, {7.14, 129.55}), 5521923.004);
    ASSERT_DOUBLE_EQUAL(distance({1.1539, -120.398}, {77.1804, -129.55}), 8472294.8786);

    ASSERT_DOUBLE_EQUAL(distance({0.0, 0.0}, {0.0, 180.0}), EquatorLength / 2.0);
    ASSERT_DOUBLE_EQUAL(distance({-180.0, 0.0}, {0.0, 0.0}), EquatorLength / 2.0);
    ASSERT_DOUBLE_EQUAL(distance({-90.0, 0.0}, {90.0, 0.0}), EquatorLength / 2.0);

    ASSERT_DOUBLE_EQUAL(distance({55.574371, 37.6517}, {55.581065, 37.64839}), 772.870);
    ASSERT_DOUBLE_EQUAL(distance({55.581065, 37.64839}, {55.587655, 37.645687}), 752.207);
    ASSERT_DOUBLE_EQUAL(distance({55.587655, 37.645687}, {55.592028, 37.653656}), 697.996);
    ASSERT_DOUBLE_EQUAL(distance({55.592028, 37.653656}, {55.580999, 37.659164}), 1274.281);
    ASSERT_DOUBLE_EQUAL(distance({55.580999, 37.659164}, {55.574371, 37.6517}), 873.662);
    ASSERT_DOUBLE_EQUAL(distance({55.611087, 37.20829}, {55.595884, 37.209755}), 1692.999);
    ASSERT_DOUBLE_EQUAL(distance({55.632761, 37.333324}, {55.595884, 37.209755}), 8776.742);

    for (size_t i = 0; i < 100; i++)
    {
        const auto ranLa1 = getRandomLa();
        const auto ranLo1 = getRandomLa();
        const auto ranLa2 = getRandomLo();
        const auto ranLo2 = getRandomLo();

        const auto distanceOneWay = distance({ranLa1, ranLo1}, {ranLa2, ranLo2});
        const auto distanceTheWayBack = distance({ranLa2, ranLo2}, {ranLa1, ranLo1});

        stringstream description;
        DebugOutput(description) << "the way from" << ranLa1 << ranLo1 << "to" << ranLa2 << ranLo2
                                 << "is not equal to the way back";
        AssertEqual(distanceOneWay, distanceTheWayBack, description.str());
    }

    for (size_t i = 0; i < 100; i++)
    {
        const auto ranLa1 = getRandomLa();
        const auto ranLo1 = getRandomLo();

        stringstream description;
        DebugOutput(description) << "the way from" << ranLa1 << ranLo1
                                 << "to itself is not equal 0.0";
        AssertEqual(distance({ranLa1, ranLo1}, {ranLa1, ranLo1}), 0.0, description.str());
    }
}

void run()
{
    TestRunner tr;
    RUN_TEST(tr, testOrthodromicDistance);
};
} // namespace Tests
} // namespace Sphere
