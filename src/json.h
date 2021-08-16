#pragma once

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Json
{
class Node;
using Array = std::vector<Node>;
using Map = std::map<std::string, Node>;

class Node final : public std::variant<Map, std::string, Array, int, double, bool>
{
public:
    using variant::variant;
    const variant& getBase() const
    {
        return *this;
    }

    bool isMap() const
    {
        return std::holds_alternative<Map>(*this);
    }
    const auto& asMap() const
    {
        return std::get<Map>(*this);
    }

    bool isString() const
    {
        return std::holds_alternative<std::string>(*this);
    }
    const auto& asString() const
    {
        return std::get<std::string>(*this);
    }

    bool isArray() const
    {
        return std::holds_alternative<Array>(*this);
    }
    const auto& asArray() const
    {
        return std::get<Array>(*this);
    }

    bool isInt() const
    {
        return std::holds_alternative<int>(*this);
    }
    int asInt() const
    {
        return std::get<int>(*this);
    }

    bool isPureDouble() const
    {
        return std::holds_alternative<double>(*this);
    }
    bool isDouble() const
    {
        return isPureDouble() || isInt();
    }
    double asDouble() const
    {
        return isPureDouble() ? std::get<double>(*this) : asInt();
    }

    bool isBool() const
    {
        return std::holds_alternative<bool>(*this);
    }
    bool asBool() const
    {
        return std::get<bool>(*this);
    }
};

class Tree
{
public:
    explicit Tree(Node root)
        : root_(move(root))
    {
    }

    const Node& getRoot() const
    {
        return root_;
    }

private:
    Node root_;
};

Node loadNode(std::istream& input);

Tree load(std::istream& input);

void printNode(const Node& node, std::ostream& output);

template <typename Value>
void printValue(const Value& value, std::ostream& output)
{
    output << value;
}

void printValue(const std::string& value, std::ostream& output);

void printValue(const bool& value, std::ostream& output);

void printValue(const Array& nodes, std::ostream& output);

void printValue(const Map& map, std::ostream& output);

void print(const Tree& tree, std::ostream& output);
} // namespace Json
