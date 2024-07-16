#pragma once

namespace InlineUtils
{

	[[nodiscard]] inline RE::TESForm* GetFormIDFromIdentifier(const std::string& identifier)
	{
		try {
			RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
			size_t              delimiter = identifier.find('|');
			if (delimiter != std::string::npos) {
				std::string        modName = identifier.substr(0, delimiter);
				std::string        modForm = identifier.substr(delimiter + 1);
				RE::FormID         formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
				const RE::TESFile* mod = dataHandler->LookupModByName(modName);
				if (mod) {
					if (mod->IsLight()) {
						formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
					}
					return dataHandler->LookupForm(formID, modName);
				}
			}
			return nullptr;
		} catch (const std::invalid_argument& e) {
			logger::error("Invalid argument: {}", e.what());
			return nullptr;
		} catch (const std::out_of_range& e) {
			logger::error("Out of range: {}", e.what());
			return nullptr;
		}
	}

	[[nodiscard]] inline std::string GetStringFromFormIDAndModName(RE::FormID formID, RE::TESFile* File)
	{
		bool              isLight = File->recordFlags.all(RE::TESFile::RecordFlag::kSmallFile);
		RE::FormID        FormID = isLight ? formID & 0xFFF : formID & 0xFFFFFF;
		std::stringstream ss;
		ss << File->GetFilename() << "|0x" << std::uppercase << std::hex << FormID;
		// std::string identifier = std::format("{}|{:X}", File->GetFilename(), FormID);
		return ss.str();
	}

	template <typename StringType>
	[[nodiscard]] inline std::wstring toWideString(const StringType& str)
	{
		if constexpr (std::is_same_v<StringType, std::string_view>) {
			size_t wideLength = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), nullptr, 0);

			std::wstring wideStr(wideLength, L'\0');

			MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), &wideStr[0], static_cast<int>(wideLength));

			return wideStr;
		} else if constexpr (std::is_same_v<StringType, std::string>) {
			size_t wideLength = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);

			std::wstring wideStr(wideLength, L'\0');

			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wideStr[0], static_cast<int>(wideLength));

			return wideStr;
		} else {
			static_assert(std::is_same_v<StringType, std::string> || std::is_same_v<StringType, std::string_view>, "Unsupported string type");
		}
	}

	inline void customMessageBox(const std::string& errorString)
	{
		std::wstring confirmationMessage = InlineUtils::toWideString(errorString + "\nClick 'NO' to Close the Game");
		std::wstring moduleName = InlineUtils::toWideString(SKSE::PluginDeclaration::GetSingleton()->GetName());
		switch (REX::W32::MessageBoxW(nullptr, confirmationMessage.c_str(), moduleName.c_str(), MB_YESNO | MB_ICONQUESTION)) {
		case IDNO:
			REX::W32::TerminateProcess(REX::W32::GetCurrentProcess(), EXIT_FAILURE);
			break;
		default:
			break;
		}
	}

	template <typename T>
	inline void limit(T& value, T min_value, T max_value)
	{
		value = (value < min_value) ? min_value : ((value > max_value) ? max_value : value);
	}

	template <class T>
	inline void RemoveAnyDuplicates(std::vector<T>& vec)
	{
		// Sort the vector
		std::sort(vec.begin(), vec.end());

		// Use std::unique to move duplicates to the end
		auto last = std::unique(vec.begin(), vec.end());

		// Erase the duplicates
		vec.erase(last, vec.end());
	}

	template <class T>
	[[nodiscard]] inline T* GetFormFromIdentifier(const std::string& identifier)
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		auto delimiter = identifier.find('|');
		if (delimiter != std::string::npos) {
			std::string modName = identifier.substr(0, delimiter);
			std::string modForm = identifier.substr(delimiter + 1);
			uint32_t    formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
			auto*       mod = dataHandler->LookupModByName(modName.c_str());
			if (mod->IsLight())
				formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
			return dataHandler->LookupForm<T>(formID, modName.c_str());
		}
		return nullptr;
	}

	template <typename T>
	[[nodiscard]] inline T getRandom(T lower_bound, T upper_bound)
	{
		static std::random_device rd;
		static std::mt19937       gen(rd());

		if constexpr (std::is_integral<T>::value) {
			std::uniform_int_distribution<T> distributor(lower_bound, upper_bound);
			return distributor(gen);
		} else if constexpr (std::is_floating_point<T>::value) {
			std::uniform_real_distribution<T> distributor(lower_bound, upper_bound);
			return distributor(gen);
		}
	}

	[[nodiscard]] inline bool resolve_symlink(fs::path& entry_path, size_t max_depth)
	{
		size_t depth{ 0sz };
		while (fs::is_symlink(entry_path)) {
			if (depth >= max_depth) {
				logger::error("Maximum symlink depth reached: {}", fs::absolute(entry_path).generic_string());
				return false;
			}
			entry_path = fs::read_symlink(entry_path);
			depth++;
		}
		return true;
	}
}
