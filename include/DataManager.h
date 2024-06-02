#pragma once

namespace MTSOF
{
	class DataManager
	{
	public:
		bool _redo{ false };
		int _keyForRefresh{ 75 };
		int _keyForReset{ 77 };

		static DataManager* GetSingleton();

		void LoadJson(bool calledAtRuntime, std::shared_ptr<spdlog::logger> refresh);

		void spellPatch(bool calledAtKDataLoaded, std::shared_ptr<spdlog::logger> refresh);

	private:
		spdlog::level::level_enum _logLevel{ spdlog::level::info };
		std::string               _logLevelStr{ "info" };
		const char*               _folder_path{ "Data/SKSE/Plugins/Make Them Slow or Fast/" };
		nJson                     _jsonData;   // used for main json file
		nJson                     _mergeData;  // used to merge exclusion json files
		bool                      _hasFilesToMerge{ false };
		std::vector<std::string>  _formIDArray;
		std::vector<std::string>  _tesFileArray;
		const char*               _starredString{ "***************************************************************************************************" };
		bool                      _enableAimed{ true };
		bool                      _changeAimedFireForgetSpeedEnable{ true };
		float                     _aimedFireForgetSpeed{ 1000.0f };
		bool                      _limitAimedFireForgetSpeedEnable{ false };
		float                     _limitAimedFireForgetSpeedMin{ 10.0f };
		float                     _limitAimedFireForgetSpeedMax{ 10.0f };
		bool                      _changeAimedFireForgetGravityEnable{ false };
		float                     _aimedFireForgetGravity{ 1.0f };
		bool                      _limitAimedFireForgetGravityEnable{ false };
		float                     _limitAimedFireForgetGravityMin{ 0.0f };
		float                     _limitAimedFireForgetGravityMax{ 3.0f };
	};
}
