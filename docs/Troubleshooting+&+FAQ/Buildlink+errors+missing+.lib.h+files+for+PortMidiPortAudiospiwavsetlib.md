# Troubleshooting & FAQ – Build/Link Errors: Missing .lib/.h Files for PortMidi, PortAudio, and spiwavsetlib

This section addresses common Visual Studio build and linker errors related to missing PortMidi, PortAudio, and spiwavsetlib headers and libraries. It covers typical error messages, explains the underlying causes, and shows how to correct the project’s include and library settings for each Configuration (Debug/Release) and Platform (Win32/x64).

---

## Common Error Messages

- **C1083**:

`Cannot open include file: 'portmidi.h': No such file or directory`

- **C1083**:

`Cannot open include file: 'spiwavsetlib.h': No such file or directory`

- **LNK1104**:

`cannot open file 'portmidi_s.lib'` or

`cannot open file 'portmidi-dynamic.lib'`

- **LNK1104**:

`cannot open file 'portaudio_x86.lib'` or

`cannot open file 'portaudio_x64.lib'`

- **LNK1104**:

`cannot open file 'spiwavsetlib_vs2017.lib'`

These errors occur when Visual Studio cannot locate the specified header or library files during compilation or linking.

---

## Root Cause

The project’s `.vcxproj` specifies relative paths for headers and libraries under each Configuration/Platform, but those paths must be mirrored in your local file system. If the required third-party sources or built libraries are missing at those locations, or if the Project Properties do not include them, you will see the errors above.

---

## Fixing Include Directories

1. In **Solution Explorer**, right-click the project and choose **Properties**.
2. Under **Configuration** (e.g., Debug | Win32), go to:

**C/C++ → General → Additional Include Directories**.

1. Ensure the following entries appear (note the leading `.` and relative paths) :

```plaintext
   .\
   ..\lib-src\portmidi\pm_common
   ..\spiwavsetlib
   ..\lib-src\portaudio\include
   %(AdditionalIncludeDirectories)
```

1. Repeat for **Debug | x64**, **Release | Win32**, and **Release | x64** configurations, as they use the same include directories .

---

## Fixing Library Dependencies

Under each Configuration/Platform, you must list the built `.lib` files in the project’s **Linker → Input → Additional Dependencies**:

### Debug | Win32

```text
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib;
..\lib-src\portmidi\release\portmidi_s.lib
winmm.lib
..\spiwavsetlib_vs2017u\debug\spiwavsetlib_vs2017.lib
..\lib-src\portaudio\build\msvc\Win32\Debug\portaudio_x86.lib
%(AdditionalDependencies)
```

### Debug | x64

```text
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib;
..\lib-src\portmidi\release\portmidi_s.lib
winmm.lib
..\spiwavsetlib_vs2017u\x64\debug\spiwavsetlib_vs2017.lib
..\lib-src\portaudio\build\msvc\Win32\Debug\portaudio_x86.lib
%(AdditionalDependencies)
```

### Release | Win32

```text
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib;
..\lib-src\portmidi\Release\portmidi_s.lib
winmm.lib
..\spiwavsetlib_vs2017u\release\spiwavsetlib_vs2017.lib
..\lib-src\portaudio\build\msvc\Win32\Release\portaudio_x86.lib
%(AdditionalDependencies)
```

### Release | x64

```text
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;comdlg32.lib;advapi32.lib;
..\lib-src\portmidi(x64)\x64\Release\portmidi-dynamic.lib
winmm.lib
..\spiwavsetlib_vs2017u\x64\release\spiwavsetlib_vs2017.lib
..\lib-src\portaudio(x64)\build\msvc\x64\Release\portaudio_x64.lib
%(AdditionalDependencies)
```

---

## Ensuring Correct Library Directories

While the project uses fully qualified relative paths for `.lib` files, you may optionally set:

- **Linker → General → Additional Library Directories** to the parent directories:

```plaintext
  ..\lib-src\portmidi\release
  ..\lib-src\portmidi(x64)\x64\Release
  ..\spiwavsetlib_vs2017u\debug    (or \release)
  ..\spiwavsetlib_vs2017u\x64\debug (or \release)
  ..\lib-src\portaudio\build\msvc\Win32\Debug (or Release)
  ..\lib-src\portaudio(x64)\build\msvc\x64\Release
  %(AdditionalLibraryDirectories)
```

---

## Platform Mismatch

- **Win32** configurations reference 32-bit libraries (`portaudio_x86.lib`).
- **x64** configurations reference 64-bit libraries (`portaudio_x64.lib`, `portmidi-dynamic.lib`).

Ensure your **Platform** setting matches the library architecture you have built.

---

## FAQ

**Q**: 

**A**: Verify that the PortMidi source is located at `..\lib-src\portmidi\pm_common\portmidi.h` relative to your `.vcxproj`. Then confirm that `..\lib-src\portmidi\pm_common` is listed under Additional Include Directories .

**Q**: 

**A**:

1. Build PortAudio for Win32 Debug or Release under `..\lib-src\portaudio\build\msvc\Win32\[Debug|Release]\portaudio_x86.lib`.
2. Ensure the correct path appears in Additional Dependencies for your configuration .

**Q**: 

**A**:

- Clone or build the `spiwavsetlib_vs2017u` project for the matching configuration and platform.
- Confirm the `.lib` outputs land in `debug` or `release` subfolders and update Additional Dependencies accordingly .

---

By aligning your include and library paths in the Project Properties with the relative directories defined in the `.vcxproj`, the build and link errors for PortMidi, PortAudio, and spiwavsetlib should be resolved.