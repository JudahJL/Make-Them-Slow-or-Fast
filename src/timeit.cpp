//
// Created by judah on 23-02-2025.
//
#include "timeit.h"
#include <spdlog/spdlog.h>
#include <ctre.hpp>

std::string_view timeit::extract_function_name(const std::string_view full_signature) {
    constexpr auto pattern{ ctll::fixed_string{ R"((\w+(::\w+)*)\s*(?=\())" } };

    if(auto match{ ctre::search<pattern>(full_signature) }) { return match.get<1>(); }
    return full_signature.data();
}

timeit::~timeit() {
    const auto stop{ std::chrono::steady_clock::now() - start };
    char       buff[512];

    sprintf_s(buff, std::size(buff),
              "Time Taken in '%s' is %lld nanoseconds or %lld microseconds or "
              "%lld milliseconds or %lld seconds or %d minutes",
              extract_function_name(curr.function_name()).data(), stop.count(),
              std::chrono::duration_cast<std::chrono::microseconds>(stop).count(),
              std::chrono::duration_cast<std::chrono::milliseconds>(stop).count(),
              std::chrono::duration_cast<std::chrono::seconds>(stop).count(),
              std::chrono::duration_cast<std::chrono::minutes>(stop).count());
    SPDLOG_INFO(buff);
}

