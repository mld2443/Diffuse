#pragma once

#include <chrono>   // time_point, steady_clock, now, nanoseconds, duration_cast
#include <string>   // string


class Timer {
public:
    Timer(const char* name);
    ~Timer();

private:
    const std::chrono::time_point<std::chrono::steady_clock> start;
    const std::string name;
};
