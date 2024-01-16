#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;
namespace fs = std::filesystem;

namespace MTSOF {
    using json = nlohmann::json;

    const auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    spdlog::level::level_enum logLevel = spdlog::level::trace;
    std::string logLevelStr;
    bool changeaimedConcentration = false;
    float aimedConcentrationSpeed;
    bool changeaimedFireAndForget = false;
    float aimedFireAndForgetSpeed;
    std::string folder_path = "Data/SKSE/Plugins/Make Them Slow or Fast/";
    json json_data;         //used for parsing main json file first and then processing for merging json files
    json merge_data;        //used to merge json file
    json use_data;          //used to actually use merged json
    bool whereDiditCrash;
    bool hasFilesToMerge = true;

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
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= std::format("{}.log", PluginDeclaration::GetSingleton()->GetName());

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }

        log->set_level(logLevel);
        log->flush_on(logLevel);
        set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    }

    RE::TESForm* GetFormFromIdentifier(const std::string& identifier) {
    	auto dataHandler = RE::TESDataHandler::GetSingleton();
    	auto delimiter = identifier.find('|');
    	if (delimiter != std::string::npos) {
	    	std::string modName = identifier.substr(0, delimiter);
		    std::string modForm = identifier.substr(delimiter + 1);
    		uint32_t formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
	    	auto* mod = dataHandler->LookupModByName(modName.c_str());
		    if (mod->IsLight()) formID = std::stoul(modForm, nullptr, 16) & 0xFFF;
		    return dataHandler->LookupForm(formID, modName.c_str());
	    }
	    return nullptr;
    }

    void spellPatch() {
        log::info("{} {} Got Everything It Needs. Will Start Patching Now.", plugin->GetName(),version);
        
        for (const auto spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
            if(whereDiditCrash) log::debug("{},{:08X},{},{}",spell->GetFile()->GetFilename(),spell->GetFormID(),spell->GetFullName(),spell->GetFormEditorID());
            bool shouldPatch = true;
            if (hasFilesToMerge) {
                for (std::string spellFormID : use_data["Spell FormID to Exclude"]) {
                    if (auto form = GetFormFromIdentifier(spellFormID)) {
                        if (spell->GetFormID() == form->GetFormID()) {
                            shouldPatch = false;
                            break;
                        }
                    }
                }
            }

            if(shouldPatch){
                    if (spell && (spell->data.spellType == RE::MagicSystem::SpellType::kSpell)) {
                        auto delivery = spell->data.delivery;
                        auto casttype = spell->data.castingType;
                        if (delivery == RE::MagicSystem::Delivery::kAimed &&
                            casttype == RE::MagicSystem::CastingType::kConcentration && changeaimedConcentration &&
                            spell->GetAVEffect() && (!(spell->GetAVEffect() == nullptr)) &&
                            spell->GetAVEffect()->data.projectileBase &&
                            ((spell->GetAVEffect()->data.projectileBase == nullptr))) {
                            log::debug("********************************************************************************************************************");
                            log::debug("Unchanged,Aimed,Concentrated:Spell Full name:{}|Spell FormID:{:08X}|Spell Speed:{}",spell->GetFullName(),spell->GetFormID(),spell->GetAVEffect()->data.projectileBase->data.speed);
                            spell->GetAVEffect()->data.projectileBase->data.speed = aimedConcentrationSpeed;
                            log::debug("Changed,Aimed,Concentrated:Spell Full name:{}|Spell FormID:{:08X}|Spell Speed:{}",spell->GetFullName(),spell->GetFormID(),spell->GetAVEffect()->data.projectileBase->data.speed);
                            log::debug("********************************************************************************************************************");
                        }
                        if (delivery == RE::MagicSystem::Delivery::kAimed &&
                            casttype == RE::MagicSystem::CastingType::kFireAndForget && changeaimedFireAndForget &&
                            spell->GetAVEffect() && (!(spell->GetAVEffect() == nullptr)) &&
                            spell->GetAVEffect()->data.projectileBase &&
                            (spell->GetAVEffect()->data.projectileBase != nullptr)) {
                            log::debug("********************************************************************************************************************");
                            log::debug("UnChanged,Aimed,Fire And Forget:Spell Full name:{}|Spell FormID:{:08X}|Spell Speed:{}",spell->GetFullName(),spell->GetFormID(),spell->GetAVEffect()->data.projectileBase->data.speed);
                            spell->GetAVEffect()->data.projectileBase->data.speed = aimedFireAndForgetSpeed;
                            log::debug("Changed,Aimed,Fire And Forget:Spell Full name:{}|Spell FormID:{:08X}|Spell Speed:{}",spell->GetFullName(),spell->GetFormID(),spell->GetAVEffect()->data.projectileBase->data.speed);
                            log::debug("********************************************************************************************************************");
                        }
                    }
                
            }
        }
        log::info("{} {} Finished Patching.", plugin->GetName(),version);
        log::info("{} {} has finished loading.", plugin->GetName(),version);
    }

    void LoadJSON() {
        log::info("Starting to load {}.json", PluginDeclaration::GetSingleton()->GetName());
        std::ifstream jsonfile(std::format("Data/SKSE/Plugins/{}.json",plugin->GetName()));
        if (!jsonfile.is_open()) {
            log::info("Failed to open JSON file : Data/SKSE/Plugins/{}.json", plugin->GetName());
        }
        json jsonData = json::parse(jsonfile);

        logLevelStr = jsonData["Logging"]["LogLevel"].get<std::string>();
        if (logLevelStr == "trace") {
            logLevel = spdlog::level::trace;
        } else if (logLevelStr == "debug") {
            logLevel = spdlog::level::debug;
        } else if (logLevelStr == "info") {
            logLevel = spdlog::level::info;
        } else if (logLevelStr == "warn") {
            logLevel = spdlog::level::warn;
        } else if (logLevelStr == "err") {
            logLevel = spdlog::level::err;
        } else if (logLevelStr == "critical") {
            logLevel = spdlog::level::critical;
        } else {
            // Default to info if the specified log level is invalid
            log::critical("Invalid log level '{}' specified in the JSON file. Defaulting to info level.",logLevelStr);
            logLevel = spdlog::level::info;
        }

        if(changeaimedConcentration = jsonData["Aimed"]["Concentrated"]["Enable"].get<bool>()) {
            aimedConcentrationSpeed = jsonData["Aimed"]["Concentrated"]["Speed"].get<float>();
        }

        if(changeaimedFireAndForget = jsonData["Aimed"]["Fire and Forget"]["Enable"].get<bool>()) {
            aimedFireAndForgetSpeed = jsonData["Aimed"]["Fire and Forget"]["Speed"].get<float>();
        }

        whereDiditCrash = jsonData["Where did it Crash"].get<bool>();
        jsonData.clear();

        std::unordered_map<std::string, json> uniqueData;
        if (!fs::is_empty(folder_path)) {
            for (const auto& entry : fs::directory_iterator(folder_path)) {
                hasFilesToMerge = true;
                if (entry.path().extension() == ".json") {
                    std::ifstream jFile(entry.path());
                    jFile >> json_data;
                    log::debug("Loaded JSON from file: {}", entry.path().generic_string());
                    for (const auto& new_entry : json_data.items()) {
                        uniqueData[new_entry.key()] = new_entry.value();
                    }
                    json_data.clear();
                }
            }
        } else hasFilesToMerge = false;
        
        if(hasFilesToMerge) {
            // Convert uniqueData to json using move semantics
            merge_data = json(std::move(uniqueData));
            use_data = json::parse(merge_data.dump());
        } else log::info("No Exclusion will be Done");
        log::info("Finished loading {}.json", PluginDeclaration::GetSingleton()->GetName());
    }

    void InitializeMessaging() {
        if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message* message) {
                if (message->type == MessagingInterface::kDataLoaded) spellPatch();
            })) {
            stl::report_and_fail("Unable to register message listener.");
        }
    }
}  // namespace MTSOF

SKSEPluginLoad(const LoadInterface* skse) {
    MTSOF::InitializeLogging();

    log::info("{} {} is loading...", MTSOF::plugin->GetName(), MTSOF::version);
    MTSOF::LoadJSON();
    Init(skse);

    MTSOF::InitializeMessaging();
    return true;
}
