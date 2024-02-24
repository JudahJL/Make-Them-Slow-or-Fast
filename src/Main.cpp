#include <filesystem>
#include <nlohmann/json.hpp>

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;
namespace fs = std::filesystem;
using json = nlohmann::json;

namespace MTSOF {
    const auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();

    class SpellItemVectorClass {  // used to store all spell data
    public:
        static SpellItemVectorClass* GetSingleton() {
            static SpellItemVectorClass singleton;
            return std::addressof(singleton);
        }

        void resetSpells(std::shared_ptr<spdlog::logger> r) {
            if (!spellItemFormIDVector.empty() && !spellItemGravityVector.empty() && !spellItemSpeedVector.empty()) {
                for (const auto* spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
                    if (spell && spell != nullptr) {
                        const auto* sES = spell->GetAVEffect();
                        if (sES && sES != nullptr) {
                            auto* sP = sES->data.projectileBase;
                            if (sP && sP != nullptr) {
                                for (int i = 0; std::cmp_less(i, spellItemFormIDVector.size()); i++) {
                                    if (spell->GetFormID() == spellItemFormIDVector[i]) {
                                        sP->data.speed = spellItemSpeedVector[i];
                                        sP->data.gravity = spellItemGravityVector[i];
                                        r->debug(" set spell with FormID : {:08X} speed = {} , gravity = {}", sP->GetFormID(), sP->data.speed, sP->data.gravity);
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        void loadVectors() {
            if (spellItemFormIDVector.empty() && spellItemGravityVector.empty() && spellItemSpeedVector.empty()) {
                for (const auto* spells : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
                    if (spells && spells != nullptr) {
                        const auto* spellEffectSettings = spells->GetAVEffect();
                        if (spellEffectSettings && spellEffectSettings != nullptr) {
                            const auto* spellProjectiles = spellEffectSettings->data.projectileBase;
                            if (spellProjectiles && spellProjectiles != nullptr) {
                                auto ak = spells->GetRawFormID();
                                spellItemFormIDVector.push_back(ak);
                                auto dj = spellProjectiles->data.gravity;
                                spellItemGravityVector.push_back(dj);
                                auto djs = spellProjectiles->data.speed;
                                spellItemSpeedVector.push_back(djs);
                            }
                        }
                    }
                }
                spellItemFormIDVector.shrink_to_fit();
                spellItemGravityVector.shrink_to_fit();
                spellItemSpeedVector.shrink_to_fit();
            }
        }

    private:
        std::vector<RE::FormID> spellItemFormIDVector;
        std::vector<float> spellItemGravityVector;
        std::vector<float> spellItemSpeedVector;
    };

    class MTSOFDataStorage {
    private:
        spdlog::level::level_enum logLevel = spdlog::level::trace;
        std::string logLevelStr;
        std::string folder_path = "Data/SKSE/Plugins/Make Them Slow or Fast/";
        json jsonData;   // used for main json file
        json mergeData;  // used to merge exclusion json files
        bool hasFilesToMerge = false;
        std::vector<std::string> formIDArray;
        std::vector<std::string> tesFileArray;
        std::string starredString = "***************************************************************************************************";
        bool enableAimed;
        bool changeAimedFireForgetSpeedEnable;
        float aimedFireForgetSpeed;
        bool limitAimedFireForgetSpeedEnable;
        float limitAimedFireForgetSpeedMin;
        float limitAimedFireForgetSpeedMax;

        bool changeAimedFireForgetGravityEnable;
        float aimedFireForgetGravity;
        bool limitAimedFireForgetGravityEnable;
        float limitAimedFireForgetGravityMin;
        float limitAimedFireForgetGravityMax;
        int keyForRefresh = 75;
        int keyForReset = 77;

    public:
        static MTSOFDataStorage* GetSingleton() {
            static MTSOFDataStorage singleton;
            return std::addressof(singleton);
        }

        const spdlog::level::level_enum getlogLevel() { return this->logLevel; }
        const std::string getlogLevelStr() { return this->logLevelStr; }
        const std::string getfolder_path() { return this->folder_path; }
        inline json getjsonData() { return this->jsonData; }
        inline json getmergeData() { return this->mergeData; }
        const bool gethasFilesToMerge() { return this->hasFilesToMerge; }
        inline std::vector<std::string> getformIDArray() { return this->formIDArray; }
        inline std::vector<std::string> gettesFileArray() { return this->tesFileArray; }
        const std::string getstarredString() { return this->starredString; }
        const bool getenableAimed() { return this->enableAimed; }
        const bool getchangeAimedFireForgetSpeedEnable() { return this->changeAimedFireForgetSpeedEnable; }
        const float getaimedFireForgetSpeed() { return this->aimedFireForgetSpeed; }
        const bool getlimitAimedFireForgetSpeedEnable() { return this->limitAimedFireForgetSpeedEnable; }
        const float getlimitAimedFireForgetSpeedMin() { return this->limitAimedFireForgetSpeedMin; }
        const float getlimitAimedFireForgetSpeedMax() { return this->limitAimedFireForgetSpeedMax; }
        const bool getchangeAimedFireForgetGravityEnable() { return this->changeAimedFireForgetGravityEnable; }
        const float getaimedFireForgetGravity() { return this->aimedFireForgetGravity; }
        const bool getlimitAimedFireForgetGravityEnable() { return this->limitAimedFireForgetGravityEnable; }
        const float getlimitAimedFireForgetGravityMin() { return this->limitAimedFireForgetGravityMin; }
        const float getlimitAimedFireForgetGravityMax() { return this->limitAimedFireForgetGravityMax; }
        const int getkeyForRefresh() { return this->keyForRefresh; }
        const int getkeyForReset() { return this->keyForReset; }

        void setlogLevel(spdlog::level::level_enum dlogLevel) { this->logLevel = dlogLevel; }
        void setlogLevelStr(const std::string& dlogLevelStr) { this->logLevelStr = dlogLevelStr; }
        void setjsonData(json djsonData) { this->jsonData = djsonData; }
        void setmergeData(json dmergeData) { this->mergeData = dmergeData; }
        void sethasFilesToMerge(bool dhasFilesToMerge) { this->hasFilesToMerge = dhasFilesToMerge; }
        void setformIDArray(std::vector<std::string> dformIDArray) { this->formIDArray = dformIDArray; }
        void settesFileArray(std::vector<std::string> dtesFileArray) { this->tesFileArray = dtesFileArray; }
        void setenableAimed(bool denableAimed) { this->enableAimed = denableAimed; }
        void setchangeAimedFireForgetSpeedEnable(bool dchangeAimedFireForgetSpeedEnable) { this->changeAimedFireForgetSpeedEnable = dchangeAimedFireForgetSpeedEnable; }
        void setaimedFireForgetSpeed(float daimedFireForgetSpeed) { this->aimedFireForgetSpeed = daimedFireForgetSpeed; }
        void setlimitAimedFireForgetSpeedEnable(float dlimitAimedFireForgetSpeedEnable) { this->limitAimedFireForgetSpeedEnable = dlimitAimedFireForgetSpeedEnable; }
        void setlimitAimedFireForgetSpeedMin(float dlimitAimedFireForgetSpeedMin) { this->limitAimedFireForgetSpeedMin = dlimitAimedFireForgetSpeedMin; }
        void setlimitAimedFireForgetSpeedMax(float dlimitAimedFireForgetSpeedMax) { this->limitAimedFireForgetSpeedMax = dlimitAimedFireForgetSpeedMax; }
        void setchangeAimedFireForgetGravityEnable(bool dchangeAimedFireForgetGravityEnable) { this->changeAimedFireForgetGravityEnable = dchangeAimedFireForgetGravityEnable; }
        void setaimedFireForgetGravity(float daimedFireForgetGravity) { this->aimedFireForgetGravity = daimedFireForgetGravity; }
        void setlimitAimedFireForgetGravityEnable(bool dlimitAimedFireForgetGravityEnable) { this->limitAimedFireForgetGravityEnable = dlimitAimedFireForgetGravityEnable; }
        void setlimitAimedFireForgetGravityMin(float dlimitAimedFireForgetGravityMin) { this->limitAimedFireForgetGravityMin = dlimitAimedFireForgetGravityMin; }
        void setlimitAimedFireForgetGravityMax(float dlimitAimedFireForgetGravityMax) { this->limitAimedFireForgetGravityMax = dlimitAimedFireForgetGravityMax; }
        void setkeyForRefresh(int dkeyForRefresh) { if (1 <= dkeyForRefresh && dkeyForRefresh <= 211) keyForRefresh = dkeyForRefresh; }
        void setkeyForReset(int dkeyForReset) {
            if (1 <= dkeyForReset && dkeyForReset <= 211)
                this->keyForReset = dkeyForReset;
            else
                this->keyForReset = 77;
        }

        void clearMergeData() { mergeData.clear(); }

        void insertDataIntoFormIDArrayAndTesFileArray() {
            // Collect all elements into formIDArray
            formIDArray.insert(formIDArray.end(), mergeData["SPELL FormID to Exclude"].begin(), mergeData["SPELL FormID to Exclude"].end());

            // Collect all elements into tesFileArray
            tesFileArray.insert(tesFileArray.end(), mergeData["SPELL File(s) to Exclude"].begin(), mergeData["Mod File(s) to Exclude"].end());
        }
        void sortAndRemoveDuplicatesFromFormIDArrayAndTesFileArray() {
            // Sort and remove duplicates from formIDArray
            std::sort(formIDArray.begin(), formIDArray.end());
            formIDArray.erase(std::unique(formIDArray.begin(), formIDArray.end()), formIDArray.end());

            // Sort and remove duplicates from tesFileArray
            std::sort(tesFileArray.begin(), tesFileArray.end());
            tesFileArray.erase(std::unique(tesFileArray.begin(), tesFileArray.end()), tesFileArray.end());
        }

        template <typename T, typename... Args>
        T getJsonDataValue(Args... args) {
            if constexpr (sizeof...(args) != 0) {
                json jsonObject = jsonData;
                ((jsonObject = jsonObject[args]), ...);

                if constexpr (std::is_same_v<T, bool>) {
                    if (jsonObject.is_boolean()) {
                        return jsonObject.get<bool>();
                    }
                } else if constexpr (std::is_same_v<T, int>) {
                    if (jsonObject.is_number()) {
                        return jsonObject.get<int>();
                    }
                } else if constexpr (std::is_same_v<T, float>) {
                    if (jsonObject.is_number()) {
                        return jsonObject.get<float>();
                    }
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (jsonObject.is_string()) {
                        return jsonObject.get<std::string>();
                    }
                }
                return T();
            }
        }


        auto getMergeDataArray(std::string args) { return mergeData[args]; }
    };
    
    class MTSOFMainClass {
    public:
        static MTSOFMainClass* GetSingleton() {
            static MTSOFMainClass singleton;
            return std::addressof(singleton);
        }

        MTSOFDataStorage* ds = MTSOFDataStorage::GetSingleton();
        SpellItemVectorClass* sivc = SpellItemVectorClass::GetSingleton();
        /**
         * Setup logging.
         *
         * <p>
         * Logging is important to track issues. CommonLibSSE bundles functionality for spdlog, a common C++ logging
         * framework. Here we initialize it, using values from the configuration file. This includes support for a debug
         * logger that shows output in your IDE when it has a debugger attached to Skyrim, as well as a file logger which
         * writes data to the standard SKSE logging directory at <code>Documents/My Games/Skyrim Special Edition/SKSE</code>
         * (or <code>Skyrim VR</code> if you are using VR).
         * </p>
         */
        void InitializeLogging() {
            std::ifstream jsonfile(std::format("Data/SKSE/Plugins/{}.json", PluginDeclaration::GetSingleton()->GetName()));

            try {
                ds->setjsonData(json::parse(jsonfile));  // used for parsing main json file
            } catch (const json::parse_error e) {
                std::string jsonDataString = R"({"Logging":{"LogLevel":"info"},"Aimed":{"Enable":true,"Fire and Forget":{"Change Speed":{"Enable":true,"Speed":1000.0},"Limit Speed":{"Enable":false,"Min":10.0,"Max":1000.0},"Change Gravity":{"Enable":false,"Gravity":1.0},"Limit Gravity":{"Enable":false,"Min":0.0,"Max":3.0}}},"Key for repatching spells":55,"Key for Reseting spells":74)";

                ds->setjsonData(json::parse(jsonDataString));
            }
            std::string data[2] = {"Logging", "LogLevel"};
            std::string lLS = ds->getJsonDataValue<std::string>( data[0], data[1]);  // logLevelStr abbrevated

            ds->setlogLevelStr(lLS);
            if (ds->getlogLevelStr() == "trace") {
                ds->setlogLevel(spdlog::level::trace);
            } else if (ds->getlogLevelStr() == "debug") {
                ds->setlogLevel(spdlog::level::debug);
            } else if (ds->getlogLevelStr() == "info") {
                ds->setlogLevel(spdlog::level::info);
            } else if (ds->getlogLevelStr() == "warn") {
                ds->setlogLevel(spdlog::level::warn);
            } else if (ds->getlogLevelStr() == "err") {
                ds->setlogLevel(spdlog::level::err);
            } else if (ds->getlogLevelStr() == "critical") {
                ds->setlogLevel(spdlog::level::critical);
            } else {
                ds->setlogLevel(spdlog::level::info);
            }

            auto path = log_directory();
            if (!path) {
                report_and_fail("Unable to lookup SKSE logs directory.");
            }
            *path /= std::format("{}.log", PluginDeclaration::GetSingleton()->GetName());

            std::shared_ptr<spdlog::logger> log;
            if (IsDebuggerPresent()) {
                log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
            } else {
                log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
            }

            log->set_level(ds->getlogLevel());
            log->flush_on(ds->getlogLevel());
            set_default_logger(std::move(log));
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");

            std::optional<fs::path> customPath(ds->getfolder_path());

            if (!customPath) {
                fs::create_directory(ds->getfolder_path());
            }
            *customPath /= std::format("{}.log", plugin->GetName());

            std::shared_ptr<spdlog::logger> refreshLogger = std::make_shared<spdlog::logger>("refreshLogger");

            if (IsDebuggerPresent()) {
                refreshLogger->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
            } else {
                refreshLogger->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(customPath->string(), true));
            }

            refreshLogger->set_level(ds->getlogLevel());
            refreshLogger->flush_on(ds->getlogLevel());
            refreshLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
            spdlog::register_logger(refreshLogger);
            if (!ds->getjsonData().empty()) ds->getjsonData().clear();
        }

        inline float limitFloat(float value, float min_value, float max_value) { return (value < min_value) ? min_value : ((value > max_value) ? max_value : value); }

        RE::FormID GetFormIDFromIdentifier(const std::string& identifier) {
            RE::TESDataHandler* dataHandler = RE::TESDataHandler::GetSingleton();
            auto delimiter = identifier.find('|');
            if (delimiter != std::string::npos) {
                std::string modName = identifier.substr(0, delimiter);
                std::string modForm = identifier.substr(delimiter + 1);
                uint32_t formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
                const RE::TESFile* mod = (RE::TESFile*)dataHandler->LookupModByName(modName.c_str());
                if (mod && mod != nullptr) {
                    if (mod->IsLight()) formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
                    return dataHandler->LookupForm(formID, modName.c_str())->GetFormID();
                }
            }
            return (RE::FormID) nullptr;
        }

        void spellPatch(bool calledAtKDataLoaded, std::shared_ptr<spdlog::logger> refresh) {
            if (ds->getenableAimed()) {
                for (const auto* spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
                    if (spell && spell != nullptr) {
                        auto* spellEffectSetting = spell->GetAVEffect();
                        if (spellEffectSetting && spellEffectSetting != nullptr) {
                            auto* spellProjectile = spellEffectSetting->data.projectileBase;
                            if (spellProjectile && spellProjectile != nullptr) {
                                bool shouldPatch = true;
                                if (ds->gethasFilesToMerge()) {
                                    for (const std::string spellModName : ds->gettesFileArray()) {
                                        if (spellModName.c_str() == spell->GetFile()->GetFilename()) {
                                            shouldPatch = false;
                                            std::string data =
                                                std::format("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), spell->GetRawFormID(),
                                                            spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
                                            if (calledAtKDataLoaded) {
                                                debug("{}", ds->getstarredString());
                                                debug("{}", data);
                                                debug("{}", ds->getstarredString());
                                            } else {
                                                refresh->debug("{}", ds->getstarredString());
                                                refresh->debug("{}", data);
                                                refresh->debug("{}", ds->getstarredString());
                                            }
                                            break;
                                        }
                                    }
                                    if (shouldPatch) {
                                        for (const std::string spellFormID : ds->getformIDArray()) {
                                            auto formID = GetFormIDFromIdentifier(spellFormID);
                                            if (formID && spell->GetFormID() == formID) {
                                                shouldPatch = false;
                                                std::string data = std::format("Skipping Spell : Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(), formID,
                                                                               spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
                                                if (calledAtKDataLoaded) {
                                                    debug("{}", ds->getstarredString());
                                                    debug("{}", data);
                                                    debug("{}", ds->getstarredString());
                                                } else {
                                                    refresh->debug("{}", ds->getstarredString());
                                                    refresh->debug("{}", data);
                                                    refresh->debug("{}", ds->getstarredString());
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
                                        if (ds->getchangeAimedFireForgetSpeedEnable() || ds->getlimitAimedFireForgetSpeedEnable() || ds->getchangeAimedFireForgetGravityEnable() || ds->getlimitAimedFireForgetGravityEnable()) ammoPatched = true;
                                        if ((ds->getenableAimed()) && ammoPatched) {
                                            std::string data =
                                                std::format("Original Aimed,Fire And Forget Spell :Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
                                                            spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
                                            if (calledAtKDataLoaded) {
                                                debug("{}", ds->getstarredString());
                                                debug("{}", data);
                                            } else {
                                                refresh->debug("{}", ds->getstarredString());
                                                refresh->debug("{}", data);
                                            }
                                        }
                                        if ((delivery == RE::MagicSystem::Delivery::kAimed) && (casttype == RE::MagicSystem::CastingType::kFireAndForget)) {
                                            if (ds->getchangeAimedFireForgetSpeedEnable()) {
                                                spellProjectile->data.speed = ds->getaimedFireForgetSpeed();
                                                std::string data = "modified speed";
                                                if (calledAtKDataLoaded) {
                                                    debug("{}", data);
                                                } else {
                                                    refresh->debug("{}", data);
                                                }
                                            }
                                            if (ds->getlimitAimedFireForgetSpeedEnable()) {
                                                spellProjectile->data.speed = limitFloat(spellProjectile->data.speed, ds->getlimitAimedFireForgetSpeedMin(), ds->getlimitAimedFireForgetSpeedMax());
                                                std::string data = "limited speed";
                                                if (calledAtKDataLoaded) {
                                                    debug("{}", data);
                                                } else {
                                                    refresh->debug("{}", data);
                                                }
                                            }
                                            if (ds->getchangeAimedFireForgetGravityEnable()) {
                                                spellProjectile->data.gravity = ds->getaimedFireForgetGravity();
                                                std::string data = "modified gravity";
                                                if (calledAtKDataLoaded) {
                                                    debug("{}", data);
                                                } else {
                                                    refresh->debug("{}", data);
                                                }
                                            }
                                            if (ds->getlimitAimedFireForgetGravityEnable()) {
                                                spellProjectile->data.gravity = limitFloat(spellProjectile->data.gravity, ds->getlimitAimedFireForgetGravityMin(), ds->getlimitAimedFireForgetGravityMax());
                                                std::string data = "limited gravity";
                                                if (calledAtKDataLoaded) {
                                                    debug("{}", data);
                                                } else {
                                                    refresh->debug("{}", data);
                                                }
                                            }
                                        }
                                        if ((ds->getenableAimed()) && ammoPatched) {
                                            std::string data =
                                                std::format("Modified Aimed,Fire And Forget Spell :Full Name:{}|FormID:{:08X}|Projectile Name:{}|Projectile FormID:{:08X}|Projectile Speed:{}|Projectile Gravity:{}|File:{}", spell->GetFullName(),
                                                            spell->GetRawFormID(), spellProjectile->GetFullName(), spellProjectile->GetRawFormID(), spellProjectile->data.speed, spellProjectile->data.gravity, spell->GetFile()->GetFilename());
                                            if (calledAtKDataLoaded) {
                                                debug("{}", data);
                                                debug("{}", ds->getstarredString());
                                            } else {
                                                refresh->debug("{}", data);
                                                refresh->debug("{}", ds->getstarredString());
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            std::string data = std::format("{} {} Finished Patching.", plugin->GetName(), version.string());
            if (calledAtKDataLoaded)
                info("{}", data);
            else
                refresh->info("{}", data);
            if (!ds->getjsonData().empty()) ds->getjsonData().clear();
            if (!ds->getformIDArray().empty()) ds->getformIDArray().clear();
            if (!ds->gettesFileArray().empty()) ds->gettesFileArray().clear();
        }

        void LoadJSON(bool calledAtRuntime, std::shared_ptr<spdlog::logger> refresh) {
            std::ifstream jsonfile(std::format("Data/SKSE/Plugins/{}.json", PluginDeclaration::GetSingleton()->GetName()));

            try {
                ds->setjsonData(json::parse(jsonfile));  // used for parsing main json file
            } catch (const json::parse_error e) {
                std::string jsonDataString = R"({"Logging":{"LogLevel":"info"},"Aimed":{"Enable":true,"Fire and Forget":{"Change Speed":{"Enable":true,"Speed":1000.0},"Limit Speed":{"Enable":false,"Min":10.0,"Max":1000.0},"Change Gravity":{"Enable":false,"Gravity":1.0},"Limit Gravity":{"Enable":false,"Min":0.0,"Max":3.0}}},"Key for repatching spells":55,"Key for Reseting spells":74)";

                ds->setjsonData(json::parse(jsonDataString));
                std::string data[3] = {std::format("{}", e.what()), std::format("Data/SKSE/Plugins/{}.json parsing error : {}", SKSE::PluginDeclaration::GetSingleton()->GetName(), e.what()),
                                       std::format("loading default {}.json", SKSE::PluginDeclaration::GetSingleton()->GetName())};
                if (calledAtRuntime) {
                    error("{}", data[0]);
                    error("{}", data[1]);
                    error("{}", data[2]);
                } else {
                    refresh->error("{}", data[0]);
                    refresh->error("{}", data[1]);
                    refresh->error("{}", data[2]);
                }
            }
            std::string stringData[13] = {"Aimed", "Enable", "Fire and Forget", "Change Speed", "Speed", "Limit Speed", "Min", "Max", "Change Gravity", "Gravity", "Limit Gravity", "Key for Reseting spells", "Key for repatching spells"};
            ds->setenableAimed(ds->getJsonDataValue<bool>(stringData[0],stringData[1]));
            ds->setchangeAimedFireForgetSpeedEnable(ds->getJsonDataValue<bool>(stringData[0],stringData[2],stringData[3],stringData[1]));
            ds->setaimedFireForgetSpeed(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[3],stringData[4]));
            ds->setlimitAimedFireForgetSpeedEnable(ds->getJsonDataValue<bool>(stringData[0],stringData[2],stringData[5],stringData[1]));
            ds->setlimitAimedFireForgetSpeedMin(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[5],stringData[6]));
            ds->setlimitAimedFireForgetSpeedMax(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[5],stringData[7]));

            ds->setchangeAimedFireForgetGravityEnable(ds->getJsonDataValue<bool>(stringData[0],stringData[2],stringData[8],stringData[1]));
            ds->setaimedFireForgetGravity(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[8],stringData[9]));
            ds->setlimitAimedFireForgetGravityEnable(ds->getJsonDataValue<bool>(stringData[0],stringData[2],stringData[10],stringData[1]));
            ds->setlimitAimedFireForgetGravityMin(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[10],stringData[6]));
            ds->setlimitAimedFireForgetGravityMax(ds->getJsonDataValue<float>(stringData[0],stringData[2],stringData[10],stringData[7]));
            ds->setkeyForRefresh(ds->getJsonDataValue<int>(stringData[12]));
            ds->setkeyForReset(ds->getJsonDataValue<int>(stringData[11]));


            if (fs::exists(ds->getfolder_path()) && !fs::is_empty(ds->getfolder_path())) {
                ds->sethasFilesToMerge(true);
                for (const auto& entry : fs::directory_iterator(ds->getfolder_path())) {
                    if (entry.path().extension() == ".json") {
                        std::ifstream jFile(entry.path());
                        try {
                            ds->setmergeData(json::parse(jFile));
                        } catch (const json::parse_error e) {
                            if (calledAtRuntime) {
                                error("{} parsing error : {}", entry.path().generic_string(), e.what());
                                error("If you get this error, check your {}. The line above will tell where the mistake is.", entry.path().generic_string());
                            } else {
                                refresh->error("{} parsing error : {}", entry.path().generic_string(), e.what());
                                refresh->error("If you get this error, check your {}. The line above will tell where the mistake is.", entry.path().generic_string());
                            }
                        }
                        std::string data = std::format("Loaded JSON from file: {}", entry.path().generic_string());
                        if (calledAtRuntime)
                            debug("{}", data);
                        else
                            refresh->debug("{}", data);

                        ds->insertDataIntoFormIDArrayAndTesFileArray();
                        ds->clearMergeData();
                    }
                }
            }

            ds->sortAndRemoveDuplicatesFromFormIDArrayAndTesFileArray();

            // debug("formIDArray : {}",formIDArray.dump(4));
            // debug("no of items in formIDArray : {}",formIDArray.size());
            // debug("tesFileArray : {}",tesFileArray.dump(4));
            // debug("no of items in tesFileArray : {}",tesFileArray.size());
            std::string data[2] = {std::format("************************************No Exclusion will be Done**************************************"),
                                   std::format("*************************************Finished Processing Data**************************************")};
            if (!(ds->gethasFilesToMerge())) {
                if (calledAtRuntime)
                    info("{}", data[0]);
                else
                    refresh->info("{}", data[0]);
            }

            if (calledAtRuntime)
                info("{}", data[1]);
            else
                refresh->info("{}", data[1]);
        }
    };

    class InputHandler : public RE::BSTEventSink<RE::InputEvent*> {
    public:
        static InputHandler* GetSingleton() {
            static InputHandler singleton;
            return std::addressof(singleton);
        }

        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* e, RE::BSTEventSource<RE::InputEvent*>*) override {
            if (!*e) return RE::BSEventNotifyControl::kContinue;
            
            if (auto buttonEvent = (*e)->AsButtonEvent(); buttonEvent && buttonEvent->HasIDCode() && (buttonEvent->IsDown() || buttonEvent->IsPressed())) {
                if (int key = buttonEvent->GetIDCode(); key == MTSOFDataStorage::GetSingleton()->getkeyForRefresh()) {
                    MTSOFMainClass::GetSingleton()->LoadJSON(false, refresh);
                    MTSOFMainClass::GetSingleton()->spellPatch(false, refresh);
                } else if (key == MTSOFDataStorage::GetSingleton()->getkeyForReset()) {
                    SpellItemVectorClass::GetSingleton()->resetSpells(refresh);
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        }

        void enable() {
            if (auto input = RE::BSInputDeviceManager::GetSingleton()) {
                input->AddEventSink(this);
            }
        }

    private:
        std::shared_ptr<spdlog::logger> refresh = spdlog::get("refreshLogger");
    };

    void InitializeMessaging() {
        if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
                if (message->type == MessagingInterface::kDataLoaded) {
                    auto refresh = spdlog::get("refreshLogger");
                    SpellItemVectorClass::GetSingleton()->loadVectors();
                    MTSOFMainClass::GetSingleton()->spellPatch(true, refresh);
                    InputHandler::GetSingleton()->enable();
                }
            })) {
            stl::report_and_fail("Unable to register message listener.");
        }
    }
}  // namespace MTSOF

SKSEPluginLoad(const LoadInterface* skse) {
    MTSOF::MTSOFMainClass::GetSingleton()->InitializeLogging();

    info("{} {} is loading...", MTSOF::plugin->GetName(), MTSOF::version);
    auto refresh = spdlog::get("refreshLogger");
    if (refresh != nullptr) MTSOF::MTSOFMainClass::GetSingleton()->LoadJSON(true, refresh);
    Init(skse);
    MTSOF::InitializeMessaging();
    return true;
}