#pragma once

class Settings
{
public:
    struct SpellInfo {
        decltype(RE::BGSProjectileData::gravity) ProjGravity;
        decltype(RE::BGSProjectileData::speed)   ProjSpeed;
        RE::SpellItem*                           spell;
        RE::BGSProjectile*                       spellProjectile;
    };

    static Settings& GetSingleton();
    Settings&        PatchSpells();

    Settings&                  LoadCurrPreset();
    [[nodiscard]] bool         SaveCurrPreset() const;
    Settings&                  LoadPresets();
    Settings&                  PopulateSpellInfo();  // to be called once only
    Settings&                  PopulateFormIDMap();  // to be called once only
    std::optional<bool>        IsAllowed(std::string_view file, RE::FormID id);
    Settings&                  SetLogAndFlushLevel();
    Settings&                  RevertToDefault();
    [[nodiscard]] bool         ReloadPreset(const std::filesystem::path& a_path);
    Settings&                  GetDefaultPreset(rapidjson::Document& doc);
    static rapidjson::Document LoadSchema(std::array<char, 576>& buff);

    std::unordered_map<std::filesystem::path, rapidjson::Document> presets;
    std::filesystem::path                                          curr_preset;

    enum class Rule : std::uint8_t {
        kNone = 0,
        kBlacklist
    };

    struct FileRule {
        std::optional<Rule>                  global;  // file-wide rule
        std::unordered_map<RE::FormID, Rule> per_id;  // record rules
    };

    std::unordered_map<std::string, FileRule> form_id_map;
    std::vector<SpellInfo>                    spell_info;
    bool                                      patching{ false };

    Settings(const Settings&)             = delete;
    Settings(Settings&&)                  = delete;
    Settings& operator= (const Settings&) = delete;
    Settings& operator= (Settings&&)      = delete;

private:
    Settings()  = default;
    ~Settings() = default;

    static Settings singleton;
};
