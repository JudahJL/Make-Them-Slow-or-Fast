#pragma once

class DataManager
{
public:
	static DataManager* GetSingleton();

	void LoadMainJson();

	void LoadExclusionJsonFiles();

	void PatchSpells();

	void RevertToDefault();

	void ReloadLoggingIfNecessary(const std::string_view& LogLevelStr);

	void ProcessMainJson();

	void LogDataManagerContents();

private:
	DataManager() = default;
	~DataManager() = default;
	DataManager(const DataManager&) = delete;
	DataManager(DataManager&&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	DataManager& operator=(DataManager&&) = delete;

public:
	const char*  _FolderPath{ "Data/SKSE/Plugins/Make Them Slow or Fast/" };
	mutable bool _DonePatchingSpells{ false };

private:
	mutable bool  _hasFilesToMerge{ false };
	mutable bool  _enableAimed{ true };
	mutable bool  _changeAimedFireForgetSpeedEnable{ true };
	mutable bool  _limitAimedFireForgetSpeedEnable{ false };
	mutable bool  _RandomizeAimedFireForgetSpeedEnable{ false };
	mutable bool  _changeAimedFireForgetGravityEnable{ false };
	mutable bool  _limitAimedFireForgetGravityEnable{ false };
	mutable float _aimedFireForgetSpeed{ 1000.0f };
	mutable float _limitAimedFireForgetSpeedMin{ 10.0f };
	mutable float _limitAimedFireForgetSpeedMax{ 1000.0f };
	mutable float _RandomizeAimedFireForgetSpeedMin{ 3000.0f };
	mutable float _RandomizeAimedFireForgetSpeedMax{ 9000.0f };
	mutable float _aimedFireForgetGravity{ 1.0f };
	mutable float _limitAimedFireForgetGravityMin{ 0.0f };
	mutable float _limitAimedFireForgetGravityMax{ 3.0f };

public:
	ordered_nJson                   _MainJsonData;  // used for main json file
	std::string                     _UserName{ "User" };
	ordered_nJson                   _SpellInfo;
	std::vector<std::string>        _SpellModFiles;
	std::unordered_set<std::string> _FormIDArray;
	std::unordered_set<std::string> _TESFileArray;
	std::mutex                      _lock;
};
