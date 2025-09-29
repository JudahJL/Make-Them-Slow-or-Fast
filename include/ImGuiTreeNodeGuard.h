//
// Created by judah on 24-02-2025.
//

#ifndef IMGUITREENODEGUARD_H
#define IMGUITREENODEGUARD_H

class ImGuiTreeNodeGuard
{
public:
    explicit ImGuiTreeNodeGuard(const char* label) noexcept;

    explicit ImGuiTreeNodeGuard(const char* str_id, const char* fmt, ...) noexcept;

    explicit ImGuiTreeNodeGuard(const void* ptr_id, const char* fmt, ...) noexcept;

    ~ImGuiTreeNodeGuard() noexcept;

    [[nodiscard]] explicit operator bool () const noexcept;

    ImGuiTreeNodeGuard(const ImGuiTreeNodeGuard&)             = delete;
    ImGuiTreeNodeGuard(ImGuiTreeNodeGuard&&)                  = delete;
    ImGuiTreeNodeGuard& operator= (const ImGuiTreeNodeGuard&) = delete;
    ImGuiTreeNodeGuard& operator= (ImGuiTreeNodeGuard&&)      = delete;

private:
    bool result;
};
#endif  // IMGUITREENODEGUARD_H
