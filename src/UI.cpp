#include "UI.h"

#include "FilePtrManager.h"
#include "ImGuiIDGuard.h"
#include "ImGuiTreeNodeGuard.h"
#include "Settings.h"
#include "utils.h"
#include <ctre.hpp>

SMFRenderer SMFRenderer::Singleton;

SMFRenderer& SMFRenderer::GetSingleton() { return Singleton; }

void SMFRenderer::Register() {
    if(!SKSEMenuFramework::IsInstalled()) {
        SPDLOG_WARN("Unable to Register For SKSEMenuFramework, Please install SKSEMenuFramework to "
                    "configure the Json Files(if you want to)");
        return;
    }

    SKSEMenuFramework::SetSection("Make Them Slow or Fast");

    SKSEMenuFramework::AddSectionItem("Edit Presets", RenderEditPresets);

#ifndef NDEBUG
    SKSEMenuFramework::AddSectionItem("Debug", RenderDebug);
#endif
    SPDLOG_INFO("Registered For SKSEMenuFramework");
}

#pragma push_macro("GetObject")
#undef GetObject

#ifndef NDEBUG
void SMFRenderer::RenderDebug() {
    if(ImGui::Button("Quit Game?")) {
        REX::W32::TerminateProcess(REX::W32::GetCurrentProcess(), EXIT_SUCCESS);
    }
}
#endif

void DrawSplitter(const bool split_vertically, const float thickness, float* size0, float* size1, const float min_size0, const float min_size1) {
    ImVec2 backup_pos;
    ImGui::GetCursorPos(&backup_pos);

    if(split_vertically) {
        ImGui::SetCursorPosY(backup_pos.y + *size0);
    } else {
        ImGui::SetCursorPosX(backup_pos.x + *size0);
    }
    ImGui::SetNextItemAllowOverlap();  // This is to allow having other buttons OVER our splitter.
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // We don't draw while
                                                                       // active/pressed because as
                                                                       // we move the panes the
                                                                       // splitter button will be 1
                                                                       // frame late
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.10f));
    ImGui::Button("##Splitter", ImVec2(!split_vertically ? thickness : -1.0f, split_vertically ? thickness : -1.0f));
    ImGui::PopStyleColor(3);

    if(ImGui::IsItemActive()) {
        float mouse_delta =
            split_vertically ? ImGui::GetIO()->MouseDelta.y : ImGui::GetIO()->MouseDelta.x;

        // Minimum pane size
        if(mouse_delta < min_size0 - *size0) mouse_delta = min_size0 - *size0;
        if(mouse_delta > *size1 - min_size1) mouse_delta = *size1 - min_size1;

        // Apply resize
        *size0 += mouse_delta;
        *size1 -= mouse_delta;
    }
    ImGui::SetCursorPos(backup_pos);
}

inline void RightAlignSmallButtons(const std::initializer_list<const char*>& btns) {
    const ImGuiStyle* style = ImGui::GetStyle();

    float button_width = style->FramePadding.x * 2;

    for(const auto& label : btns) {
        button_width += ImGui::CalcTextSize(label, nullptr, true, -1.0f).x;  // text width
        button_width += style->FramePadding.x * 2;                           // padding per button
    }

    if(btns.size() > 1) {
        button_width += style->ItemSpacing.x * (static_cast<float>(btns.size()) - 1.0f);  // spacing
        // between
        // buttons
    }
    ImVec2 cra;
    ImGui::GetContentRegionAvail(&cra);
    ImGui::SameLine(ImGui::GetCursorPosX() + cra.x - button_width);
}

enum class AlignAxis : int {
    None = 0,
    X    = 1 << 0,
    Y    = 1 << 1,
    XY   = X | Y,
};

inline bool IsAligned(AlignAxis value, AlignAxis check) {
    return (static_cast<int>(value) & static_cast<int>(check)) != 0;
}

#pragma push_macro("max")
#undef max

inline void CenterText(const char* text, const AlignAxis axis = AlignAxis::X) {
    ImVec2 avail;
    ImGui::GetContentRegionAvail(&avail);
    const ImVec2 text_size = ImGui::CalcTextSize(text);

    if(IsAligned(axis, AlignAxis::X)) {
        const float offsetX = (avail.x - text_size.x) * 0.5f;
        if(offsetX > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
    }

    if(IsAligned(axis, AlignAxis::Y)) {
        const float offsetY = (avail.y - text_size.y) * 0.5f;
        if(offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
    }
}

#pragma pop_macro("max")

void SMFRenderer::RenderEditPresets() {
    auto&  s = Settings::GetSingleton();
    ImVec2 context_region;
    ImGui::GetContentRegionAvail(&context_region);

    if(s.patching) {
        FontAwesome::PushSolid();
        constexpr auto txt = UI::Format<"Patching Spells... {ICON} ...Please wait a moment", UI::IconHourglassStart>();
        CenterText(txt.data(), AlignAxis::XY);
        ImGui::Text(txt.data());
        FontAwesome::Pop();
        return;
    }

    auto&      r            = SMFRenderer::GetSingleton();
    const auto preset_names = std::views::keys(s.presets);

    if(!r.once_flag_for_left_width) [[unlikely]] {
        r.left_width               = context_region.x * .4f;
        r.once_flag_for_left_width = true;
    }

    constexpr float splitter_thickness = 4.0f;
    float           right_width        = context_region.x - r.left_width - splitter_thickness;

    DrawSplitter(false, 10.0f, &r.left_width, &right_width, 100, 100);
    // Left section: preset list
    ImGui::BeginChild("LeftSection", ImVec2(r.left_width, context_region.y), true);

    if(ImGui::BeginCombo("##Selected_Preset", InlineUtils::wstringToString(s.curr_preset.filename()).data(), ImGuiComboFlags_None)) {
        for(auto& preset_path : preset_names) {
            std::string_view label       = InlineUtils::wstringToString(preset_path.filename());
            const bool       is_selected = (s.curr_preset == preset_path);

            if(ImGui::Selectable(label.data(), is_selected)) { s.curr_preset = preset_path; }
            if(is_selected) { ImGui::SetItemDefaultFocus(); }
        }
        ImGui::EndCombo();
    }
    if(ImGui::IsItemHovered()) ImGui::SetTooltip("Select current preset");

    constexpr auto reload_curr_preset_btn_id = UI::Format<"{ICON}##load_curr_preset", UI::IconReload>();

    FontAwesome::PushSolid();

    r.DrawFeedbackButton(UI::IconSave{}, [&s]() { return s.SaveCurrPreset(); }, "Saves current preset to file");

    ImGui::SameLine();

    if(ImGui::SmallButton(reload_curr_preset_btn_id.data())) { s.LoadCurrPreset(); }

    if(ImGui::IsItemHovered()) ImGui::SetTooltip("reloads Current Preset from file");
    ImGui::SameLine();

    if(ImGui::SmallButton(UI::IconRevert{})) { s.RevertToDefault(); }

    if(ImGui::IsItemHovered()) ImGui::SetTooltip("Revert all changes to the originally saved values");
    ImGui::SameLine();

    if(ImGui::SmallButton(UI::IconPatch{})) { s.PatchSpells(); }

    if(ImGui::IsItemHovered())
        ImGui::SetTooltip("Apply current preset %ls to all spells", s.curr_preset.filename().c_str());
    FontAwesome::Pop();

    ImGui::Separator();

    (void)r.filter.Draw("Filter");
    FontAwesome::PushSolid();
    RightAlignSmallButtons({ UI::IconReload{} });
    if(ImGui::SmallButton(UI::IconReload{})) {
        s.LoadPresets();
        r.selected_path.clear();
    }
    FontAwesome::Pop();
    if(ImGui::IsItemHovered()) ImGui::SetTooltip("Reloads all presets from disk");

    std::string_view label;
    ImVec2           curr_content_region;
    ImGui::GetContentRegionAvail(&curr_content_region);
    curr_content_region.y -= ImGui::GetFrameHeightWithSpacing();
    if(r.warning[0] != '\0') curr_content_region.y -= ImGui::GetTextLineHeightWithSpacing();

    if(ImGui::BeginListBox("##PresetList", curr_content_region)) {
        for(auto itr = s.presets.begin(); itr != s.presets.end();) {
            label = InlineUtils::wstringToString(itr->first.filename());

            if(!r.filter.PassFilter(label.data())) continue;

            const bool is_selected = (r.selected_path == itr->first);
            {
                ImGui::BeginGroup();
                if(ImGui::Selectable(label.data(), is_selected, ImGuiSelectableFlags_AllowOverlap)) {
                    r.selected_path = itr->first;
                }

                std::array<char, MAX_PATH> btn_id{}, snd_btn_id{};
                sprintf_s(btn_id.data(), btn_id.size(), "%s##%s", static_cast<const char*>(UI::IconReload{}),
                          label.data());
                sprintf_s(snd_btn_id.data(), snd_btn_id.size(), "%s##%s", static_cast<const char*>(UI::IconTrashBin{}),
                          label.data());
                FontAwesome::PushSolid();
                RightAlignSmallButtons({ btn_id.data(), snd_btn_id.data() });
                r.DrawFeedbackButton(
                    btn_id.data(), [&s, &itr]() { return s.ReloadPreset(itr->first); }, "Reloads %s from File",
                    label.data());
                ImGui::SameLine();
                r.DrawFeedbackButton(
                    snd_btn_id.data(),
                    [&itr, &s, &r]() {
                        if(s.curr_preset == itr->first) { s.curr_preset.clear(); }
                        if(r.selected_path == itr->first) { r.selected_path.clear(); }
                        const auto ret = std::filesystem::remove(itr->first);
                        if(ret) { itr = s.presets.erase(itr); }
                        return ret;
                    },
                    "Deletes %s from disk", label.data());

                FontAwesome::Pop();
                ImGui::EndGroup();
            }
            if(is_selected) ImGui::SetItemDefaultFocus();

            if(itr != s.presets.end()) ++itr;
        }
        ImGui::EndListBox();
    }

    if(r.warning[0] != '\0')
        ImGui::TextColored(ImGui::GetStyle()->Colors[ImGuiCol_PlotHistogramHovered], r.warning.data());
    if(ImGui::InputText("Create Preset?", r.buff.data(), std::size(r.buff), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if(r.buff[0] == '\0') {
            std::snprintf(r.warning.data(), std::size(r.warning), "Type in something!");
        } else {
            char created_file_path[1'024]{};
            std::snprintf(created_file_path, std::size(created_file_path), BASE_PATH "Make Them Slow or Fast\\presets/%s.json",
                          r.buff.data());

            if(FilePtrManager f{ created_file_path, "wb" }) {
                char                       writeBuffer[1'024]{};
                rapidjson::FileWriteStream os(f.get(), writeBuffer, sizeof(writeBuffer));
                rapidjson::PrettyWriter    writer(os);
                rapidjson::Document        doc;
                s.GetDefaultPreset(doc);
                doc.Accept(writer);
                s.presets.try_emplace(created_file_path, std::move(doc));
                r.warning[0] = '\0';
            } else {
                std::snprintf(r.warning.data(), std::size(r.warning), "Failed to create preset! check the log for the reason.");
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    // Right section: preset details
    ImGui::BeginChild("RightSection", ImVec2(right_width, context_region.y), true);
    if(const auto file_status = std::filesystem::status(s.curr_preset); s.curr_preset.empty() || !std::filesystem::exists(file_status) || !std::filesystem::is_regular_file(file_status)) {
        constexpr auto msg = "No current preset selected. Please select one from the list on the "
                             "left and click the save button.";
        CenterText(msg);
        constexpr auto color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        ImGui::TextColored(color, msg);
        constexpr auto msg_2 = "Unexpected behaviour follows if you proceed.";
        CenterText(msg_2);
        ImGui::TextColored(color, msg_2);
        ImGui::Separator();
    }
    if(!r.selected_path.empty()) {
        if(const auto it = s.presets.find(r.selected_path); it != s.presets.end()) {
            rapidjson::Document& doc = it->second;
            const auto selected      = InlineUtils::wstringToString(r.selected_path.filename());

            CenterText(selected.data());
            ImGui::Text(selected.data());
            ImGui::Separator();
            RenderJsonEditor(selected, doc);
        }
    } else {
        ImGui::Text("Select a preset from the left.");
    }
    ImGui::EndChild();
}

void SMFRenderer::RenderJsonEditor(const std::string_view file, rapidjson::Document& doc) noexcept {
    if(doc.IsObject()) {
        for(auto& [name, value] : doc.GetObject()) {
            std::array<char, MAX_PATH> temp{};
            sprintf_s(temp.data(), temp.size(), "%s/%s", file.data(), name.GetString());
            RenderJsonValue(temp.data(), name.GetString(), value, doc.GetAllocator());
        }
    } else if(doc.IsArray()) {
        RenderJsonArray(file.data(), "[ ]", doc, doc.GetAllocator());
    } else {
        RenderJsonValue(file.data(), "", doc, doc.GetAllocator());
    }
}

// NOLINTBEGIN(*-no-recursion)

void SMFRenderer::RenderJsonValue(const char* uniqueID, const std::string_view key, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept {
    const ImGuiIDGuard currentPathIDManager{ uniqueID };
    if(value.IsObject()) {
        std::array<char, MAX_PATH> temp{};
        sprintf_s(temp.data(), temp.size(), "%s { }", key.data());
        if(ImGuiTreeNodeGuard i{ temp.data() }) {
            std::array<char, MAX_PATH> temp1{};
            sprintf_s(temp1.data(), temp1.size(), "%s/%s", uniqueID, key.data());
            RenderJsonObject(temp1.data(), value, alloc);
        }
    } else if(value.IsArray()) {
        std::array<char, MAX_PATH> temp{};
        sprintf_s(temp.data(), temp.size(), "%s [ ]", key.data());
        if(ImGuiTreeNodeGuard i{ temp.data() }) {
            std::array<char, MAX_PATH> temp1{};
            sprintf_s(temp1.data(), temp1.size(), "%s/%s", uniqueID, key.data());
            RenderJsonArray(temp1.data(), key, value, alloc);
        }
    } else if(value.IsString()) {
        if(key == "LogLevel") [[unlikely]] {
            int                  currentLogLevel{ spdlog::level::from_str(value.GetString()) };
            constexpr std::array LogLevels{ "trace"sv, "debug"sv,    "info"sv, "warning"sv,
                                            "error"sv, "critical"sv, "off"sv };

            if(ImGui::BeginCombo(key.data(), LogLevels[currentLogLevel].data())) {
                for(int i{}; i < LogLevels.size(); i++) {
                    const bool isSelected = (currentLogLevel == i);
                    if(ImGui::Selectable(LogLevels[i].data(), isSelected)) {
                        currentLogLevel = i;
                        value           = rapidjson::Value{}.SetString(
                            LogLevels[currentLogLevel].data(),
                            static_cast<rapidjson::SizeType>(LogLevels[currentLogLevel].size()), alloc);
                    }
                    if(isSelected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
        } else {
            char buffer[256];
            strcpy_s(buffer, std::size(buffer), value.GetString());
            if(ImGui::InputText(key.data(), buffer, std::size(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                value = rapidjson::Value{}.SetString(buffer, static_cast<rapidjson::SizeType>(strlen(buffer)), alloc);
            }
            if(ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Press Enter to save your input.\nMaximum allowed characters: %zu", std::size(buffer) - 1);
            }
        }
    } else if(value.IsBool()) {
        auto boolValue{ value.GetBool() };
        if(ImGui::Checkbox(key.data(), &boolValue)) { value = boolValue; }
    } else if(value.IsInt()) {
        auto intValue{ value.GetInt() };
        if(ImGui::InputInt(key.data(), &intValue, ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = intValue;
        }
    } else if(value.IsInt64()) {
        auto int64Value{ value.GetInt64() };
        if(ImGui::InputScalar(key.data(), ImGuiDataType_S64, &int64Value, nullptr, nullptr, "%lld", ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = int64Value;
        }
    } else if(value.IsUint()) {
        auto uintValue = value.GetUint();
        if(ImGui::InputScalar(key.data(), ImGuiDataType_U32, &uintValue, nullptr, nullptr, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = uintValue;
        }
    } else if(value.IsUint64()) {
        auto uint64Value = value.GetUint64();
        if(ImGui::InputScalar(key.data(), ImGuiDataType_U64, &uint64Value, nullptr, nullptr, "%llu", ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = uint64Value;
        }
    } else if(value.IsFloat()) {
        auto floatValue{ value.GetFloat() };
        if(ImGui::InputFloat(key.data(), &floatValue, 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = floatValue;
        }
    } else if(value.IsDouble()) {
        double doubleValue = value.GetDouble();
        if(ImGui::InputDouble(key.data(), &doubleValue, 0, 0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            value = doubleValue;
        }
    }
}

void SMFRenderer::RenderJsonObject(const char* uniqueID, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept {
    for(auto& [a_name, a_value] : value.GetObject()) {
        std::array<char, MAX_PATH> temp{};
        sprintf_s(temp.data(), temp.size(), "%s/%s", uniqueID, a_name.GetString());
        RenderJsonValue(temp.data(), a_name.GetString(), a_value, alloc);
    }
}

void SMFRenderer::RenderJsonArray(const char* uniqueID, const std::string_view key, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept {
    const ImGuiIDGuard current_path_id_manager{ uniqueID };
    for(auto it{ value.Begin() }; it != value.End();) {
        const auto i{ static_cast<rapidjson::SizeType>(std::distance(value.Begin(), it)) };
        std::array<char, MAX_PATH> Path{};
        sprintf_s(Path.data(), Path.size(), "%s/%s", uniqueID, key.data());

        const ImGuiIDGuard path_id_manager{ Path.data() };
        char               buff[64];
        std::snprintf(buff, std::size(buff), "[%u]", i);
        std::array<char, MAX_PATH> uniqueIDBuffer{};
        sprintf_s(uniqueIDBuffer.data(), std::size(uniqueIDBuffer), "%s/%u", Path.data(), i);
        RenderJsonValue(uniqueIDBuffer.data(), buff, *it, alloc);

        ImGui::SameLine();
        if(ImGui::Button(UI::IconCross{})) {
            it = value.Erase(it);
            continue;
        }
        if(ImGui::IsItemHovered()) {
            char buffer[64];
            sprintf_s(buffer, sizeof(buffer), "Removes line [%u]", i);
            ImGui::SetTooltip(buffer);
        }
        if(it != value.Begin()) {
            ImGui::SameLine();
            if(ImGui::ArrowButton("##up", ImGuiDir_Up)) {  // Move up
                std::iter_swap(it, std::prev(it));
                continue;
            }
            if(ImGui::IsItemHovered()) { ImGui::SetTooltip("Click Me to Move me Up"); }
        }
        if(std::next(it) != value.End()) {
            ImGui::SameLine();
            if(ImGui::ArrowButton("##down", ImGuiDir_Down)) {  // Move Down
                std::iter_swap(it, std::next(it));
                continue;
            }
            if(ImGui::IsItemHovered()) { ImGui::SetTooltip("Click Me to Move me Down"); }
        }
        ++it;
    }

    if(ImGui::Button(UI::IconPlus{})) {
        value.PushBack(rapidjson::Value{}.SetString("", 0, alloc), alloc);
    }
    if(ImGui::IsItemHovered()) { ImGui::SetTooltip("Adds a Empty Line"); }
}

SMFRenderer::SMFRenderer():
    warning(2'048, '\0'), buff(MAX_PATH, '\0') { }

template<typename F>
void SMFRenderer::DrawFeedbackButton(const std::string_view id, F&& func, const char* const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    const double now = ImGui::GetTime();

    bool       push_color = false;
    const auto it         = reload_feedback.find(id.data());
    if(it != reload_feedback.end()) {
        if(now < it->second.first) {
            ImGui::PushStyleColor(ImGuiCol_Button, it->second.second);
            push_color = true;
        } else {
            reload_feedback.erase(it);
        }
    }

    const bool clicked = ImGui::SmallButton(id.data());

    if(ImGui::IsItemHovered()) { ImGui::SetTooltipV(fmt, args); }

    if(push_color) { ImGui::PopStyleColor(); }
    if(clicked) {
        reload_feedback[id.data()] = std::pair<double, ImVec4>{ now + 5.0, func() ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f) };
    }

    va_end(args);
}

// NOLINTEND(*-no-recursion)
#pragma pop_macro("GetObject")
