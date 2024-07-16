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

	const char* GetFolderPath()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _FolderPath;
	}

	bool GetDonePatchingSpells()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _DonePatchingSpells;
	}

	void SetDonePatchingSpells()
	{
		std::lock_guard<std::mutex> lock(_lock);
		_DonePatchingSpells = true;
	}

	void ResetDonePatchingSpells()
	{
		std::lock_guard<std::mutex> lock(_lock);
		_DonePatchingSpells = false;
	}

	const ordered_nJson& GetUnmodifiableMainJsonData()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _MainJsonData;
	}

	ordered_nJson& GetModifiableMainJsonData()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _MainJsonData;
	}

	void SetUserName(const std::string_view& UserName)
	{
		std::lock_guard<std::mutex> lock(_lock);
		_UserName = UserName;
	}

	const std::string_view GetUsername()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _UserName;
	}

	const ordered_nJson& GetUnmodifiableSpellInfo()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _SpellInfo;
	}

	ordered_nJson& GetModifiableSpellInfo()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _SpellInfo;
	}

	const std::vector<std::string>& GetUnmodifiableSpellModFiles()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _SpellModFiles;
	}

	std::vector<std::string>& GetModifiableSpellModFiles()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _SpellModFiles;
	}

	const std::unordered_set<std::string>& GetUnmodifiableFormIDArray()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _FormIDArray;
	}

	std::unordered_set<std::string>& GetModifiableFormIDArray()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _FormIDArray;
	}

	const std::unordered_set<std::string>& GetUnmodifiableTESFileArray()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _TESFileArray;
	}

	std::unordered_set<std::string>& GetModifiableTESFileArray()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _TESFileArray;
	}

private:
	DataManager() = default;
	~DataManager() = default;
	DataManager(const DataManager&) = delete;
	DataManager(DataManager&&) = delete;
	DataManager& operator=(const DataManager&) = delete;
	DataManager& operator=(DataManager&&) = delete;

private:
	const char*                     _FolderPath{ "Data/SKSE/Plugins/Make Them Slow or Fast/" };
	mutable bool                    _DonePatchingSpells{ false };
	mutable bool                    _hasFilesToMerge{ false };
	mutable bool                    _enableAimed{ true };
	mutable bool                    _changeAimedFireForgetSpeedEnable{ true };
	mutable bool                    _limitAimedFireForgetSpeedEnable{ false };
	mutable bool                    _RandomizeAimedFireForgetSpeedEnable{ false };
	mutable bool                    _changeAimedFireForgetGravityEnable{ false };
	mutable bool                    _limitAimedFireForgetGravityEnable{ false };
	mutable float                   _aimedFireForgetSpeed{ 1000.0f };
	mutable float                   _limitAimedFireForgetSpeedMin{ 10.0f };
	mutable float                   _limitAimedFireForgetSpeedMax{ 1000.0f };
	mutable float                   _RandomizeAimedFireForgetSpeedMin{ 3000.0f };
	mutable float                   _RandomizeAimedFireForgetSpeedMax{ 9000.0f };
	mutable float                   _aimedFireForgetGravity{ 1.0f };
	mutable float                   _limitAimedFireForgetGravityMin{ 0.0f };
	mutable float                   _limitAimedFireForgetGravityMax{ 3.0f };
	ordered_nJson                   _MainJsonData;  // used for main json file
	mutable std::string             _UserName{ "User" };
	ordered_nJson                   _SpellInfo;
	std::vector<std::string>        _SpellModFiles;
	std::unordered_set<std::string> _FormIDArray;
	std::unordered_set<std::string> _TESFileArray;
	std::mutex _lock;
};
