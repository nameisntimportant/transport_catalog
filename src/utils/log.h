#pragma once
#include <iomanip>
#include <iostream>

class DebugOutput
{
public:
    DebugOutput(std::ostream& str = std::cout)
        : stream_(str){};

    template <typename T>
    DebugOutput& operator<<(const T& info)
    {
        stream_ << std::setprecision(6) << info << ' ';
        return *this;
    }

    ~DebugOutput()
    {
        stream_ << std::endl;
    }

private:
    std::ostream& stream_;
};

#define LOG_ERROR DebugOutput(std::cerr)
#define LOG_DEBUG DebugOutput()
