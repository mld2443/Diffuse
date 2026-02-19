#include "Timer.h"

#include <cmath> // log10, powf
#include <print> // println


Timer::Timer(const char* name)
  : start(std::chrono::steady_clock::now())
  , name(name) {
    std::println("Start \"{}\".", name);
}

Timer::~Timer() {
    const auto end = std::chrono::steady_clock::now();
    const uint64_t elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    const uint32_t digits = static_cast<uint32_t>(std::log10(elapsed));
    const uint32_t scale = std::min(digits / 3u, 3u);
    const float trimmed = static_cast<float>(elapsed) / std::powf(1000.f, static_cast<float>(scale));

    static const char *unitScales[4] = {"ns", "us", "ms", "s"};
    const char *units = unitScales[scale];

    std::println("-Stop \"{}\", {}{} elapsed.", name, trimmed, units);
}
