#pragma once
namespace InlineUtils
{
	inline float limitFloat(float value, float min_value, float max_value)
	{
		return (value < min_value) ? min_value : ((value > max_value) ? max_value : value);
	}

	inline RE::FormID GetFormIDFromIdentifier(const std::string& identifier)
	{
		RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
		auto                delimiter = identifier.find('|');
		if (delimiter != std::string::npos) {
			std::string        modName = identifier.substr(0, delimiter);
			std::string        modForm = identifier.substr(delimiter + 1);
			uint32_t           formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
			const RE::TESFile* mod = (RE::TESFile*)dataHandler->LookupModByName(modName.c_str());
			if (mod && mod != nullptr) {
				if (mod->IsLight())
					formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
				return dataHandler->LookupForm(formID, modName.c_str())->GetFormID();
			}
		}
		return (RE::FormID) nullptr;
	}
}

namespace Utils
{
	template <typename StringType>
	std::wstring toWideString(const StringType& str)
	{
		if constexpr (std::is_same_v<StringType, std::string_view>) {
			// Calculate the length of the wide string
			size_t wideLength = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), nullptr, 0);

			// Allocate a buffer for the wide string
			std::wstring wideStr(wideLength, L'\0');

			// Convert the string
			MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), &wideStr[0], static_cast<int>(wideLength));

			return wideStr;
		} else if constexpr (std::is_same_v<StringType, std::string>) {
			// Calculate the length of the wide string
			size_t wideLength = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);

			// Allocate a buffer for the wide string
			std::wstring wideStr(wideLength, L'\0');

			// Convert the string
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wideStr[0], static_cast<int>(wideLength));

			return wideStr;
		} else {
			static_assert(std::is_same_v<StringType, std::string> || std::is_same_v<StringType, std::string_view>, "Unsupported string type");
		}
	}

	void customMessageBox(const std::string& errorString)
	{
		// Create the confirmation message
		std::wstring confirmationMessage = toWideString<std::string>(errorString);
		std::wstring moduleName = toWideString<std::string_view>(SKSE::PluginDeclaration::GetSingleton()->GetName());
		// Call MessageBoxW with the confirmation message
		switch (MessageBoxW(nullptr, confirmationMessage.c_str(), moduleName.c_str(), MB_YESNO | MB_ICONQUESTION)) {
		case IDNO:
			SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_FAILURE);
		default:
			break;
		}
	}
}
