#pragma once
#include "SKSE-MCP/SKSEMenuFramework.h"
#include <ctre.hpp>

namespace UI {
    template<unsigned int Code>
    struct Icon {
        static constexpr auto data = FontAwesome::UnicodeToUtf8Array<Code>();

        [[nodiscard]] constexpr operator std::basic_string_view<char> () const noexcept {
            return std::basic_string_view<char>{ data.data(), data.size() };
        }

        [[nodiscard]] constexpr operator const char* () const noexcept { return data.data(); }

        [[nodiscard]] constexpr auto size() const noexcept { return data.size(); }

        [[nodiscard]] constexpr auto begin() const noexcept { return data.begin(); }

        [[nodiscard]] constexpr auto end() const noexcept { return data.end(); }

        [[nodiscard]] constexpr auto front() const noexcept { return data.front(); }

        [[nodiscard]] constexpr auto back() const noexcept { return data.back(); }

        [[nodiscard]] constexpr auto cbegin() const noexcept { return data.cbegin(); }

        [[nodiscard]] constexpr auto cend() const noexcept { return data.cend(); }

        [[nodiscard]] constexpr auto rbegin() const noexcept { return data.rbegin(); }

        [[nodiscard]] constexpr auto rend() const noexcept { return data.rend(); }

        [[nodiscard]] constexpr auto at(std::size_t index) const { return data.at(index); }

        [[nodiscard]] constexpr auto operator[] (std::size_t index) const { return data.at(index); }

        [[nodiscard]] constexpr auto begin() noexcept { return data.begin(); }

        [[nodiscard]] constexpr auto end() noexcept { return data.end(); }

        [[nodiscard]] constexpr auto front() noexcept { return data.front(); }

        [[nodiscard]] constexpr auto back() noexcept { return data.back(); }

        [[nodiscard]] constexpr auto cbegin() noexcept { return data.cbegin(); }

        [[nodiscard]] constexpr auto cend() noexcept { return data.cend(); }

        [[nodiscard]] constexpr auto rbegin() noexcept { return data.rbegin(); }

        [[nodiscard]] constexpr auto rend() noexcept { return data.rend(); }

        [[nodiscard]] constexpr auto at(std::size_t index) { return data.at(index); }

        [[nodiscard]] constexpr auto operator[] (std::size_t index) { return data.at(index); }
    };

#define ICON_LIST                                                                                  \
    X(Plus, 0x00'2B)                                                                               \
    X(Cross, 0x00'58)                                                                              \
    X(Question, 0x00'3F)                                                                           \
    X(Reload, 0xF2'F1)                                                                             \
    X(Save, 0xF0'C7)                                                                               \
    X(TrashBin, 0xF1'F8)                                                                           \
    X(Revert, 0xF0'4A)                                                                             \
    X(HourglassStart, 0xF2'51)                                                                     \
    X(Patch, 0xF5'AD)

#define X(name, code) using Icon##name = Icon<code>;
    ICON_LIST
#undef X

    constexpr bool is_known_icon(auto) { return false; }

#define X(name, code)                                                                              \
    constexpr bool is_known_icon(Icon##name) { return true; }
    ICON_LIST
#undef X

    constexpr std::size_t count_substrings(std::u32string_view str, const std::u32string_view str_to_find) noexcept {
        if(str.empty() || str_to_find.empty()) return 0;

        std::size_t buzz{ 0 };
        while(str_to_find.size() <= str.size()) {
            const auto pos = str.find(str_to_find);
            if(pos == str.npos) break;  // NOLINT(*-static-accessed-through-instance)
            ++buzz;
            str.remove_prefix(pos + str_to_find.size());
        }
        return buzz;
    }

    template<typename... Icon>
    requires((sizeof...(Icon) > 0) && (is_known_icon(Icon{}) && ...))
    constexpr std::size_t CalcTrueLength(std::u32string_view str,const std::u32string_view needle) noexcept {
        std::size_t length = 0;

        // Helper to add UTF-8 byte length for a single UTF-32 codepoint
        auto add_utf8_length = [&](char32_t c) constexpr {
            if (c < 0x80) length += 1;
            else if (c < 0x800) length += 2;
            else if (c < 0x10000) length += 3;
            else length += 4;
        };

        (
            [&] {
                const auto pos = str.find(needle);
                if (pos == std::u32string_view::npos)
                    return;

                // count UTF-8 bytes before this {ICON}
                for (std::size_t i = 0; i < pos; ++i)
                    add_utf8_length(str[i]);

                // move past the {ICON}
                str.remove_prefix(pos + needle.size());

                // add the icon’s UTF-8 byte count
                length += Icon{}.size();
            }(),
            ...
        );

        // add any remaining characters after last {ICON}
        for (auto c : str)
            add_utf8_length(c);

        return length;
    }

    template<class... Icon>
    requires((sizeof...(Icon) > 0) && (is_known_icon(Icon{}) && ...))
    constexpr std::size_t SizeRequiredForIcons() {
        return (Icon{}.size() + ...) - sizeof...(Icon);
    }

    template<ctll::fixed_string fmt, typename... IconTs>
    requires(fmt.size() > 0 && sizeof...(IconTs) > 0 && (is_known_icon(IconTs{}) && ...))
    constexpr auto Format() {
        constexpr auto needle{ ctll::fixed_string("{ICON}") };
        constexpr auto count{ count_substrings(fmt, needle) };
        static_assert(count == sizeof...(IconTs), "Format specifiers don't match icon count");

        constexpr auto                 true_len = CalcTrueLength<IconTs...>(fmt, needle);
        std::array<char, true_len> buffer{};
        std::size_t                    pos       = 0;
        std::u32string_view            remaining = fmt;

        auto appendsv = [&pos, &buffer](std::string_view sv) constexpr {
            for(std::size_t i = 0; i < sv.size() - 1; ++i)
                buffer[pos++] = sv[i];
        };
        auto append832sv = [&pos, &buffer](std::u32string_view sv) constexpr {
            for(auto c : sv) {
                if(c < 0x80) buffer[pos++] = static_cast<char>(c);
                else if(c < 0x8'00) {
                    buffer[pos++] = static_cast<char>(0xC0 | (c >> 6));
                    buffer[pos++] = static_cast<char>(0x80 | (c & 0x3F));
                } else if(c < 0x1'00'00) {
                    buffer[pos++] = static_cast<char>(0xE0 | (c >> 12));
                    buffer[pos++] = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                    buffer[pos++] = static_cast<char>(0x80 | (c & 0x3F));
                } else {
                    buffer[pos++] = static_cast<char>(0xF0 | (c >> 18));
                    buffer[pos++] = static_cast<char>(0x80 | ((c >> 12) & 0x3F));
                    buffer[pos++] = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                    buffer[pos++] = static_cast<char>(0x80 | (c & 0x3F));
                }
            }
        };

        (
            [&remaining, &needle, &appendsv, &append832sv] {
                const auto found = remaining.find(needle);
                append832sv(remaining.substr(0, found));
                appendsv(IconTs{});
                remaining.remove_prefix(found + needle.size());
            }(),
            ...);

        append832sv(remaining);

        buffer[pos] = '\0';
        return buffer;
    }
#undef ICON_LIST
}  // namespace UI

class SMFRenderer
{
public:
    static SMFRenderer& GetSingleton();
    static void __stdcall Register();
#ifndef NDEBUG
    static void __stdcall RenderDebug();
#endif
    static void __stdcall RenderEditPresets();

    SMFRenderer(const SMFRenderer&)             = delete;
    SMFRenderer(SMFRenderer&&)                  = delete;
    SMFRenderer& operator= (const SMFRenderer&) = delete;
    SMFRenderer& operator= (SMFRenderer&&)      = delete;

    static void RenderJsonEditor(std::string_view file, rapidjson::Document& doc) noexcept;
    static void RenderJsonValue(const char* uniqueID, std::string_view key, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept;
    static void RenderJsonObject(const char* uniqueID, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept;
    static void RenderJsonArray(const char* uniqueID, std::string_view key, rapidjson::Value& value, rapidjson::MemoryPoolAllocator<>& alloc) noexcept;

    template<typename F>
    void DrawFeedbackButton(std::string_view id, F&& func, const char* fmt, ...);

private:
    SMFRenderer();

    ~SMFRenderer() = default;

    // caching
    std::filesystem::path                                      selected_path;
    ImGuiTextFilter                                            filter;
    std::unordered_map<std::string, std::pair<double, ImVec4>> reload_feedback;
    float                                                      left_width{ 0.f };
    std::vector<char>                                          warning;
    std::vector<char>                                          buff;
    bool                                                       once_flag_for_left_width{ false };

    static SMFRenderer Singleton;
};
