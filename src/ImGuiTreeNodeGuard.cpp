//
// Created by judah on 24-02-2025.
//
#include "ImGuiTreeNodeGuard.h"
#include "SKSE-MCP/SKSEMenuFramework.h"

ImGuiTreeNodeGuard::ImGuiTreeNodeGuard(const char* label) noexcept:
    result(ImGui::TreeNode(label)) { }

ImGuiTreeNodeGuard::ImGuiTreeNodeGuard(const char* str_id, const char* fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);
    result = ImGui::TreeNode(str_id, fmt, args);
    va_end(args);
}

ImGuiTreeNodeGuard::ImGuiTreeNodeGuard(const void* ptr_id, const char* fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);
    result = ImGui::TreeNode(ptr_id, fmt, args);
    va_end(args);
}

ImGuiTreeNodeGuard::~ImGuiTreeNodeGuard() noexcept {
    if(result) { ImGui::TreePop(); }
}

ImGuiTreeNodeGuard::operator bool () const noexcept { return result; }
