#include "../json.h"
#include "log.h"
#include "testRunner.h"

using namespace std;

namespace
{
void assertIsNotAllTypesExcept(const Json::Node& node,
                               const std::string& exceptedType,
                               const std::string& anotherExceptedType = "")
{
    if (exceptedType != "int" && anotherExceptedType != "int")
    {
        ASSERT(!node.isInt());
    }
    if (exceptedType != "double" && anotherExceptedType != "double")
    {
        ASSERT(!node.isDouble());
    }
    if (exceptedType != "pureDouble" && anotherExceptedType != "pureDouble")
    {
        ASSERT(!node.isPureDouble());
    }
    if (exceptedType != "string" && anotherExceptedType != "string")
    {
        ASSERT(!node.isString());
    }
    if (exceptedType != "Array" && anotherExceptedType != "Array")
    {
        ASSERT(!node.isArray());
    }
    if (exceptedType != "Map" && anotherExceptedType != "Map")
    {
        ASSERT(!node.isMap());
    }
    if (exceptedType != "bool" && anotherExceptedType != "bool")
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
        assertIsNotAllTypesExcept(ssRoot, "string");
        ASSERT_EQUAL(ssRoot.asString(), "simpleString");

        istringstream ssww("\"simple string with witespaces\"");
        const auto sswwRoot = load(ssww).getRoot();
        ASSERT(sswwRoot.isString());
        assertIsNotAllTypesExcept(ssRoot, "string");
        ASSERT_EQUAL(sswwRoot.asString(), "simple string with witespaces");
    }

    {
        istringstream positiveN("835");
        const auto positiveNRoot = load(positiveN).getRoot();
        ASSERT(positiveNRoot.isInt());
        ASSERT(positiveNRoot.isDouble());
        assertIsNotAllTypesExcept(positiveNRoot, "int", "double");
        ASSERT_EQUAL(positiveNRoot.asInt(), 835);

        istringstream negativeN("-2400234");
        const auto negativeNRoot = load(negativeN).getRoot();
        ASSERT(negativeNRoot.isInt());
        ASSERT(negativeNRoot.isDouble());
        assertIsNotAllTypesExcept(negativeNRoot, "int", "double");
        ASSERT_EQUAL(negativeNRoot.asInt(), -2400234);

        istringstream maxN(std::to_string(std::numeric_limits<int>::max()));
        auto maxNRoot = Json::load(maxN).getRoot();
        ASSERT(maxNRoot.isInt());
        ASSERT(maxNRoot.isDouble());
        assertIsNotAllTypesExcept(negativeNRoot, "int", "double");
        ASSERT_EQUAL(maxNRoot.asInt(), std::numeric_limits<int>::max());

        istringstream minN(std::to_string(std::numeric_limits<int>::min()));
        auto minNRoot = Json::load(minN).getRoot();
        ASSERT(minNRoot.isInt());
        ASSERT(minNRoot.isDouble());
        assertIsNotAllTypesExcept(minNRoot, "int", "double");
        ASSERT_EQUAL(minNRoot.asInt(), std::numeric_limits<int>::min());

        istringstream zero(std::to_string(0));
        auto zeroRoot = Json::load(zero).getRoot();
        ASSERT(zeroRoot.isInt());
        ASSERT(zeroRoot.isDouble());
        assertIsNotAllTypesExcept(zeroRoot, "int", "double");
        ASSERT_EQUAL(zeroRoot.asInt(), 0);
    }

    {
        istringstream positiveN("7837824.4385");
        const auto positiveNRoot = load(positiveN).getRoot();
        ASSERT(positiveNRoot.isDouble());
        ASSERT(positiveNRoot.isPureDouble());
        assertIsNotAllTypesExcept(positiveNRoot, "double", "pureDouble");
        ASSERT_DOUBLE_EQUAL(positiveNRoot.asDouble(), 7837824.4385);

        istringstream negativeN("-0.348236");
        const auto negativeNRoot = load(negativeN).getRoot();
        ASSERT(negativeNRoot.isDouble());
        ASSERT(negativeNRoot.isPureDouble());
        assertIsNotAllTypesExcept(negativeNRoot, "double", "pureDouble");
        ASSERT_DOUBLE_EQUAL(negativeNRoot.asDouble(), -0.348236);

        istringstream smallN("0.000001");
        const auto smallNRoot = load(smallN).getRoot();
        ASSERT(smallNRoot.isDouble());
        ASSERT(smallNRoot.isPureDouble());
        assertIsNotAllTypesExcept(smallNRoot, "double", "pureDouble");
        ASSERT_DOUBLE_EQUAL(smallNRoot.asDouble(), 0.000001);

        istringstream maxN(std::to_string(std::numeric_limits<int>::max()));
        auto maxNRoot = Json::load(maxN).getRoot();
        ASSERT(maxNRoot.isDouble());
        ASSERT(maxNRoot.isInt());
        assertIsNotAllTypesExcept(maxNRoot, "double", "int");
        ASSERT_DOUBLE_EQUAL(maxNRoot.asDouble(), std::numeric_limits<int>::max());

        istringstream minN(std::to_string(std::numeric_limits<int>::min()));
        auto minNRoot = Json::load(minN).getRoot();
        ASSERT(minNRoot.isDouble());
        ASSERT(minNRoot.isInt());
        assertIsNotAllTypesExcept(minNRoot, "double", "int");
        ASSERT_DOUBLE_EQUAL(minNRoot.asDouble(), std::numeric_limits<int>::min());

        istringstream zero(std::to_string(0));
        auto zeroRoot = Json::load(zero).getRoot();
        ASSERT(zeroRoot.isDouble());
        ASSERT(zeroRoot.isInt());
        assertIsNotAllTypesExcept(zeroRoot, "double", "int");
        ASSERT_EQUAL(zeroRoot.asDouble(), 0);
    }

    {
        istringstream array("[\"A\", \"B\", \"C\"]");
        const auto arrayRoot = load(array).getRoot();
        ASSERT(arrayRoot.isArray());
        assertIsNotAllTypesExcept(arrayRoot, "Array");
        ASSERT_EQUAL(arrayRoot.asArray(), Array({Node("A"), Node("B"), Node("C")}));

        istringstream emptyArray("[]");
        const auto emptyArrayRoot = load(emptyArray).getRoot();
        ASSERT(emptyArrayRoot.isArray());
        assertIsNotAllTypesExcept(emptyArrayRoot, "Array");
        ASSERT_EQUAL(emptyArrayRoot.asArray(), Array({}));
    }

    {
        istringstream map("{\"A\" : 1, \"B\" : 2, \"C\" : 3}");
        const auto mapRoot = load(map).getRoot();
        ASSERT(mapRoot.isMap());
        assertIsNotAllTypesExcept(mapRoot, "Map");
        ASSERT_EQUAL(mapRoot.asMap(), Map({{"A", {Node(1)}}, {"B", {Node(2)}}, {"C", {Node(3)}}}));

        istringstream emptyMap("{}");
        const auto emptyMapRoot = load(emptyMap).getRoot();
        ASSERT(emptyMapRoot.isMap());
        assertIsNotAllTypesExcept(emptyMapRoot, "Map");
        ASSERT_EQUAL(emptyMapRoot.asMap(), Map({}));
    }

    {
        istringstream falseBool("false");
        const auto falseBoolRoot = load(falseBool).getRoot();
        ASSERT(falseBoolRoot.isBool());
        assertIsNotAllTypesExcept(falseBoolRoot, "bool");
        ASSERT_EQUAL(falseBoolRoot.asBool(), Node(false));

        istringstream trueBool("true");
        const auto trueBoolRoot = load(trueBool).getRoot();
        ASSERT(trueBoolRoot.isBool());
        assertIsNotAllTypesExcept(trueBoolRoot, "bool");
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
    assertIsNotAllTypesExcept(nestedRoot.asMap(), "Map");
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

void testPrint()
{
    const auto doublesArray = Array{Node(1.1), Node(2.2), Node(3.3)};
    const auto booleansArray = Array{Node(true), Node(false), Node(false), Node(true)};
    const auto stringToIntMap = Map{{"one", Node(1)}, {"ten", Node(10)}};
    const auto arrayOfMaps =
        Array{Map{{"trueKey", Node(true)}, {"falseKey", Node(false)}},
              Map{{"key", Node("value")}, {"another key", Node("another value")}}};
    const auto rootNode = Map{{"doublesArray", doublesArray},
                              {"booleansArray", booleansArray},
                              {"stringToIntMap", stringToIntMap},
                              {"arrayOfMaps", arrayOfMaps}};
    string expected(
        "{\"arrayOfMaps\": [{\"falseKey\": false, \"trueKey\": true}, {\"another key\": \"another "
        "value\", \"key\": \"value\"}], \"booleansArray\": [true, false, false, true], "
        "\"doublesArray\": [1.1, 2.2, 3.3], \"stringToIntMap\": {\"one\": 1, \"ten\": 10}}");

    ostringstream actual;
    print(JsonTree(rootNode), actual);
    ASSERT_EQUAL(actual.str(), expected);
}

void run()
{
    TestRunner tr;
    RUN_TEST(tr, testSingleElementLoading);
    RUN_TEST(tr, testNestedElementsLoading);
    RUN_TEST(tr, testInvalidInputException);
    RUN_TEST(tr, testPrint);
}
} // namespace Tests
} // namespace Json
