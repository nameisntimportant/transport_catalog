#pragma once

#include <string>
#include <unordered_map>

struct FromTo
{
    std::string from;
    std::string to;
};

inline bool operator==(const FromTo& lhs, const FromTo& rhs)
{
    return lhs.from == rhs.from && lhs.from == rhs.from;
}

struct FromToHasher
{
    size_t operator()(const FromTo& fromTo) const
    {
        const auto hashFrom = std::hash<std::string>{}(fromTo.from);
        const auto hashTo = std::hash<std::string>{}(fromTo.to);
        constexpr auto primeNumber = 37;
        return hashFrom * primeNumber + hashTo;
    }
};

using RouteDistancesMap = std::unordered_map<FromTo, unsigned int, FromToHasher>;
