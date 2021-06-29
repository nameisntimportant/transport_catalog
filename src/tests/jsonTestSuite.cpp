#include "../json.h"
#include "log.h"
#include "testRunner.h"

using namespace std;

namespace
{
void assertIsNotAllTypesExceptOne(const Json::Node& node, const std::string& exceptedType)
{
    if (exceptedType != "int")
    {
        ASSERT(!node.isInt());
    }
    if (exceptedType != "double")
    {
        ASSERT(!node.isDouble());
    }
    if (exceptedType != "string")
    {
        ASSERT(!node.isString());
    }
    if (exceptedType != "Array")
    {
        ASSERT(!node.isArray());
    }
    if (exceptedType != "Map")
    {
        ASSERT(!node.isMap());
    }
    if (exceptedType != "bool")
    {
        ASSERT(!node.isBool());
    };
}
} // namespace

namespace Json
{
namespace Tests
{

void testSingleElementLoading()
{
    {
        istringstream ss("\"simpleString\"");
        const auto ssRoot = load(ss).getRoot();
        ASSERT(ssRoot.isString());
        assertIsNotAllTypesExceptOne(ssRoot, "string");
        ASSERT_EQUAL(ssRoot.asString(), "simpleString");

        istringstream ssww("\"simple string with witespaces\"");
        const auto sswwRoot = load(ssww).getRoot();
        ASSERT(sswwRoot.isString());
        assertIsNotAllTypesExceptOne(ssRoot, "string");
        ASSERT_EQUAL(sswwRoot.asString(), "simple string with witespaces");
    }

    {
        istringstream positiveN("835");
        const auto positiveNRoot = load(positiveN).getRoot();
        ASSERT(positiveNRoot.isInt());
        assertIsNotAllTypesExceptOne(positiveNRoot, "int");
        ASSERT_EQUAL(positiveNRoot.asInt(), 835);

        istringstream negativeN("-2400234");
        const auto negativeNRoot = load(negativeN).getRoot();
        ASSERT(negativeNRoot.isInt());
        assertIsNotAllTypesExceptOne(negativeNRoot, "int");
        ASSERT_EQUAL(negativeNRoot.asInt(), -2400234);

        istringstream maxN(std::to_string(std::numeric_limits<int>::max()));
        auto maxNRoot = Json::load(maxN).getRoot();
        ASSERT(maxNRoot.isInt());
        assertIsNotAllTypesExceptOne(negativeNRoot, "int");
        ASSERT_EQUAL(maxNRoot.asInt(), std::numeric_limits<int>::max());

        istringstream minN(std::to_string(std::numeric_limits<int>::min()));
        auto minNRoot = Json::load(minN).getRoot();
        ASSERT(minNRoot.isInt());
        assertIsNotAllTypesExceptOne(minNRoot, "int");
        ASSERT_EQUAL(minNRoot.asInt(), std::numeric_limits<int>::min());

        istringstream zero(std::to_string(0));
        auto zeroRoot = Json::load(zero).getRoot();
        ASSERT(zeroRoot.isInt());
        assertIsNotAllTypesExceptOne(zeroRoot, "int");
        ASSERT_EQUAL(zeroRoot.asInt(), 0);
    }

    {
        istringstream positiveN("7837824.4385");
        const auto positiveNRoot = load(positiveN).getRoot();
        ASSERT(positiveNRoot.isDouble());
        assertIsNotAllTypesExceptOne(positiveNRoot, "double");
        ASSERT_DOUBLE_EQUAL(positiveNRoot.asDouble(), 7837824.4385);

        istringstream negativeN("-0.348236");
        const auto negativeNRoot = load(negativeN).getRoot();
        ASSERT(negativeNRoot.isDouble());
        assertIsNotAllTypesExceptOne(negativeNRoot, "double");
        ASSERT_DOUBLE_EQUAL(negativeNRoot.asDouble(), -0.348236);

        istringstream smallN("0.000001");
        const auto smallNRoot = load(smallN).getRoot();
        ASSERT_DOUBLE_EQUAL(smallNRoot.asDouble(), 0.000001);
        assertIsNotAllTypesExceptOne(smallNRoot, "double");
        ASSERT(smallNRoot.isDouble());

        istringstream maxN(std::to_string(std::numeric_limits<int>::max()));
        auto maxNRoot = Json::load(maxN).getRoot();
        ASSERT(maxNRoot.isDouble());
        assertIsNotAllTypesExceptOne(maxNRoot, "double");
        ASSERT_DOUBLE_EQUAL(maxNRoot.asDouble(), std::numeric_limits<int>::max());

        istringstream minN(std::to_string(std::numeric_limits<int>::min()));
        auto minNRoot = Json::load(minN).getRoot();
        ASSERT(minNRoot.isDouble());
        assertIsNotAllTypesExceptOne(minNRoot, "double");
        ASSERT_DOUBLE_EQUAL(minNRoot.asDouble(), std::numeric_limits<int>::min());

        istringstream zero(std::to_string(0));
        auto zeroRoot = Json::load(zero).getRoot();
        ASSERT(zeroRoot.isDouble());
        assertIsNotAllTypesExceptOne(zeroRoot, "double");
        ASSERT_EQUAL(zeroRoot.asDouble(), 0);
    }

    {
        istringstream array("[\"A\", \"B\", \"C\"]");
        const auto arrayRoot = load(array).getRoot();
        ASSERT(arrayRoot.isArray());
        assertIsNotAllTypesExceptOne(arrayRoot, "Array");
        ASSERT_EQUAL(arrayRoot.asArray(), Array({Node("A"), Node("B"), Node("C")}));

        istringstream emptyArray("[]");
        const auto emptyArrayRoot = load(emptyArray).getRoot();
        ASSERT(emptyArrayRoot.isArray());
        assertIsNotAllTypesExceptOne(emptyArrayRoot, "Array");
        ASSERT_EQUAL(emptyArrayRoot.asArray(), Array({}));
    }

    {
        istringstream map("{\"A\" : 1, \"B\" : 2, \"C\" : 3}");
        const auto mapRoot = load(map).getRoot();
        ASSERT(mapRoot.isMap());
        assertIsNotAllTypesExceptOne(mapRoot, "Map");
        ASSERT_EQUAL(mapRoot.asMap(), Map({{"A", {Node(1)}}, {"B", {Node(2)}}, {"C", {Node(3)}}}));

        istringstream emptyMap("{}");
        const auto emptyMapRoot = load(emptyMap).getRoot();
        ASSERT(emptyMapRoot.isMap());
        assertIsNotAllTypesExceptOne(emptyMapRoot, "Map");
        ASSERT_EQUAL(emptyMapRoot.asMap(), Map({}));
    }

    {
        istringstream falseBool("false");
        const auto falseBoolRoot = load(falseBool).getRoot();
        ASSERT(falseBoolRoot.isBool());
        assertIsNotAllTypesExceptOne(falseBoolRoot, "bool");
        ASSERT_EQUAL(falseBoolRoot.asBool(), Node(false));

        istringstream trueBool("true");
        const auto trueBoolRoot = load(trueBool).getRoot();
        ASSERT(trueBoolRoot.isBool());
        assertIsNotAllTypesExceptOne(trueBoolRoot, "bool");
        ASSERT_EQUAL(trueBoolRoot.asBool(), Node(true));
    }
}

void testNestedElementsLoading()
{
    istringstream nested("{\n"
                         "\"doublesArray\": [1.1, 2.2, 3.3],\n"
                         "\"booleansArray\": [true, false, false, true],\n"
                         "\"stringToIntMap\": {\"one\": 1, \"ten\" : 10},\n"
                         "\"arrayOfMaps\": [\n"
                         "{\"trueKey\": true, \"falseKey\": false},\n"
                         "{\"key\": \"value\", \"another key\": \"another value\"}\n"
                         "]\n"
                         "}");

    const auto doublesArray = Array{Node(1.1), Node(2.2), Node(3.3)};
    const auto booleansArray = Array{Node(true), Node(false), Node(false), Node(true)};
    const auto stringToIntMap = Map{{"one", Node(1)}, {"ten", Node(10)}};
    const auto arrayOfMaps =
        Array{Map{{"trueKey", Node(true)}, {"falseKey", Node(false)}},
              Map{{"key", Node("value")}, {"another key", Node("another value")}}};
    const auto expectedResult = Map{{"doublesArray", doublesArray},
                                     {"booleansArray", booleansArray},
                                     {"stringToIntMap", stringToIntMap},
                                     {"arrayOfMaps", arrayOfMaps}};

    const auto nestedRoot = load(nested).getRoot();
    assertIsNotAllTypesExceptOne(nestedRoot.asMap(), "Map");
    ASSERT_EQUAL(nestedRoot.asMap(), expectedResult);
}

void testInvalidInputException()
{
    istringstream typoInput("fFalse");

    bool isExceptionThrown = false;
    try
    {
        load(typoInput).getRoot();
    }
    catch (...)
    {
        isExceptionThrown = true;

    }
    ASSERT(isExceptionThrown);
}

void run()
{
    TestRunner tr;
    RUN_TEST(tr, testSingleElementLoading);
    RUN_TEST(tr, testNestedElementsLoading);
    RUN_TEST(tr, testInvalidInputException);
}

} // namespace Tests
} // namespace Json
