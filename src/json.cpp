#include "json.h"
#include "utils.h"

using namespace std;

namespace Json
{

Node loadArray(istream& input)
{
    vector<Node> result;

    for (char c; input >> c && c != ']';)
    {
        if (c != ',')
        {
            input.putback(c);
        }
        result.push_back(loadNode(input));
    }

    return Node(move(result));
}

Node loadBool(istream& input)
{
    string s;
    while (isalpha(input.peek()))
    {
        s.push_back(input.get());
    }
    ASSERT_WITH_MESSAGE((s == "true" || s == "false"), "Can't cast string " + s + " to bool");
    return Node(s == "true");
}

Node loadNumber(istream& input)
{
    bool isNegative = input.peek() == '-';
    if (isNegative)
    {
        input.ignore(1);
    }

    // use double to store int part in case we are parsing double with int part bigger then INT_MAX
    double intPart = 0;
    while (isdigit(input.peek()))
    {
        intPart *= 10;
        intPart += input.get() - '0';
    }
    if (input.peek() != '.')
    {
        return Node(intPart * (isNegative ? -1 : 1));
    }

    input.ignore(1); // '.'
    double result = intPart;
    double fracCoeff = 0.1;
    while (isdigit(input.peek()))
    {
        result += fracCoeff * (input.get() - '0');
        fracCoeff /= 10;
    }
    return Node(result * (isNegative ? -1 : 1));
}

Node loadString(istream& input)
{
    string line;
    getline(input, line, '"');
    return Node(move(line));
}

Node loadMap(istream& input)
{
    Map result;

    for (char c; input >> c && c != '}';)
    {
        if (c == ',')
        {
            input >> c;
        }

        string key = loadString(input).asString();
        input >> c;
        result.emplace(move(key), loadNode(input));
    }

    return Node(move(result));
}

Node loadNode(istream& input)
{
    char c;
    input >> c;

    if (c == '[')
    {
        return loadArray(input);
    }
    else if (c == '{')
    {
        return loadMap(input);
    }
    else if (c == '"')
    {
        return loadString(input);
    }
    else if (c == 't' || c == 'f')
    {
        input.putback(c);
        return loadBool(input);
    }
    else if (isdigit(c) || c == '-')
    {
        input.putback(c);
        return loadNumber(input);
    }
    else
    {
        UNREACHABLE("Unexpected symbol in JSON input: "s + c);
    }
}

JsonTree load(istream& input)
{
    return JsonTree{loadNode(input)};
}

void printValue(const string& value, ostream& output)
{
    output << '"';
    for (const char c : value)
    {
        if (c == '"' || c == '\\')
        {
            output << '\\';
        }
        output << c;
    }
    output << '"';
}

void printValue(const bool& value, ostream& output)
{
    output << std::boolalpha << value;
}

void printValue(const Array& nodes, ostream& output)
{
    output << '[';
    bool first = true;
    for (const Node& node : nodes)
    {
        if (!first)
        {
            output << ", ";
        }
        first = false;
        printNode(node, output);
    }
    output << ']';
}

void printValue(const Map& map, ostream& output)
{
    output << '{';
    bool first = true;
    for (const auto& [key, node] : map)
    {
        if (!first)
        {
            output << ", ";
        }
        first = false;
        printValue(key, output);
        output << ": ";
        printNode(node, output);
    }
    output << '}';
}

void printNode(const Json::Node& node, ostream& output)
{
    visit([&output](const auto& value) { printValue(value, output); }, node.getBase());
}

void print(const JsonTree& tree, ostream& output)
{
    printNode(tree.getRoot(), output);
}

} // namespace Json
