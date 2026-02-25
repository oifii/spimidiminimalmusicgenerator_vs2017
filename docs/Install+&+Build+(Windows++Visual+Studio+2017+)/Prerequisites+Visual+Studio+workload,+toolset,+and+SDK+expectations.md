# Install & Build (Windows / Visual Studio 2017+)

## Prerequisites

### Visual Studio Workload

Ensure you have Visual Studio 2017 (version 15.x) or later installed with the **Desktop development with C++** workload. This installs the MSVC compiler, the VC++ libraries, and the IDE integration needed to build console applications.

### Platform Toolset

This project is configured to use the **v141** PlatformToolset (the VS 2017 toolchain) in all configurations (`Debug|Win32`, `Debug|x64`, `Release|Win32`, `Release|x64`) .

- To verify, open **Project → Properties → Configuration Properties → General** and confirm **Platform Toolset** is set to `v141`.
- If **v141** is not listed, run the Visual Studio Installer, choose **Modify** → **Individual components**, and select **VC++ 2017 v141 toolset**.

### Windows SDK

The project’s global SDK version is **10.0.19041.0** (`WindowsTargetPlatformVersion`) .

- In the Installer under **Individual components**, install **Windows 10 SDK (10.0.19041.0)**.
- In **Project → Properties → Configuration Properties → General**, ensure **Windows SDK Version** matches **10.0.19041.0**.

### Supported Configurations

Four build targets are defined in `spimidiminimalmusicgenerator.vcxproj` ﹕

- Debug | Win32
- Debug | x64
- Release | Win32
- Release | x64

Switch between these via **Build → Configuration Manager** in the IDE or by supplying MSBuild parameters:

```batch
msbuild spimidiminimalmusicgenerator.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Verifying Your Environment

1. **Open the Solution**
2. Launch Visual Studio and open the `.vcxproj` in **File → Open → Project/Solution**.
3. **Inspect Global Properties**
4. Under the top-level project node, confirm `<WindowsTargetPlatformVersion>` is `10.0.19041.0` and each `<PropertyGroup>` for your active configuration specifies `<PlatformToolset>v141</PlatformToolset>` .
5. **Rebuild**
6. Build the project (`Ctrl+Shift+B`) in both **Debug** and **Release** for **Win32** and **x64** to verify all dependencies (PortMidi, spiwavsetlib, PortAudio) link correctly.

If you encounter “toolset not found” or “SDK not installed” errors, revisit the Visual Studio Installer and add the missing components.