//
// Created by judah on 24-02-2025.
//
#include "ImGuiIDGuard.h"
#include "SKSE-MCP/SKSEMenuFramework.h"

ImGuiIDGuard::ImGuiIDGuard(const char* str_id) noexcept { ImGui::PushID(str_id); }

ImGuiIDGuard::ImGuiIDGuard(const char* str_id_begin, const char* str_id_end) noexcept {
    ImGui::PushID(str_id_begin, str_id_end);
}

ImGuiIDGuard::ImGuiIDGuard(int int_id) noexcept { ImGui::PushID(int_id); }

ImGuiIDGuard::ImGuiIDGuard(const void* ptr_id) noexcept { ImGui::PushID(ptr_id); }

ImGuiIDGuard::~ImGuiIDGuard() noexcept { ImGui::PopID(); }
