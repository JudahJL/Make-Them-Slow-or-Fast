//
// Created by judah on 24-02-2025.
//

#ifndef IMGUIIDGUARD_H
#define IMGUIIDGUARD_H

class ImGuiIDGuard
{
public:
    explicit ImGuiIDGuard(const char* str_id) noexcept;

    explicit ImGuiIDGuard(const char* str_id_begin, const char* str_id_end) noexcept;

    // ReSharper disable once CppParameterMayBeConst
    explicit ImGuiIDGuard(int int_id) noexcept;

    explicit ImGuiIDGuard(const void* ptr_id) noexcept;

    ~ImGuiIDGuard() noexcept;

    ImGuiIDGuard(const ImGuiIDGuard&)             = delete;
    ImGuiIDGuard(ImGuiIDGuard&&)                  = delete;
    ImGuiIDGuard& operator= (const ImGuiIDGuard&) = delete;
    ImGuiIDGuard& operator= (ImGuiIDGuard&&)      = delete;
};
#endif  // IMGUIIDGUARD_H
