#pragma once

namespace InlineUtils {
    template<std::size_t N, char FillChar>
    [[nodiscard]] constexpr auto make_filled_char_array() -> std::array<char, N + 1> {
        std::array<char, N + 1> arr{};
        arr.fill(FillChar);
        arr.back() = '\0';
        return arr;
    }

    inline std::string _wstringToString(const std::wstring& wstr) {
        if(wstr.empty()) return {};
        const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string str(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), str.data(), sizeNeeded, nullptr, nullptr);
        return str;
    }

    static inline std::unordered_map<std::wstring, std::string> cache;

    inline std::string_view wstringToString(const std::wstring& wstr) {
        if(const auto it = cache.find(wstr); it != cache.end()) {
            return it->second;  // Return cached result
        }

        return cache.emplace(wstr, _wstringToString(wstr)).first->second;
    }

    [[nodiscard]] inline RE::TESForm* GetFormIDFromIdentifier(const std::string_view identifier) {
        try {
            auto* dataHandler = RE::TESDataHandler::GetSingleton();
            if(const auto delimiter = identifier.find('|'); delimiter != std::string::npos) {
                const auto modName = identifier.substr(0, delimiter);
                const auto modForm = identifier.substr(delimiter + 1);
                RE::FormID formID  = std::stoul(modForm.data(), nullptr, 16) & 0xFF'FF'FF;
                if(const RE::TESFile* mod = dataHandler->LookupModByName(modName)) {
                    if(mod->IsLight()) {
                        formID = std::stoul(modForm.data(), nullptr, 16) & 0xF'FF;
                    }
                    return dataHandler->LookupForm(formID, modName);
                }
            }
            return nullptr;
        } catch(const std::invalid_argument& e) {
            SPDLOG_ERROR("Invalid argument: {}", e.what());
            return nullptr;
        } catch(const std::out_of_range& e) {
            SPDLOG_ERROR("Out of range: {}", e.what());
            return nullptr;
        }
    }

    [[nodiscard]] inline std::string GetStringFromFormIDAndModName(const RE::FormID formID, const RE::TESFile* File) {
        const RE::FormID  FormID = File->IsLight() ? formID & 0xF'FF : formID & 0xFF'FF'FF;
        std::stringstream ss;
        ss << File->GetFilename() << "|0x" << std::uppercase << std::hex << FormID;
        return ss.str();
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    void limit(T& value, T min_value, T max_value) {
        value = (value < min_value) ? min_value : ((value > max_value) ? max_value : value);
    }

    template<class T>
    void RemoveAnyDuplicates(std::vector<T>& vec) {
        // Sort the vector
        std::sort(vec.begin(), vec.end());

        // Use std::unique to move duplicates to the end
        auto last = std::unique(vec.begin(), vec.end());

        // Erase the duplicates
        vec.erase(last, vec.end());
    }

    template<class T>
    [[nodiscard]] T* GetFormFromIdentifier(const std::string& identifier) {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();
        if(const auto delimiter = identifier.find('|'); delimiter != std::string::npos) {
            const std::string modName = identifier.substr(0, delimiter);
            const std::string modForm = identifier.substr(delimiter + 1);
            uint32_t          formID  = std::stoul(modForm, nullptr, 16) & 0xFF'FF'FF;
            if(const auto* mod = dataHandler->LookupModByName(modName)) {
                if(mod->IsLight()) formID = std::stoul(modForm, nullptr, 16) & 0xF'FF;
                return dataHandler->LookupForm<T>(formID, modName);
            }
        }
        return nullptr;
    }

    // ReSharper disable once CppNotAllPathsReturnValue
    template<class T>
    requires std::is_arithmetic_v<T>
    T getRandom(T min, T max) {
        // non-inclusive i.e., [min, max)
        if(min >= max) {
            char errorMessage[256];
            if constexpr(std::is_floating_point_v<T>) {
                sprintf_s(errorMessage, std::size(errorMessage), "The Value of min: '%f' must be lesser than the value of max: '%f'", min, max);  // max length possible: 153
            } else {
                sprintf_s(errorMessage, std::size(errorMessage), "The Value of min: '%lld' must be lesser than the value of max: '%lld'", static_cast<long long>(min), static_cast<long long>(max));  // max length possible: 99
            }
            throw std::invalid_argument(errorMessage);
        }
        static std::random_device rd;
        thread_local std::mt19937 gen(rd());
        if constexpr(std::is_integral_v<T>) {
            std::uniform_int_distribution<T> distrib(min, max - 1);
            return distrib(gen);
        } else if constexpr(std::is_floating_point_v<T>) {
            std::uniform_real_distribution<T> distrib(min, std::nextafter(max, min));
            return distrib(gen);
        }
    }
}  // namespace InlineUtils
