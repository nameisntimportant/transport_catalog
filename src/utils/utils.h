#pragma once

#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

template <typename It>
class Range
{
public:
    using ValueType = typename std::iterator_traits<It>::value_type;

    Range(It begin, It end)
        : begin_(begin)
        , end_(end)
    {
    }
    It begin() const
    {
        return begin_;
    }
    It end() const
    {
        return end_;
    }

private:
    It begin_;
    It end_;
};

template <typename It>
bool operator==(const Range<It>& lhs, const Range<It>& rhs)
{
    if (std::distance(lhs.begin(), lhs.end()) != std::distance(rhs.begin(), rhs.end()))
    {
        return false;
    }

    auto rhsIt = rhs.begin();
    for (auto lhsIt = lhs.begin(); lhsIt != lhs.end();
         lhsIt = std::next(lhsIt), rhsIt = std::next(rhsIt))
    {
        if (*lhsIt != *rhsIt)
        {
            return false;
        }
    }
    return true;
}

template <typename It>
std::ostream& operator<<(std::ostream& stream, const Range<It>& range)
{
    stream << "{";
    bool isFirst = true;
    for (const auto& item : range)
    {
        if (!isFirst)
        {
            stream << ", ";
        }
        isFirst = false;
        stream << item;
    }
    return stream << "}";
}

template <typename C>
auto asRange(const C& container)
{
    return Range{std::begin(container), std::end(container)};
}

template <typename It>
size_t calculateUniqueItemsCount(Range<It> range)
{
    return std::unordered_set<typename Range<It>::ValueType>{range.begin(), range.end()}.size();
}

template <typename K, typename V>
const V* getValuePointer(const std::unordered_map<K, V>& map, const K& key)
{
    if (auto it = map.find(key); it != end(map))
    {
        return &it->second;
    }
    else
    {
        return nullptr;
    }
}

std::string_view strip(std::string_view line);

bool fuzzyCompare(double lhs, double rhs);

#define UNREACHABLE(msg)                                                                        \
    {                                                                                           \
        std::ostringstream str;                                                                 \
        str << "unreachable point was reached! " << __FILE__ << ':' << __LINE__ << ". " << msg; \
        throw std::runtime_error(str.str());                                                    \
    }

#define M_ASSERT(expr, msg)                                                              \
    {                                                                                  \
        if (!expr)                                                                     \
        {                                                                              \
            std::ostringstream str;                                                    \
            str << "assertion failed! " << __FILE__ << ':' << __LINE__ << ". " << msg; \
            throw std::runtime_error(str.str());                                       \
        }                                                                              \
    }
