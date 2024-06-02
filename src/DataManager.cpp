#include "DataManager.h"
#include "utils.h"

MTSOF::DataManager* MTSOF::DataManager::GetSingleton()
{
	static DataManager Singleton;
	return std::addressof(Singleton);
}

void MTSOF::DataManager::LoadJson(bool calledAtRuntime, std::shared_ptr<spdlog::logger> refresh)
{
	std::ifstream jsonfile(std::format("Data/SKSE/Plugins/{}.json", SKSE::PluginDeclaration::GetSingleton()->GetName()));

	try {
		_jsonData = nJson::parse(jsonfile);
	} catch (const nJson::parse_error e) {
		std::string data[2]{ std::format("Data/SKSE/Plugins/{}.json parsing error : {}", SKSE::PluginDeclaration::GetSingleton()->GetName(), e.what()),
			std::format("loading default {}.json", SKSE::PluginDeclaration::GetSingleton()->GetName()) };

		if (calledAtRuntime) {
			logger::error("{}", data[0]);
			logger::error("{}", data[1]);
		} else {
			refresh->error("{}", data[0]);
			refresh->error("{}", data[1]);
		}
		Utils::customMessageBox(std::format("{}. Are you sure you want to continue? if you continue, The error will be logged and a default json will be used instead.", e.what()));
		_jsonData = R"({"Logging":{"LogLevel":"info"},"Aimed":{"Enable":true,"Fire and Forget":{"Change Speed":{"Enable":true,"Speed":1000.0},"Limit Speed":{"Enable":false,"Min":10.0,"Max":1000.0},"Change Gravity":{"Enable":false,"Gravity":1.0},"Limit Gravity":{"Enable":false,"Min":0.0,"Max":3.0}}},"Redo":{"Enable":false,"Key for repatching spells":75,"Key for Reseting spells":77}})"_json;
	}
	const char* stringData[16]{ "Aimed", "Enable", "Fire and Forget", "Change Speed", "Speed", "Limit Speed", "Min", "Max", "Change Gravity", "Gravity", "Limit Gravity", "Key for Reseting spells", "Key for repatching spells", "Redo", "Logging", "LogLevel" };
	try {
		_enableAimed = _jsonData[stringData[0]][stringData[1]].get<bool>();
		_changeAimedFireForgetSpeedEnable = _jsonData[stringData[0]][stringData[2]][stringData[3]][stringData[1]].get<bool>();
		_aimedFireForgetSpeed = _jsonData[stringData[0]][stringData[2]][stringData[3]][stringData[4]].get<float>();
		_limitAimedFireForgetSpeedEnable = _jsonData[stringData[0]][stringData[2]][stringData[5]][stringData[1]].get<bool>();
		_limitAimedFireForgetSpeedMin = _jsonData[stringData[0]][stringData[2]][stringData[5]][stringData[6]].get<float>();
		_limitAimedFireForgetSpeedMax = _jsonData[stringData[0]][stringData[2]][stringData[5]][stringData[7]].get<float>();

		_changeAimedFireForgetGravityEnable = _jsonData[stringData[0]][stringData[2]][stringData[8]][stringData[1]].get<bool>();
		_aimedFireForgetGravity = _jsonData[stringData[0]][stringData[2]][stringData[8]][stringData[9]].get<float>();
		_limitAimedFireForgetGravityEnable = _jsonData[stringData[0]][stringData[2]][stringData[10]][stringData[1]].get<bool>();
		_limitAimedFireForgetGravityMin = _jsonData[stringData[0]][stringData[2]][stringData[10]][stringData[6]].get<float>();
		_limitAimedFireForgetGravityMax = _jsonData[stringData[0]][stringData[2]][stringData[10]][stringData[7]].get<float>();
		_redo = _jsonData[stringData[13]][stringData[1]].get<bool>();
		_keyForRefresh = _jsonData[stringData[13]][stringData[12]].get<int>();
		_keyForReset = _jsonData[stringData[13]][stringData[11]].get<int>();
		_logLevelStr = _jsonData[stringData[14]][stringData[15]].get<std::string>();

		if (_logLevelStr == "trace")
			_logLevel = spdlog::level::trace;
		else if (_logLevelStr == "debug")
			_logLevel = spdlog::level::debug;
		else if (_logLevelStr == "info")
			_logLevel = spdlog::level::info;
		else if (_logLevelStr == "warn")
			_logLevel = spdlog::level::warn;
		else if (_logLevelStr == "err")
			_logLevel = spdlog::level::err;
		else if (_logLevelStr == "critical")
			_logLevel = spdlog::level::critical;
		else
			_logLevel = spdlog::level::info;

		auto defaultLogger = spdlog::default_logger();
		defaultLogger->set_level(_logLevel);
		defaultLogger->flush_on(_logLevel);
		refresh->set_level(_logLevel);
		refresh->flush_on(_logLevel);


		if (fs::exists(_folder_path) && !fs::is_empty(_folder_path)) {
			_hasFilesToMerge = true;
			for (const auto& entry : fs::directory_iterator(_folder_path)) {
				if (entry.path().extension() == ".json") {
					std::ifstream jFile(entry.path());
					try {
						_mergeData = nJson::parse(jFile);
					} catch (const nJson::parse_error e) {
						{
							Utils::customMessageBox(std::format("{}. Are you sure you want to continue? if you continue, {} will be ignored and the error will be logged.", e.what(), entry.path().generic_string()));
							std::string data[2]{ std::format("{} parsing error : {}", entry.path().generic_string(), e.what()), std::format("If you get this error, check your {}. The line above will tell where the mistake is.", entry.path().generic_string()) };
							if (calledAtRuntime) {
								logger::error("{}", data[0]);
								logger::error("{}", data[1]);
							} else {
								refresh->error("{}", data[0]);
								refresh->error("{}", data[1]);
							}
						}
					}
					std::string data = std::format("Loaded JSON from file: {}", entry.path().generic_string());
					if (calledAtRuntime)
						logger::debug("{}", data);
					else
						refresh->debug("{}", data);

					const char* data1{ "SPELL FormID to Exclude" };
					const char* data2{ "SPELL File(s) to Exclude" };
					_formIDArray.insert(_formIDArray.end(), _mergeData[data1].begin(), _mergeData[data1].end());
					_tesFileArray.insert(_tesFileArray.end(), _mergeData[data2].begin(), _mergeData[data2].end());
				}
			}
		}

		if (_hasFilesToMerge) {
			std::sort(_formIDArray.begin(), _formIDArray.end());
			_formIDArray.erase(std::unique(_formIDArray.begin(), _formIDArray.end()), _formIDArray.end());
			std::sort(_tesFileArray.begin(), _tesFileArray.end());
			_tesFileArray.erase(std::unique(_tesFileArray.begin(), _tesFileArray.end()), _tesFileArray.end());
		}

	}
	catch (const nJson::exception& e) {
		if (calledAtRuntime)
			logger::error("{}", e.what());
		else
			refresh->error("{}", e.what());
		Utils::customMessageBox(std::format("{}. Want to Continue?", e.what()));
	}

	std::string data[17]{
		std::format("************************************No Exclusion will be Done**************************************"),  //0
		std::format("*************************************Finished Processing Data**************************************"),  //1
		std::format("Aimed :"),                                                                                              //2
		std::format("\tEnabled : {} ;this is a ultimate kill switch for all aimed spells", _enableAimed),                    //3
		std::format("\tChange Speed : {}", _changeAimedFireForgetSpeedEnable),                                               //4
		std::format("\tNew Speed Value : {}", _aimedFireForgetSpeed),                                                        //5
		std::format("\tChange Gravity : {}", _changeAimedFireForgetGravityEnable),                                           //6
		std::format("\tNew Gravity Value : {}", _aimedFireForgetSpeed),                                                      //7
		std::format("\tLimit Speed : {}", _limitAimedFireForgetSpeedEnable),                                                 //8
		std::format("\tSpeed Min : {}", _limitAimedFireForgetSpeedMin),                                                      //9
		std::format("\tSpeed Max : {}", _limitAimedFireForgetSpeedMax),                                                      //10
		std::format("\tLimit Gravity : {}", _limitAimedFireForgetGravityEnable),                                             //11
		std::format("\tGravity Min : {}", _limitAimedFireForgetGravityMin),                                                  //12
		std::format("\tGravity Max : {}", _limitAimedFireForgetGravityMax),                                                  //13
		std::format("\tKey for Repatching spells : {}", _keyForRefresh),                                                     //14
		std::format("\tKey for reloading original values : {}", _keyForReset),                                               //15
		std::format("\tEnable Redo : {}", _redo)
	};
	if (!(_hasFilesToMerge)) {
		if (calledAtRuntime)
			logger::info("{}", data[0]);
		else
			refresh->info("{}", data[0]);
	}

	if (calledAtRuntime) {
		if (!(_hasFilesToMerge))
			logger::info("{}", data[0]);
		logger::info("{}", data[1]);
		logger::info("{}", _starredString);
		logger::info("{}", data[2]);
		logger::info("{}", data[3]);
		logger::info("{}", data[4]);
		logger::info("{}", data[5]);
		logger::info("{}", data[8]);
		logger::info("{}", data[9]);
		logger::info("{}", data[10]);
		logger::info("{}", data[6]);
		logger::info("{}", data[7]);
		logger::info("{}", data[11]);
		logger::info("{}", data[12]);
		logger::info("{}", data[13]);
		logger::info("{}", data[16]);
		logger::info("{}", data[14]);
		logger::info("{}", data[15]);
		logger::info("{}", _starredString);
	} else {
		if (!(_hasFilesToMerge))
			refresh->info("{}", data[0]);
		refresh->info("{}", data[1]);
		refresh->info("{}", _starredString);
		refresh->info("{}", data[2]);
		refresh->info("{}", data[3]);
		refresh->info("{}", data[4]);
		refresh->info("{}", data[5]);
		refresh->info("{}", data[8]);
		refresh->info("{}", data[9]);
		refresh->info("{}", data[10]);
		refresh->info("{}", data[6]);
		refresh->info("{}", data[7]);
		refresh->info("{}", data[11]);
		refresh->info("{}", data[12]);
		refresh->info("{}", data[13]);
		refresh->info("{}", data[16]);
		refresh->info("{}", data[14]);
		refresh->info("{}", data[15]);
		refresh->info("{}", _starredString);
	}
}

void MTSOF::DataManager::spellPatch(bool calledAtKDataLoaded, std::shared_ptr<spdlog::logger> refresh)
{
	auto startSP = std::chrono::high_resolution_clock::now();
	if (_enableAimed) {
		for (const auto* spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
			if (spell && spell != nullptr) {
				auto* spellEffectSetting = spell->GetAVEffect();
				if (spellEffectSetting && spellEffectSetting != nullptr) {
					auto* spellProjectile = spellEffectSetting->data.projectileBase;
					if (spellProjectile && spellProjectile != nullptr) {
						bool shouldPatch = true;
						if (_hasFilesToMerge) {
							for (const std::string spellModName : _tesFileArray) {
								if (spellModName.c_str() == spell->GetFile()->GetFilename()) {
									shouldPatch = false;
									std::string data =
										std::format("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), spell->GetRawFormID(),
											spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
									if (calledAtKDataLoaded) {
										logger::debug("{}", _starredString);
										logger::debug("{}", data);
										logger::debug("{}", _starredString);
									} else {
										refresh->debug("{}", _starredString);
										refresh->debug("{}", data);
										refresh->debug("{}", _starredString);
									}
									break;
								}
							}
							if (shouldPatch) {
								for (const std::string spellFormID : _formIDArray) {
									auto formID = InlineUtils::GetFormIDFromIdentifier(spellFormID);
									if (formID && spell->GetFormID() == formID) {
										shouldPatch = false;
										std::string data = std::format("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), formID,
											spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
										if (calledAtKDataLoaded) {
											logger::debug("{}", _starredString);
											logger::debug("{}", data);
											logger::debug("{}", _starredString);
										} else {
											refresh->debug("{}", _starredString);
											refresh->debug("{}", data);
											refresh->debug("{}", _starredString);
										}
										break;
									}
								}
							}
						}

						if (shouldPatch) {
							if (spell && (spell->data.spellType == RE::MagicSystem::SpellType::kSpell)) {
								auto delivery = spell->data.delivery;
								auto casttype = spell->data.castingType;
								bool ammoPatched = false;
								if (_changeAimedFireForgetSpeedEnable || _limitAimedFireForgetSpeedEnable || _changeAimedFireForgetGravityEnable || _limitAimedFireForgetGravityEnable)
									ammoPatched = true;
								if ((delivery == RE::MagicSystem::Delivery::kAimed) && (casttype == RE::MagicSystem::CastingType::kFireAndForget)) {
									if ((_enableAimed) && ammoPatched) {
										std::string data =
											std::format("Original Aimed,Fire And Forget Spell : Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
												spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
										if (calledAtKDataLoaded) {
											logger::debug("{}", _starredString);
											logger::debug("{}", data);
										} else {
											refresh->debug("{}", _starredString);
											refresh->debug("{}", data);
										}
									}
									if (_changeAimedFireForgetSpeedEnable) {
										spellProjectile->data.speed = _aimedFireForgetSpeed;
										const char* data{ "modified speed" };
										if (calledAtKDataLoaded) {
											logger::debug("{}", data);
										} else {
											refresh->debug("{}", data);
										}
									}
									if (_limitAimedFireForgetSpeedEnable) {
										spellProjectile->data.speed = InlineUtils::limitFloat(spellProjectile->data.speed, _limitAimedFireForgetSpeedMin, _limitAimedFireForgetSpeedMax);
										const char* data{ "limited speed" };
										if (calledAtKDataLoaded) {
											logger::debug("{}", data);
										} else {
											refresh->debug("{}", data);
										}
									}
									if (_changeAimedFireForgetGravityEnable) {
										spellProjectile->data.gravity = _aimedFireForgetGravity;
										const char* data{ "modified gravity" };
										if (calledAtKDataLoaded) {
											logger::debug("{}", data);
										} else {
											refresh->debug("{}", data);
										}
									}
									if (_limitAimedFireForgetGravityEnable) {
										spellProjectile->data.gravity = InlineUtils::limitFloat(spellProjectile->data.gravity, _limitAimedFireForgetGravityMin, _limitAimedFireForgetGravityMax);
										const char* data{ "limited gravity" };
										if (calledAtKDataLoaded) {
											logger::debug("{}", data);
										} else {
											refresh->debug("{}", data);
										}
									}
									if ((_enableAimed) && ammoPatched) {
										std::string data =
											std::format("Modified Aimed,Fire And Forget Spell : Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
												spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
										if (calledAtKDataLoaded) {
											logger::debug("{}", data);
											logger::debug("{}", _starredString);
										} else {
											refresh->debug("{}", data);
											refresh->debug("{}", _starredString);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	std::string data = std::format("{} {} Finished Patching.", SKSE::PluginDeclaration::GetSingleton()->GetName(), SKSE::PluginDeclaration::GetSingleton()->GetVersion().string());
	if (calledAtKDataLoaded)
		logger::info("{}", data);
	else
		refresh->info("{}", data);

	#ifdef NDEBUG
	if (calledAtKDataLoaded) {
		for (decltype(_formIDArray.size()) i = 0; i <_formIDArray.size();i++)
			logger::debug("_formIDArray[{}] : {}", i, _formIDArray[i]);
		for (decltype(_tesFileArray.size()) i = 0; i < _tesFileArray.size(); i++)
			logger::debug("_tesFileArray[{}] : {}", i, _tesFileArray[i]);
	} else {
		for (decltype(_formIDArray.size()) i = 0; i < _formIDArray.size(); i++)
			refresh->debug("_formIDArray[{}] : {}", i, _formIDArray[i]);
		for (decltype(_tesFileArray.size()) i = 0; i < _tesFileArray.size(); i++)
			refresh->debug("_tesFileArray[{}] : {}", i, _tesFileArray[i]);
	}
	#endif

	if (!_jsonData.empty())
		_jsonData.clear();
	if (!_formIDArray.empty())
		_formIDArray.clear();
	if (!_tesFileArray.empty())
		_tesFileArray.clear();
	auto nanosecondsTakenForSP = std::chrono::duration(std::chrono::high_resolution_clock::now() - startSP);
	{
		std::string data1{ std::format("Time Taken in ammo_patch() totally is {} nanoseconds or {} microseconds or {} milliseconds or {} seconds or {} minutes", nanosecondsTakenForSP.count(),
			std::chrono::duration_cast<std::chrono::microseconds>(nanosecondsTakenForSP).count(), std::chrono::duration_cast<std::chrono::milliseconds>(nanosecondsTakenForSP).count(),
			std::chrono::duration_cast<std::chrono::seconds>(nanosecondsTakenForSP).count(), std::chrono::duration_cast<std::chrono::minutes>(nanosecondsTakenForSP).count()) };
		if (calledAtKDataLoaded)
			logger::info("{}", data1);
		else
			refresh->info("{}", data1);
	}
}
