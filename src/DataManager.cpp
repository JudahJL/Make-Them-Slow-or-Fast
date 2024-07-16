#include "DataManager.h"
#include "logging.h"
#include "utils.h"

DataManager* DataManager::GetSingleton()
{
	static DataManager Singleton;
	return std::addressof(Singleton);
}

void DataManager::LoadMainJson()
{
	std::lock_guard<std::mutex> lock(_lock);
	std::ifstream               MainJsonFile(std::format("Data/SKSE/Plugins/{}.json", SKSE::PluginDeclaration::GetSingleton()->GetName()));

	try {
		MainJsonFile >> _MainJsonData;
	} catch (const ordered_nJson::parse_error& e) {
		logger::error("Data/SKSE/Plugins/{}.json parsing error : {}", SKSE::PluginDeclaration::GetSingleton()->GetName(), e.what());
		logger::error("loading default {}.json", SKSE::PluginDeclaration::GetSingleton()->GetName());

		InlineUtils::customMessageBox(std::format("{}. Are you sure you want to continue? if you continue, The error will be logged and a default json will be used instead.", e.what()));
		_MainJsonData = {
			{ "Logging", { { "LogLevel", "info" } } },
			{ "User Details", { { "Username", "User" } } },
			{ "Aimed", { { "Enable", true },
						   { "Fire and Forget",
							   { "Change Speed", { { "Enable", true },
													 { "Speed", 1000.0f } } },
							   { "Limit Speed", { { "Enable", false },
													{ "Min", 10.0f },
													{ "Max", 1000.0f } } },
							   { "Randomize Speed", { { "Enable", true },
														{ "Min", 3000.0f },
														{ "Max", 12000.0f } } },
							   { "Change Gravity", { { "Enable", false },
													   { "Gravity", 1.0f } } },
							   { "Limit Gravity", { { "Enable", false },
													  { "Min", 0.0f },
													  { "Max", 3.0f } } } } } }
		};
	}
}

void DataManager::LoadExclusionJsonFiles()
{
	try {
		if (fs::exists(_FolderPath) && !fs::is_empty(_FolderPath)) {
			_hasFilesToMerge = true;
			for (const auto& entry : fs::directory_iterator(_FolderPath)) {
				fs::path entry_path = entry.path();

				if (!InlineUtils::resolve_symlink(entry_path, 10)) {
					logger::error("Skipping entry due to symlink loop: {}", fs::absolute(entry_path).generic_string());
					continue;
				}

				if (fs::is_regular_file(entry_path) && entry_path.extension() == ".json") {
					std::string EntryPathStr(fs::absolute(entry_path).generic_string());

					std::ifstream jFile(entry_path);

					if (!jFile.is_open()) {
						logger::error("Failed to open file: {}", EntryPathStr);
						continue;
					}

					ordered_nJson MergeJsonData;
					try {
						jFile >> MergeJsonData;
					} catch (const ordered_nJson::parse_error& e) {
						std::string_view ErrorMessage(e.what());
						logger::error("{}:{}", EntryPathStr, ErrorMessage);
						logger::error("If you get this error, check your {}. The line above will tell where the mistake is.", EntryPathStr);
						InlineUtils::customMessageBox(std::format("{}. Are you sure you want to continue? if you continue, {} will be ignored and the error will be logged.", ErrorMessage, EntryPathStr));
					}

					logger::debug("Loaded JSON from file: {}", EntryPathStr);

					constexpr static const char* data1{ "SPELL FormID to Exclude" };
					constexpr static const char* data2{ "SPELL File(s) to Exclude" };
					if (MergeJsonData[data1].is_array() && MergeJsonData[data2].is_array()) {
						for (const std::string& a : MergeJsonData[data1]) _FormIDArray.insert(a);
						for (const std::string& a : MergeJsonData[data2]) _TESFileArray.insert(a);
					}
				}
			}
		}

	} catch (const ordered_nJson::type_error& e) {
		logger::error("{}", e.what());
		InlineUtils::customMessageBox(std::format("{}. Want to Continue?", e.what()));
	} catch (const ordered_nJson::other_error& e) {
		logger::error("{}", e.what());
		InlineUtils::customMessageBox(std::format("{}. Want to Continue?", e.what()));
	} catch (const ordered_nJson::exception& e) {
		logger::error("{}", e.what());
		InlineUtils::customMessageBox(std::format("{}. Want to Continue?", e.what()));
	}

	if (!(_hasFilesToMerge))
		logger::info("************************************No Exclusion will be Done**************************************");
}

void DataManager::PatchSpells()
{
	std::lock_guard<std::mutex> lock(_lock);
	_DonePatchingSpells = false;
	constexpr const char*   _starredString{ "***************************************************************************************************" };
	ordered_nJson           j;
	std::chrono::time_point startSP = std::chrono::high_resolution_clock::now();
	if (_enableAimed) {
		for (const auto* spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
			j.clear();
			if (spell) {
				j["ModName"] = spell->GetFile()->GetFilename();                                                          //ModName
				j["SpellName"] = spell->GetFullName();                                                                   //SpellName
				j["SpellFormID"] = spell->GetRawFormID();                                                                //SpellFormID
				j["SpellString"] = InlineUtils::GetStringFromFormIDAndModName(spell->GetRawFormID(), spell->GetFile());  //SpellString
				j["ProjFormID"] = nullptr;                                                                               //ProjFormID
				j["ProjSpeed"] = nullptr;                                                                                //ProjSpeed
				j["ProjGravity"] = nullptr;                                                                              //ProjGravity
				auto* spellEffectSetting = spell->GetAVEffect();
				if (spellEffectSetting) {
					spellEffectSetting;
					auto* spellProjectile = spellEffectSetting->data.projectileBase;
					if (spellProjectile) {
						j["ProjFormID"] = spellProjectile->GetRawFormID();  //ProjFormID
						j["ProjSpeed"] = spellProjectile->data.speed;       //ProjSpeed
						j["ProjGravity"] = spellProjectile->data.gravity;   //ProjGravity
						bool shouldPatch = true;
						if (_hasFilesToMerge) {
							for (const std::string_view spellModName : _TESFileArray) {
								if (spellModName == spell->GetFile()->GetFilename()) {
									shouldPatch = false;

									logger::debug("{}", _starredString);
									logger::debug("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), spell->GetRawFormID(),
										spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
									logger::debug("{}", _starredString);
									break;
								}
							}
							if (shouldPatch) {
								for (const std::string& spellFormID : _FormIDArray) {
									auto form = InlineUtils::GetFormIDFromIdentifier(spellFormID);
									if (form) {
										if (spell->GetFormID() == form->GetFormID()) {
											shouldPatch = false;

											logger::debug("{}", _starredString);
											logger::debug("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), form->GetFormID(),
												spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
											logger::debug("{}", _starredString);
											break;
										}
									}
								}
							}
						}

						if (shouldPatch) {
							if (spell && (spell->data.spellType == RE::MagicSystem::SpellType::kSpell)) {
								auto delivery = spell->data.delivery;
								auto casttype = spell->data.castingType;
								bool SpellPatched = false;
								if (_changeAimedFireForgetSpeedEnable || _limitAimedFireForgetSpeedEnable || _changeAimedFireForgetGravityEnable || _limitAimedFireForgetGravityEnable)
									SpellPatched = true;
								if ((delivery == RE::MagicSystem::Delivery::kAimed) && (casttype == RE::MagicSystem::CastingType::kFireAndForget)) {
									if (SpellPatched) {
										logger::debug("{}", _starredString);
										logger::debug("Original Aimed,Fire And Forget Spell : Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
											spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
									}
									if (_changeAimedFireForgetSpeedEnable) {
										spellProjectile->data.speed = _aimedFireForgetSpeed;
										logger::debug("modified speed");
									}
									if (_limitAimedFireForgetSpeedEnable) {
										InlineUtils::limit(spellProjectile->data.speed, _limitAimedFireForgetSpeedMin, _limitAimedFireForgetSpeedMax);
										logger::debug("limited speed");
									}
									if (_RandomizeAimedFireForgetSpeedEnable) {
										float speed = InlineUtils::getRandom(_RandomizeAimedFireForgetSpeedMin, _RandomizeAimedFireForgetSpeedMax);
										spellProjectile->data.speed = speed;
										logger::debug("Randomized Speed to {}", speed);
									}
									if (_changeAimedFireForgetGravityEnable) {
										spellProjectile->data.gravity = _aimedFireForgetGravity;
										logger::debug("modified gravity");
									}
									if (_limitAimedFireForgetGravityEnable) {
										InlineUtils::limit(spellProjectile->data.gravity, _limitAimedFireForgetGravityMin, _limitAimedFireForgetGravityMax);
										logger::debug("limited gravity");
									}
									if (SpellPatched) {
										logger::debug("Modified Aimed,Fire And Forget Spell : Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
											spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
										logger::debug("{}", _starredString);
									}
								}
							}
						}
					}
				}
			}
			_SpellInfo.push_back(j);
		}
	}
	logger::info("{} {} Finished Patching.", SKSE::PluginDeclaration::GetSingleton()->GetName(), SKSE::PluginDeclaration::GetSingleton()->GetVersion().string("."));

#ifdef TESTING
	size_t size{ 0sz };
	for (const std::string_view s : _FormIDArray) {
		logger::debug("_FormIDArray[{}] : {}", size, s);
		size++;
	}
	size = 0sz;
	for (const std::string_view s : _TESFileArray) {
		logger::debug("_TESFileArray[{}] : {}", size, s);
		size++;
	}
#endif
	_SpellModFiles.clear();

	for (auto& item : _SpellInfo) _SpellModFiles.push_back(item["ModName"].get<std::string>());

	InlineUtils::RemoveAnyDuplicates(_SpellModFiles);

	_DonePatchingSpells = true;

	std::chrono::nanoseconds nanosecondsTakenForSP = std::chrono::duration(std::chrono::high_resolution_clock::now() - startSP);

	logger::info("Time Taken in {} totally is {} nanoseconds or {} microseconds or {} milliseconds or {} seconds or {} minutes", std::source_location::current().function_name(), nanosecondsTakenForSP.count(),
		std::chrono::duration_cast<std::chrono::microseconds>(nanosecondsTakenForSP).count(), std::chrono::duration_cast<std::chrono::milliseconds>(nanosecondsTakenForSP).count(),
		std::chrono::duration_cast<std::chrono::seconds>(nanosecondsTakenForSP).count(), std::chrono::duration_cast<std::chrono::minutes>(nanosecondsTakenForSP).count());
}

void DataManager::RevertToDefault()
{
	logger::info("");
	logger::info("Starting to Revert");
	if (!_SpellInfo.empty()) {
		_DonePatchingSpells = false;
		constexpr const char* starString = "******************************************************************************************************************************";

		for (size_t i = 0sz; i < _SpellInfo.size(); i++) {
			if (auto spell = InlineUtils::GetFormFromIdentifier<RE::SpellItem>(_SpellInfo[i]["SpellString"]); spell) {
				if (auto SpellEffectSetting = spell->GetAVEffect(); SpellEffectSetting) {
					if (auto SpellProjectile = SpellEffectSetting->data.projectileBase; SpellProjectile) {
						if (spell->GetRawFormID() == _SpellInfo[i]["SpellFormID"].get<RE::FormID>() && SpellProjectile->GetRawFormID() == _SpellInfo[i]["ProjFormID"].get<RE::FormID>()) {
							logger::debug("{}", starString);
							logger::debug("Before Reverting : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), spell->GetRawFormID(),
								SpellProjectile->GetFullName(), SpellProjectile->GetRawFormID(), SpellProjectile->data.speed, SpellProjectile->data.gravity, spell->GetFile()->GetFilename());
							SpellProjectile->data.speed = _SpellInfo[i]["ProjSpeed"].get<float>();
							SpellProjectile->data.gravity = _SpellInfo[i]["ProjGravity"].get<float>();
							logger::debug("After Reverting : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), spell->GetRawFormID(),
								SpellProjectile->GetFullName(), SpellProjectile->GetRawFormID(), SpellProjectile->data.speed, SpellProjectile->data.gravity, spell->GetFile()->GetFilename());
							logger::debug("{}", starString);
						}
					}
				}
			}
		}
	}
	_DonePatchingSpells = true;
	logger::info("Finished Reverting");
	logger::info("");
}

void DataManager::ReloadLoggingIfNecessary(const std::string_view& LogLevelStr)
{

	const static std::unordered_map<std::string_view, spdlog::level::level_enum> logLevelMap{
		{ "trace"sv, spdlog::level::trace },
		{ "debug"sv, spdlog::level::debug },
		{ "info"sv, spdlog::level::info },
		{ "warn"sv, spdlog::level::warn },
		{ "err"sv, spdlog::level::err },
		{ "critical"sv, spdlog::level::critical },
		{ "off"sv, spdlog::level::off }
	};
	constexpr static const char* Pattern{ "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v" };

	auto it = logLevelMap.find(LogLevelStr);
	if (it != logLevelMap.end()) {
		spdlog::level::level_enum newLevel = it->second;

		if (newLevel != spdlog::level::off) {
			if (spdlog::default_logger()->sinks().empty()) {
        logger::info("Spdlog Sinks are empty. Creating new sinks on log level: {}", LogLevelStr);
				Logging logger(newLevel);
			} else {
				spdlog::set_level(newLevel);
				spdlog::flush_on(newLevel);
				spdlog::set_pattern(Pattern);
			}
		} else {
			spdlog::default_logger()->sinks().clear();
			spdlog::set_pattern("");
		}
	} else {
		logger::error("Invalid log level: {}. Defaulting to info.", LogLevelStr);
		if (spdlog::default_logger()->sinks().empty()) {
      logger::info("Spdlog Sinks are empty. Creating new sinks on log level: info");
			Logging logger(spdlog::level::info);
		} else {
			spdlog::set_level(spdlog::level::info);
			spdlog::flush_on(spdlog::level::info);
			spdlog::set_pattern(Pattern);
		}
	}
}

void DataManager::ProcessMainJson()
{  //                                             0           1          2            3             4               5             6            7       8       9          10                   11          12                13
	constexpr const char* MainJsonKeys[]{ "User Details", "Username", "Aimed", "Enable", "Fire and Forget", "Change Speed", "Speed", "Limit Speed", "Min", "Max", "Randomize Speed", "Change Gravity", "Gravity", "Limit Gravity" };

	ordered_nJson Aimed = _MainJsonData[MainJsonKeys[2]];  //Aimed

	ordered_nJson AimedFireForget = Aimed[MainJsonKeys[4]];  //Fire And Forget for Aimed

	ordered_nJson AimedFireForgetCS = AimedFireForget[MainJsonKeys[5]];   //CS == Change Speed
	ordered_nJson AimedFireForgetLS = AimedFireForget[MainJsonKeys[7]];   //LS == Limit Speed
	ordered_nJson AimedFireForgetRS = AimedFireForget[MainJsonKeys[10]];  //RS == Randomize Speed
	ordered_nJson AimedFireForgetCG = AimedFireForget[MainJsonKeys[11]];  //CG == Change Gravity
	ordered_nJson AimedFireForgetLG = AimedFireForget[MainJsonKeys[13]];  //LS == Limit Gravity

	//Enable
	constexpr auto Enable{ 3 };
	_enableAimed = Aimed[MainJsonKeys[Enable]].get<bool>();
	_changeAimedFireForgetSpeedEnable = AimedFireForgetCS[MainJsonKeys[Enable]].get<bool>();
	_limitAimedFireForgetSpeedEnable = AimedFireForgetLS[MainJsonKeys[Enable]].get<bool>();
	_RandomizeAimedFireForgetSpeedEnable = AimedFireForgetRS[MainJsonKeys[Enable]].get<bool>();
	_changeAimedFireForgetGravityEnable = AimedFireForgetCG[MainJsonKeys[Enable]].get<bool>();
	_limitAimedFireForgetGravityEnable = AimedFireForgetLG[MainJsonKeys[Enable]].get<bool>();

	//Speed
	constexpr auto Speed{ 6 };
	_aimedFireForgetSpeed = AimedFireForgetCS[MainJsonKeys[Speed]].get<float>();

	//Gravity
	constexpr auto Gravity{ 12 };
	_aimedFireForgetGravity = AimedFireForgetCG[MainJsonKeys[Gravity]].get<float>();

	//Min
	constexpr auto Min{ 8 };
	_limitAimedFireForgetSpeedMin = AimedFireForgetLS[MainJsonKeys[Min]].get<float>();
	_RandomizeAimedFireForgetSpeedMin = AimedFireForgetRS[MainJsonKeys[Min]].get<float>();
	_limitAimedFireForgetGravityMin = AimedFireForgetLG[MainJsonKeys[Min]].get<float>();

	//Max
	constexpr auto Max{ 9 };
	_limitAimedFireForgetSpeedMax = AimedFireForgetLS[MainJsonKeys[Max]].get<float>();
	_RandomizeAimedFireForgetSpeedMax = AimedFireForgetRS[MainJsonKeys[Max]].get<float>();
	_limitAimedFireForgetGravityMax = AimedFireForgetLG[MainJsonKeys[Max]].get<float>();

	//UserName
	_UserName = _MainJsonData["User Details"]["Username"].get<std::string>();
}

void DataManager::LogDataManagerContents()
{
	std::lock_guard<std::mutex> lock(_lock);
	constexpr const char*       StarredString{ "***************************************************************************************************" };
	logger::info("Aimed :");
	logger::info("{}", StarredString);
	logger::info("\tEnabled : {}", _enableAimed);
	logger::info("\tChange Speed : {}", _changeAimedFireForgetSpeedEnable);
	logger::info("\tNew Speed Value : {}", _aimedFireForgetSpeed);
	logger::info("\tChange Gravity : {}", _changeAimedFireForgetGravityEnable);
	logger::info("\tNew Gravity Value : {}", _aimedFireForgetSpeed);
	logger::info("\tLimit Speed : {}", _limitAimedFireForgetSpeedEnable);
	logger::info("\tSpeed Min : {}", _limitAimedFireForgetSpeedMin);
	logger::info("\tSpeed Max : {}", _limitAimedFireForgetSpeedMax);
	logger::info("\tRandomize Speed: {}", _RandomizeAimedFireForgetSpeedEnable);
	logger::info("\tRandomize Min : {}", _RandomizeAimedFireForgetSpeedMin);
	logger::info("\tRandomize Max : {}", _RandomizeAimedFireForgetSpeedMax);
	logger::info("\tLimit Gravity : {}", _limitAimedFireForgetGravityEnable);
	logger::info("\tGravity Min : {}", _limitAimedFireForgetGravityMin);
	logger::info("\tGravity Max : {}", _limitAimedFireForgetGravityMax);
	logger::info("{}", StarredString);
}
