#pragma once

#include <chrono>
#include <iostream>
#include <string>

class LogDuration
{
public:
    explicit LogDuration(const std::string& msg = "")
        : logMessage(msg)
        , startTimePoint(std::chrono::steady_clock::now())
    {
    }

    ~LogDuration()
    {
        using namespace std::chrono;
        const auto duration = steady_clock::now() - startTimePoint;
        std::cerr << logMessage << ": " << duration_cast<milliseconds>(duration).count() << " ms"
                  << std::endl;
    }

private:
    std::string logMessage;
    std::chrono::steady_clock::time_point startTimePoint;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) LogDuration UNIQ_ID(__LINE__){message};
