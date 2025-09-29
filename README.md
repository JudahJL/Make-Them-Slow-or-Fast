# Make Them Slow or Fast

A simple SKSE plugin to patch all aimed Fire and Forget spells at runtime. Supports all Skyrim runtimes.

[![Nexus Mods](https://img.shields.io/badge/Nexus-NG-blue)](https://www.nexusmods.com/skyrimspecialedition/mods/109355)

## Overview

This plugin dynamically modifies the behavior of Fire and Forget spells in Skyrim, allowing you to make them faster or slower as needed. It works with all Skyrim versions including Special Edition, Anniversary Edition, and VR.

## User Requirements

### Required Dependencies
- [Skyrim Script Extender (SKSE)](https://skse.silverlock.org/) - Required for any SKSE plugin
- [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444) - Required for SSE/AE
- [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101) - Required for VR

## Development Requirements

### Prerequisites
- [Git](https://git-scm.com/downloads) - Latest version
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/) with Desktop development with C++ workload

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/JudahJL/Make-Them-Slow-or-Fast.git
   cd Make-Them-Slow-or-Fast
   ```

2. Build the project:
   ```bash
   # Using the provided PowerShell script
   .\build.ps1
   
   # Or manually with xmake
   xmake -y Make_Them_Slow_or_Fast
   ```

### Build Script Explanation

The `build.ps1` script automates the build process with the following steps:

1. **Environment Setup**: If not running in GitHub Actions and a `.env` file exists, it loads environment variables from the file
2. **Package Management**: Updates xmake repositories and upgrades project dependencies
3. **IDE Support**: Generates CMakeLists.txt for CLion integration
4. **Build**: Compiles the project with the target name `Make_Them_Slow_or_Fast`

The script essentially runs:
```bash
xmake repo --update
xmake require --upgrade  
xmake project -k cmakelists
xmake -y Make_Them_Slow_or_Fast
```

### Environment Variables

The build system supports several environment variables for customization:

#### Project-Specific Variables (from xmake.lua)
- **`MO2_MODS_DIR`**: If set, copies built files to `{MO2_MODS_DIR}/testing/skse/plugins/` for Mod Organizer 2 testing

#### CommonLibSSE-NG Variables (from submodule)
- **`XSE_TES5_MODS_PATH`**: Alternative mods directory path for installation
- **`XSE_TES5_GAME_PATH`**: Skyrim game directory path for direct installation

#### Usage Examples
```bash
# For Mod Organizer 2 testing (project-specific)
$env:MO2_MODS_DIR = "C:\Modding\MO2\mods"
.\build.ps1

# For direct game installation (CommonLibSSE-NG)
$env:XSE_TES5_GAME_PATH = "C:\Steam\steamapps\common\Skyrim Special Edition"
.\build.ps1

# For alternative mods directory (CommonLibSSE-NG)
$env:XSE_TES5_MODS_PATH = "C:\Modding\Mods"
.\build.ps1
```

#### Default Behavior
Without environment variables, the build system:
- Copies `.dll` and `.pdb` files to `contrib/Distribution/data/skse/plugins/`
- Uses the standard xmake build process

## Development

### Build System
This project uses [xmake](https://xmake.io/) as the build system.

### Available Commands

#### Package Management
```bash
# Update package repositories
xmake repo --update

# Upgrade project dependencies
xmake require --upgrade
```

#### IDE Support
```bash
# Generate project files for various IDEs
xmake project -k cmakelists  # For CLion
xmake project -k vsxmake     # For Visual Studio
```

### Configuration Rules
```
Blacklist(ID) > Blacklist(File)
```

## License

[MIT License](LICENSE)