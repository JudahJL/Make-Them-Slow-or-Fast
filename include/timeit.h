//
// Created by judah on 17-02-2025.
//

#ifndef TIMEIT_H
#define TIMEIT_H
#include <chrono>
#include <source_location>

class timeit
{
public:
    explicit timeit(const std::source_location& a_curr = std::source_location::current()):
        curr(a_curr) { }

    ~timeit();

private:
    std::source_location                  curr;
    std::chrono::steady_clock::time_point start{ std::chrono::steady_clock::now() };

    static std::string_view extract_function_name(std::string_view full_signature);
};
#endif  // TIMEIT_H
