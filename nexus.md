# Make Them Slow or Fast

A powerful SKSE plugin that dynamically modifies the behavior of Fire and Forget spells in Skyrim, allowing you to make them faster or slower as needed. Works with all Skyrim versions including Special Edition, Anniversary Edition, and VR.

## 🎯 Overview

This mod provides runtime patching of aimed Fire and Forget spells, giving you complete control over projectile speed and gravity. Whether you want to make spells faster for more dynamic combat or slower for more tactical gameplay, this mod has you covered.

## ✨ Features

### 🚀 **Speed Control**
- **Change Speed**: Set a fixed speed for all aimed Fire and Forget spells
- **Limit Speed**: Constrain spell speeds within min/max bounds
- **Randomize Speed**: Add variety with random speeds within specified ranges

### 🌍 **Gravity Control**
- **Change Gravity**: Modify projectile gravity for all spells
- **Limit Gravity**: Constrain gravity values within min/max bounds

### ⚙️ **Advanced Configuration**
- **Preset System**: Create and manage multiple configuration presets
- **In-Game UI**: Configure settings through SKSEMenuFramework integration
- **Blacklist**: Fine-grained control over which spells are affected
- **Real-time Patching**: Apply changes without restarting the game

### 🎮 **User-Friendly Interface**
- **Visual Editor**: Edit JSON configurations through an intuitive interface
- **Preset Management**: Create, save, load, and delete presets easily
- **Live Preview**: See changes applied immediately
- **Error Handling**: Comprehensive validation and error reporting

## 📋 Requirements

### Required Dependencies
- [Skyrim Script Extender (SKSE)](https://skse.silverlock.org/) - Required for any SKSE plugin
- [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444) - Required for SSE/AE
- [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101) - Required for VR

### Optional Dependencies
- [SKSEMenuFramework](https://www.nexusmods.com/skyrimspecialedition/mods/49015) - For in-game configuration UI

## 🚀 Installation

1. Download the latest release from the Files tab
2. Extract the archive to your Skyrim directory
3. Ensure you have the required dependencies installed
4. Launch Skyrim through SKSE

## ⚙️ Configuration

### Configuration Files
- `Data/SKSE/Plugins/Make_Them_Slow_or_Fast.json` - Persistent settings file
- `Data/SKSE/Plugins/Make Them Slow or Fast/Presets/` - Preset storage directory
- `Data/SKSE/Plugins/MTSOF_Rules.json` - blacklist rules
- `Data/SKSE/Plugins/MTSOFConfig_schema.json` - Configuration schema

### In-Game Configuration
If you have SKSEMenuFramework installed:
1. Open the MCP (Mod Configuration Panel)
2. Navigate to "Make Them Slow or Fast"
3. Use the "Edit Presets" option to configure settings
4. Create, modify, and apply presets in real-time

### Manual Configuration
Edit the JSON files directly in your `Data/SKSE/Plugins/Make Them Slow or Fast/Presets/` directory:

```json
{
  "Logging": {
    "LogLevel": "info"
  },
  "Aimed": {
    "Enable": true,
    "Fire and Forget": {
      "Change Speed": {
        "Enable": true,
        "Speed": 1000.0
      },
      "Limit Speed": {
        "Enable": false,
        "Min": 10.0,
        "Max": 1000.0
      },
      "Randomize Speed": {
        "Enable": true,
        "Min": 3000.0,
        "Max": 12000.0
      },
      "Change Gravity": {
        "Enable": false,
        "Gravity": 1.0
      },
      "Limit Gravity": {
        "Enable": false,
        "Min": 0.0,
        "Max": 3.0
      }
    }
  }
}
```

## 🎯 Use Cases

### Combat Enhancement
- **Faster Spells**: Make combat more dynamic with faster projectiles
- **Slower Spells**: Create more tactical gameplay with slower, more visible spells
- **Varied Speeds**: Add unpredictability with randomized speeds

### Roleplay Scenarios
- **Magic Schools**: Different speeds for different schools of magic
- **Character Builds**: Customize spell behavior for specific character concepts
- **Difficulty Adjustment**: Make magic more or less challenging

### Mod Compatibility
- **Spell Packs**: Works with any mod that adds new spells
- **Combat Overhauls**: Complements combat mods by adjusting spell behavior
- **Magic Mods**: Enhances magic-focused mods with customizable projectile behavior

## 🔧 Advanced Features

### Blacklist System
Control which spells are affected using the rules system:

```json
{
  "Skyrim.esm": {
    "global": "blacklist",
    "records": {
      "12AB34": "blacklist"
    }
  }
}
```

### Preset Management
- Create multiple presets for different scenarios
- Switch between presets without restarting
- Share presets with other users
- Backup and restore configurations

### Log Files
Check the SKSE log files for detailed information:
- Log file: `Documents/My Games/Skyrim Special Edition/SKSE/Make_Them_Slow_or_Fast.log`

### Performance
- The mod patches spells at startup and when presets are applied
- Minimal performance impact during gameplay
- Memory usage scales with the number of spells in your load order

## 🤝 Contributing

This mod is open source! Contributions are welcome:
- Report bugs and issues
- Suggest new features
- Submit pull requests
- Help with documentation

## 📄 License

This mod is licensed under the MIT License. See the LICENSE file for details.

## 🙏 Credits

- **Author**: NoName365
- **Framework**: CommonLibSSE-VR NG Branch
- **UI**: SKSEMenuFramework

## 🔗 Links

- **GitHub**: [Repository](https://github.com/JudahJL/Make-Them-Slow-or-Fast)
- **SKSE**: [Skyrim Script Extender](https://skse.silverlock.org/)
- **Address Library**: [SSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444) | [VR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
- **SKSEMenuFramework**: [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/120352)

---

**Enjoy your enhanced magic experience!** 🧙‍♂️✨
