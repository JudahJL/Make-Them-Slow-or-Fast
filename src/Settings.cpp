#include "Settings.h"

#include "FilePtrManager.h"
#include "timeit.h"
#include "utils.h"
#include <ctre.hpp>

Settings Settings::singleton;

Settings& Settings::GetSingleton() { return singleton; }

Settings& Settings::PatchSpells() {
    auto preset_itr = presets.find(curr_preset);

    if(preset_itr == presets.end()) {
        std::wstringstream wss;
        wss << "Couldn't find the preset: " << curr_preset.c_str() << ". Will not Patch";
        SPDLOG_ERROR(wss.view().data());
        return *this;
    }

    auto& preset = preset_itr->second;

    constexpr const char* MainJsonKeys[]{
        "User Details",     // 00
        "Aimed",            // 01
        "Enable",           // 02
        "Fire and Forget",  // 03
        "Change Speed",     // 04
        "Speed",            // 05
        "Limit Speed",      // 06
        "Min",              // 07
        "Max",              // 08
        "Randomize Speed",  // 09
        "Change Gravity",   // 10
        "Gravity",          // 11
        "Limit Gravity"     // 12
    };

    auto& Aimed = preset[MainJsonKeys[1]];                        // Aimed

    auto& AimedFireForget = Aimed[MainJsonKeys[3]];               // Fire And Forget for Aimed

    auto& AimedFireForgetCS = AimedFireForget[MainJsonKeys[4]];   // CS == Change Speed
    auto& AimedFireForgetLS = AimedFireForget[MainJsonKeys[6]];   // LS == Limit Speed
    auto& AimedFireForgetRS = AimedFireForget[MainJsonKeys[9]];   // RS == Randomize Speed
    auto& AimedFireForgetCG = AimedFireForget[MainJsonKeys[10]];  // CG == Change Gravity
    auto& AimedFireForgetLG = AimedFireForget[MainJsonKeys[12]];  // LS == Limit Gravity

    // Enable
    constexpr auto Enable{ 2 };
    auto           enableAimed               = Aimed[MainJsonKeys[Enable]].GetBool();
    auto changeAimedFireForgetSpeedEnable    = AimedFireForgetCS[MainJsonKeys[Enable]].GetBool();
    auto limitAimedFireForgetSpeedEnable     = AimedFireForgetLS[MainJsonKeys[Enable]].GetBool();
    auto RandomizeAimedFireForgetSpeedEnable = AimedFireForgetRS[MainJsonKeys[Enable]].GetBool();
    auto changeAimedFireForgetGravityEnable  = AimedFireForgetCG[MainJsonKeys[Enable]].GetBool();
    auto limitAimedFireForgetGravityEnable   = AimedFireForgetLG[MainJsonKeys[Enable]].GetBool();

    // Speed
    constexpr auto Speed{ 5 };
    auto           aimedFireForgetSpeed = AimedFireForgetCS[MainJsonKeys[Speed]].GetFloat();

    // Gravity
    constexpr auto Gravity{ 11 };
    auto           aimedFireForgetGravity = AimedFireForgetCG[MainJsonKeys[Gravity]].GetFloat();

    // Min
    constexpr auto Min{ 7 };
    auto           limitAimedFireForgetSpeedMin = AimedFireForgetLS[MainJsonKeys[Min]].GetFloat();
    auto RandomizeAimedFireForgetSpeedMin       = AimedFireForgetRS[MainJsonKeys[Min]].GetFloat();
    auto limitAimedFireForgetGravityMin         = AimedFireForgetLG[MainJsonKeys[Min]].GetFloat();

    // Max
    constexpr auto Max{ 8 };
    auto           limitAimedFireForgetSpeedMax = AimedFireForgetLS[MainJsonKeys[Max]].GetFloat();
    auto RandomizeAimedFireForgetSpeedMax       = AimedFireForgetRS[MainJsonKeys[Max]].GetFloat();
    auto limitAimedFireForgetGravityMax         = AimedFireForgetLG[MainJsonKeys[Max]].GetFloat();

    patching = true;

    if(enableAimed) {
        for(const auto* const spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
            if(spell) {
                if(auto* const spellEffectSetting = spell->GetAVEffect()) {
                    if(auto* const spellProjectile = spellEffectSetting->data.projectileBase) {
                        constexpr auto starString{ InlineUtils::make_filled_char_array<125, '*'>() };
                        auto isAllowed = IsAllowed(spell->GetFile()->GetFilename().data(), spell->GetLocalFormID());
                        if(isAllowed.has_value() && *isAllowed == false) {  // blacklisted
                            SPDLOG_DEBUG("{}", starString);
                            SPDLOG_DEBUG("Skipping Spell : Name:{}|FormID:{:08X}|Projectile "
                                         "Name:{}|Projectile FormID:{:08X}|Projectile "
                                         "Speed:{}|Projectile Gravity:{}|File:{}",
                                         spell->GetFullName(), spell->GetRawFormID(),
                                         spellProjectile->GetFullName(), spellProjectile->GetRawFormID(),
                                         spellProjectile->data.speed, spellProjectile->data.gravity,
                                         spell->GetFile()->GetFilename());
                            SPDLOG_DEBUG("{}", starString);
                            continue;
                        }
                        if(spell->data.spellType == RE::MagicSystem::SpellType::kSpell) {
                            auto delivery = spell->data.delivery;
                            auto casttype = spell->data.castingType;
                            bool SpellPatched = changeAimedFireForgetSpeedEnable || limitAimedFireForgetSpeedEnable || changeAimedFireForgetGravityEnable || limitAimedFireForgetGravityEnable;
                            if((delivery == RE::MagicSystem::Delivery::kAimed) && (casttype == RE::MagicSystem::CastingType::kFireAndForget)) {
                                if(SpellPatched) {
                                    SPDLOG_DEBUG("{}", starString);
                                    SPDLOG_DEBUG("Original Aimed,Fire And Forget Spell : Full "
                                                 "Name:{}|FormID:{:08X}|Projectile "
                                                 "Name:{}|Projectile FormID:{:08X}|Projectile "
                                                 "Speed:{}|Projectile Gravity:{}|File:{}",
                                                 spell->GetFullName(), spell->GetRawFormID(),
                                                 spellProjectile->GetFullName(), spellProjectile->GetRawFormID(),
                                                 spellProjectile->data.speed, spellProjectile->data.gravity,
                                                 spell->GetFile()->GetFilename());
                                }
                                if(changeAimedFireForgetSpeedEnable) {
                                    spellProjectile->data.speed = aimedFireForgetSpeed;
                                    SPDLOG_DEBUG("modified speed");
                                }
                                if(limitAimedFireForgetSpeedEnable) {
                                    InlineUtils::limit(spellProjectile->data.speed, limitAimedFireForgetSpeedMin, limitAimedFireForgetSpeedMax);
                                    SPDLOG_DEBUG("limited speed");
                                }
                                if(RandomizeAimedFireForgetSpeedEnable) {
                                    float speed = InlineUtils::getRandom(RandomizeAimedFireForgetSpeedMin, RandomizeAimedFireForgetSpeedMax);
                                    spellProjectile->data.speed = speed;
                                    SPDLOG_DEBUG("Randomized Speed to {}", speed);
                                }
                                if(changeAimedFireForgetGravityEnable) {
                                    spellProjectile->data.gravity = aimedFireForgetGravity;
                                    SPDLOG_DEBUG("modified gravity");
                                }
                                if(limitAimedFireForgetGravityEnable) {
                                    InlineUtils::limit(spellProjectile->data.gravity, limitAimedFireForgetGravityMin, limitAimedFireForgetGravityMax);
                                    SPDLOG_DEBUG("limited gravity");
                                }
                                if(SpellPatched) {
                                    SPDLOG_DEBUG("Modified Aimed,Fire And Forget Spell : Full "
                                                 "Name:{}|FormID:{:08X}|Projectile "
                                                 "Name:{}|Projectile FormID:{:08X}|Projectile "
                                                 "Speed:{}|Projectile Gravity:{}|File:{}",
                                                 spell->GetFullName(), spell->GetRawFormID(),
                                                 spellProjectile->GetFullName(), spellProjectile->GetRawFormID(),
                                                 spellProjectile->data.speed, spellProjectile->data.gravity,
                                                 spell->GetFile()->GetFilename());
                                    SPDLOG_DEBUG("{}", starString);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    patching = false;
    return *this;
}

Settings& Settings::LoadCurrPreset() {
    constexpr auto             file_path{ BASE_PATH "Make_Them_Slow_or_Fast.json" };
    std::array<char, MAX_PATH> readBuffer{};
    if(FilePtrManager file{ file_path }) {
        rapidjson::Document       sd;
        rapidjson::FileReadStream bis(file.get(), readBuffer.data(), std::size(readBuffer));
        if(rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(bis);
           sd.ParseStream<0, rapidjson::AutoUTF<unsigned>>(eis).HasParseError()) [[unlikely]] {
            SPDLOG_ERROR("Error(offset {}): {}", sd.GetErrorOffset(), rapidjson::GetParseError_En(sd.GetParseError()));
            SPDLOG_ERROR("Loading {} failed", file_path);
        } else if(auto itr{ sd.FindMember("load") }; itr != sd.MemberEnd() && itr->value.IsString()) {
            curr_preset  = BASE_PATH "Make Them Slow or Fast/Presets";
            curr_preset /= itr->value.GetString();
            if(curr_preset.extension() != ".json" || !std::filesystem::exists(curr_preset) || !std::filesystem::is_regular_file(curr_preset)) {
                SPDLOG_ERROR("Couldn't Find '{}'. Will load Default.json", itr->value.GetString());
                curr_preset = BASE_PATH "Make Them Slow or Fast/Presets/Default.json";
                if(curr_preset.extension() != ".json" || !std::filesystem::exists(curr_preset) || !std::filesystem::is_regular_file(curr_preset)) {
                    if((file = FilePtrManager{ curr_preset.c_str(), L"wb" })) {
                        rapidjson::Document doc;
                        GetDefaultPreset(doc);

                        std::array<char, 576> writeBuffer{};
                        rapidjson::FileWriteStream os(file.get(), writeBuffer.data(), std::size(writeBuffer));
                        rapidjson::PrettyWriter writer(os);
                        doc.Accept(writer);
                        presets.emplace(curr_preset, std::move(doc));
                    }
                }
            }
        }
    }
    return *this;
}

bool Settings::SaveCurrPreset() const {
    rapidjson::Document file(rapidjson::kObjectType);
    auto                temp = InlineUtils::wstringToString(curr_preset.filename());
    file.AddMember("load", rapidjson::Value{}.SetString(temp.data(), static_cast<rapidjson::SizeType>(temp.size())), file.GetAllocator());

    if(FilePtrManager f{ BASE_PATH "Make_Them_Slow_or_Fast.json", "wb" }) {
        char                       writeBuffer[MAX_PATH]{};
        rapidjson::FileWriteStream os(f.get(), writeBuffer, sizeof(writeBuffer));
        rapidjson::PrettyWriter    writer(os);
        return file.Accept(writer);
    }
    SPDLOG_ERROR("Couldn't open Make_Them_Slow_or_Fast.json for writing.");
    return false;
}

rapidjson::Document Settings::LoadSchema(std::array<char, 576>& buff) {
    constexpr auto schema_path{ "data/SKSE/Plugins/MTSOFConfig_schema.json" };
    if(FilePtrManager schema{ schema_path }) {
        rapidjson::Document       sd;
        rapidjson::FileReadStream bis(schema.get(), buff.data(), buff.size());
        if(rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(bis);
           sd.ParseStream<0, rapidjson::AutoUTF<unsigned>>(eis).HasParseError()) {
            SPDLOG_CRITICAL("Error(offset {}): {}", sd.GetErrorOffset(), rapidjson::GetParseError_En(sd.GetParseError()));
            SPDLOG_CRITICAL("Loading {} failed", schema_path);
            SKSE::stl::report_and_fail("Please Check the log. Seems like there is a "
                                       "issue with loading the schema");
        }
        std::memset(buff.data(), 0, buff.size());
        return sd;
    }
    SKSE::stl::report_and_fail("Please Check the log. Seems like there is a issue "
                               "with loading the schema");
}

Settings& Settings::LoadPresets() {
    [[maybe_unused]] const timeit t;
    presets.clear();

    std::array<char, 576>     readBuffer{};
    constexpr auto            presets_path{ BASE_PATH "Make Them Slow or Fast/Presets/" };
    rapidjson::SchemaDocument schemaDoc{ LoadSchema(readBuffer) };

    if(fs::exists(presets_path) && !fs::is_empty(presets_path)) {
        for(const auto& entry : std::filesystem::directory_iterator(presets_path)) {
            const auto& path{ entry.path() };
            if(fs::is_regular_file(path) && path.extension() == ".json") {
                if(FilePtrManager file{ path.c_str() }) {
                    std::memset(readBuffer.data(), 0, readBuffer.size());
                    rapidjson::Document sd;
                    rapidjson::FileReadStream bis(file.get(), readBuffer.data(), std::size(readBuffer));
                    if(rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(bis);
                       sd.ParseStream<0, rapidjson::AutoUTF<unsigned>>(eis).HasParseError()) {
                        char buff[1'024];
                        sprintf_s(buff, std::size(buff), "File '%ls':Error(offset %zu): %s", path.c_str(), sd.GetErrorOffset(), rapidjson::GetParseError_En(sd.GetParseError()));
                        SPDLOG_ERROR(buff);
                        continue;
                    }
                    if(rapidjson::SchemaValidator validator{ schemaDoc }; !sd.Accept(validator)) {
                        {
                            wchar_t buff[2'048]{ L"" };
                            swprintf_s(buff, std::size(buff), L"File: %ls", path.c_str());
                            SPDLOG_ERROR(buff);
                        }
                        rapidjson::StringBuffer sb;
                        rapidjson::PrettyWriter writer(sb);
                        const auto invalidSchemaPointer{ validator.GetInvalidSchemaPointer() };
                        invalidSchemaPointer.StringifyUriFragment(sb);
                        SPDLOG_ERROR("Invalid schema: {}", sb.GetString());
                        SPDLOG_ERROR("Invalid keyword: {}", validator.GetInvalidSchemaKeyword());
                        sb.Clear();
                        const auto invalidDocumentPointer{ validator.GetInvalidDocumentPointer() };
                        invalidDocumentPointer.StringifyUriFragment(sb);
                        SPDLOG_ERROR("Invalid document: {}", sb.GetString());
                        sb.Clear();
                        if(auto* err_value_ptr{ invalidDocumentPointer.Get(sd) }) {
                            err_value_ptr->Accept(writer);
                            SPDLOG_ERROR("Error at: {}", sb.GetString());
                            sb.Clear();
                        }
                        if(auto* err_values_schema_pointer{ invalidSchemaPointer.Get(sd) }) {
                            err_values_schema_pointer->Accept(writer);
                            SPDLOG_ERROR("Schema Definition of Error: {}", sb.GetString());
                        }
                        continue;
                    }
                    presets.try_emplace(path, std::move(sd));
                    {
                        wchar_t buff[2'048]{ L"" };
                        swprintf_s(buff, std::size(buff), L"Found Preset: %s", path.c_str());
                        SPDLOG_TRACE(buff);
                    }
                }
            }
        }
    }
    constexpr auto default_preset_path{ BASE_PATH "Make Them Slow or Fast/Presets/Default.json" };
    if(!presets.contains(curr_preset) && !presets.contains(default_preset_path)) {
        if(FilePtrManager f{ default_preset_path, "wb" }) {
            curr_preset = default_preset_path;
            rapidjson::FileWriteStream os(f.get(), readBuffer.data(), std::size(readBuffer));
            rapidjson::PrettyWriter    writer(os);
            rapidjson::Document        doc;
            GetDefaultPreset(doc);
            std::memset(readBuffer.data(), 0, readBuffer.size());
            doc.Accept(writer);
            presets.try_emplace(curr_preset, std::move(doc));
        }
    }
    return *this;
}

Settings& Settings::PopulateSpellInfo() {
    for(auto* const spell : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::SpellItem>()) {
        if(spell) {
            if(spell->data.spellType == RE::MagicSystem::SpellType::kSpell) {
                const auto delivery    = spell->data.delivery;
                auto       castingtype = spell->data.castingType;
                if((delivery == RE::MagicSystem::Delivery::kAimed) && (castingtype == RE::MagicSystem::CastingType::kFireAndForget)) {
                    if(auto* const spellEffectSetting = spell->GetAVEffect()) {
                        if(auto* const spellProjectile = spellEffectSetting->data.projectileBase) {
                            spell_info.emplace_back(
                                spellProjectile->data.gravity, spellProjectile->data.speed, spell, spellProjectile);
                        }
                    }
                }
            }
        }
    }
    return *this;
}

Settings::Rule ParseRule(const char* s) {
    if(_stricmp(s, "blacklist") == 0) return Settings::Rule::kBlacklist;
    return Settings::Rule::kNone;
}

Settings& Settings::PopulateFormIDMap() {
    using ctre::literals::operator""_ctre;
    constexpr auto FormID{ R"(^(0[xX])?[0-9A-Fa-f]{3,8}$)"_ctre };
    constexpr auto file_path{ "data/SKSE/Plugins/MTSOF_Rules.json" };
    if(const FilePtrManager file{ file_path }) {
        char                      readBuffer[65'535]{ "" };
        rapidjson::Document       doc;
        rapidjson::FileReadStream bis(static_cast<FILE*>(file), readBuffer, std::size(readBuffer));
        if(rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(bis);
           doc.ParseStream < rapidjson::kParseCommentsFlag | rapidjson::kParseStopWhenDoneFlag | rapidjson::kParseTrailingCommasFlag,
           rapidjson::AutoUTF < unsigned >> (eis).HasParseError()) [[unlikely]] {
            SPDLOG_ERROR("Error(offset {}): {}", doc.GetErrorOffset(), rapidjson::GetParseError_En(doc.GetParseError()));
            SPDLOG_ERROR("Loading {} failed", file_path);
        } else {
            if(!doc.IsObject()) {
                SPDLOG_ERROR("Root of JSON must be an object");
                return *this;
            }

            for(auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
                std::string fileName = itr->name.GetString();
                const auto& entry    = itr->value;

                FileRule rules;

                // Global rule
                if(auto it = entry.FindMember("global"); it != entry.MemberEnd() && it->value.IsString()) {
                    rules.global = ParseRule(it->value.GetString());
                }

                // Per-record rules
                if(auto it = entry.FindMember("records"); it != entry.MemberEnd() && it->value.IsObject()) {
                    for(auto rec = it->value.MemberBegin(); rec != it->value.MemberEnd(); ++rec) {
                        std::string_view id_str = rec->name.GetString();

                        if(!FormID.match(id_str)) {  // NOLINT(*-static-accessed-through-instance)
                            SPDLOG_WARN("Invalid FormID '{}' in '{}'", id_str, fileName);
                            continue;
                        }

                        RE::FormID id = 0;
                        if(sscanf_s(id_str.data(), "%x", &id) != 1) {
                            SPDLOG_WARN("Failed to parse FormID '{}' in '{}'", id_str, fileName);
                            continue;
                        }

                        if(rec->value.IsString()) {
                            rules.per_id[id] = ParseRule(rec->value.GetString());
                        }
                    }
                }

                form_id_map.try_emplace(std::move(fileName), std::move(rules));
            }
        }
    }
    return *this;
}

std::optional<bool> Settings::IsAllowed(const std::string_view file, RE::FormID id) {
    auto it = form_id_map.find(file.data());
    if(it == form_id_map.end()) return std::nullopt;  // no rules for this file at all

    const FileRule& rules = it->second;

    // 1. Per-ID rules first
    if(auto f = rules.per_id.find(id); f != rules.per_id.end()) {
        if(f->second == Rule::kBlacklist) return false;
    }

    // 2. Global file rule
    if(rules.global.has_value()) {
        if(*rules.global == Rule::kBlacklist) return false;
    }

    // 3. No decision → use default behavior
    return std::nullopt;
}

Settings& Settings::SetLogAndFlushLevel() {
    const auto level = spdlog::level::from_str(presets[curr_preset]["Logging"]["LogLevel"].GetString());
    spdlog::set_level(level);
    spdlog::flush_on(level);
    spdlog::default_logger()->flush();  // initially set was flush to off. now flush everything
    // since we got the loglevel
    SPDLOG_DEBUG("LogLevel: {}", spdlog::level::to_string_view(level));
    return *this;
}

Settings& Settings::RevertToDefault() {
    [[maybe_unused]] const timeit t;
    for(auto& [gravity, speed, spell, spellProjectile] : spell_info) {
        SPDLOG_TRACE("Reverting: SpellName: {}, ProjectileName: {}", spell->GetName(), spellProjectile->GetName());
        SPDLOG_TRACE("Gravity: {} -> {}", spellProjectile->data.gravity, gravity);
        SPDLOG_TRACE("Speed: {} -> {}", spellProjectile->data.speed,speed);
        spellProjectile->data.gravity = gravity;
        spellProjectile->data.speed   = speed;
    }
    SPDLOG_DEBUG("Revered {} records to default", spell_info.size());
    return *this;
}

bool Settings::ReloadPreset(const std::filesystem::path& path) {
    if(auto it = presets.find(path); it != presets.end()) {
        if(auto file = FilePtrManager{ path.c_str() }) {
            std::array<char, 576>     readBuffer{};
            rapidjson::SchemaDocument schemaDoc{ LoadSchema(readBuffer) };
            rapidjson::Document       sd;
            rapidjson::FileReadStream bis(file.get(), readBuffer.data(), std::size(readBuffer));
            if(rapidjson::AutoUTFInputStream<unsigned, rapidjson::FileReadStream> eis(bis);
               sd.ParseStream<0, rapidjson::AutoUTF<unsigned>>(eis).HasParseError()) {
                char buff[1'024];
                sprintf_s(buff, std::size(buff), "File '%ls':Error(offset %zu): %s", path.c_str(), sd.GetErrorOffset(), rapidjson::GetParseError_En(sd.GetParseError()));
                SPDLOG_ERROR(buff);
                return false;
            }
            if(rapidjson::SchemaValidator validator{ schemaDoc }; !sd.Accept(validator)) {
                {
                    wchar_t buff[2'048]{ L"" };
                    swprintf_s(buff, std::size(buff), L"File: %ls", path.c_str());
                    SPDLOG_ERROR(buff);
                }
                rapidjson::StringBuffer sb;
                rapidjson::PrettyWriter writer(sb);
                const auto              invalidSchemaPointer{ validator.GetInvalidSchemaPointer() };
                invalidSchemaPointer.StringifyUriFragment(sb);
                SPDLOG_ERROR("Invalid schema: {}", sb.GetString());
                SPDLOG_ERROR("Invalid keyword: {}", validator.GetInvalidSchemaKeyword());
                sb.Clear();
                const auto invalidDocumentPointer{ validator.GetInvalidDocumentPointer() };
                invalidDocumentPointer.StringifyUriFragment(sb);
                SPDLOG_ERROR("Invalid document: {}", sb.GetString());
                sb.Clear();
                if(auto* err_value_ptr{ invalidDocumentPointer.Get(sd) }) {
                    err_value_ptr->Accept(writer);
                    SPDLOG_ERROR("Error at: {}", sb.GetString());
                    sb.Clear();
                }
                if(auto* err_values_schema_pointer{ invalidSchemaPointer.Get(sd) }) {
                    err_values_schema_pointer->Accept(writer);
                    SPDLOG_ERROR("Schema Definition of Error: {}", sb.GetString());
                }
                return false;
            }
            it->second.Swap(sd);
            return true;
        }
    }
    return false;
}

Settings& Settings::GetDefaultPreset(rapidjson::Document& doc) {
    doc.SetObject();
    auto& a = doc.GetAllocator();

    // Logging
    auto logging = rapidjson::Value(rapidjson::kObjectType);
    logging.AddMember("LogLevel", "info", a);
    doc.AddMember("Logging", logging, a);

    // Aimed
    auto aimed = rapidjson::Value(rapidjson::kObjectType);
    aimed.AddMember("Enable", true, a);

    // Fire and Forget
    auto fire_and_forget = rapidjson::Value(rapidjson::kObjectType);

    // Change Speed
    auto change_speed = rapidjson::Value(rapidjson::kObjectType);
    change_speed.AddMember("Enable", true, a);
    change_speed.AddMember("Speed", 1000.0, a);
    fire_and_forget.AddMember("Change Speed", change_speed, a);

    // Limit Speed
    auto limit_speed = rapidjson::Value(rapidjson::kObjectType);
    limit_speed.AddMember("Enable", false, a);
    limit_speed.AddMember("Min", 10.0, a);
    limit_speed.AddMember("Max", 1000.0, a);
    fire_and_forget.AddMember("Limit Speed", limit_speed, a);

    // Randomize Speed
    auto randomize_speed = rapidjson::Value(rapidjson::kObjectType);
    randomize_speed.AddMember("Enable", true, a);
    randomize_speed.AddMember("Min", 3000.0, a);
    randomize_speed.AddMember("Max", 12000.0, a);
    fire_and_forget.AddMember("Randomize Speed", randomize_speed, a);

    // Change Gravity
    auto change_gravity = rapidjson::Value(rapidjson::kObjectType);
    change_gravity.AddMember("Enable", false, a);
    change_gravity.AddMember("Gravity", 1.0, a);
    fire_and_forget.AddMember("Change Gravity", change_gravity, a);

    // Limit Gravity
    auto limit_gravity = rapidjson::Value(rapidjson::kObjectType);
    limit_gravity.AddMember("Enable", false, a);
    limit_gravity.AddMember("Min", 0.0, a);
    limit_gravity.AddMember("Max", 3.0, a);
    fire_and_forget.AddMember("Limit Gravity", limit_gravity, a);

    // Add Fire and Forget to Aimed
    aimed.AddMember("Fire and Forget", fire_and_forget, a);

    // Add Aimed to doc
    doc.AddMember("Aimed", aimed, a);

    return *this;
}
