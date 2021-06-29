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
    if (s != "true" && s != "false")
    {
        UNREACHABLE("Can't cast string " + s + " to bool")
    }
    return Node(s == "true");
}

Node loadNumber(istream& input)
{
    bool isNegative = input.peek() == '-';
    if (isNegative)
    {
        input.ignore(1);
    }

    int intPart = 0;
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

bool operator==(const Map& lhs, const Map& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    auto lhsIt = lhs.begin();
    auto rhsIt = rhs.begin();
    for (; lhsIt != lhs.end() && rhsIt != rhs.end(); lhsIt = next(lhsIt), rhsIt = next(rhsIt))
    {
        if (*lhsIt != *rhsIt)
        {
            return false;
        }
    }
    return true;
}

bool operator==(const Array& lhs, const Array& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); i++)
    {
        if (!(lhs[i] == rhs[i]))
        {
            return false;
        }
    }
    return true;
}

bool operator==(const Node& lhs, const Node& rhs)
{
    return lhs.getBase() == rhs.getBase();
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

void printValue(const bool& value, std::ostream& output)
{
    output << std::boolalpha << value;
}

void printValue(const Array& nodes, std::ostream& output)
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

void printValue(const Map& map, std::ostream& output)
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

std::ostream& operator<<(std::ostream& stream, const Node& node)
{
    printNode(node, stream);
    return stream;
}

} // namespace Json
