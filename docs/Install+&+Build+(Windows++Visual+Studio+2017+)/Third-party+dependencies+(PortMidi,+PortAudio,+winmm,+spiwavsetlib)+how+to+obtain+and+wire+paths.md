## Install & Build (Windows / Visual Studio 2017+)

This section describes how to obtain and configure all third-party dependencies required to compile and link the **spimidiminimalmusicgenerator** project under Windows with Visual Studio 2017 (v141). The generator relies on PortMidi for MIDI I/O, PortAudio for low-latency audio support, the `winmm` system library for timer and multimedia functions, and **spiwavsetlib** for WAV sample playback.

---

### Third-party Dependencies

#### PortMidi

- **What it is**

A cross-platform C API for MIDI input/output.

- **Where to get it**

Clone or download the official PortMidi repository:

https://github.com/PortMidi/portmidi

- **Expected directory layout**

Place the PortMidi source under your project root as:

```plaintext
  lib-src/
    portmidi/
      pm_common/            ← contains portmidi.h and common headers
      release/              ← contains portmidi_s.lib for Win32 Release
      x64/
        x64 Release/        ← contains portmidi-dynamic.lib for x64 Release
```

- **Include path**

Add:

```plaintext
  ..\lib-src\portmidi\pm_common
```

- **Library to link**
- Win32 Release/Debug:

```plaintext
    ..\lib-src\portmidi\release\portmidi_s.lib
```

- x64 Release:

```plaintext
    ..\lib-src\portmidi(x64)\x64\Release\portmidi-dynamic.lib
```

---

#### PortAudio

- **What it is**

A cross-platform, real-time audio I/O library.

- **Where to get it**

Clone or download PortAudio:

https://github.com/PortAudio/portaudio

- **Expected directory layout**

```plaintext
  lib-src/
    portaudio/
      include/                     ← contains portaudio.h and related headers
      build/
        msvc/
          Win32/
            Debug/                 ← portaudio_x86.lib
            Release/               ← portaudio_x86.lib
          x64/
            Release/               ← portaudio_x64.lib
```

- **Include path**

Add:

```plaintext
  ..\lib-src\portaudio\include
```

- **Library to link**
- Win32 Debug:

```plaintext
    ..\lib-src\portaudio\build\msvc\Win32\Debug\portaudio_x86.lib
```

- Win32 Release:

```plaintext
    ..\lib-src\portaudio\build\msvc\Win32\Release\portaudio_x86.lib
```

- x64 Release:

```plaintext
    ..\lib-src\portaudio\build\msvc\x64\Release\portaudio_x64.lib
```

---

#### spiwavsetlib

- **What it is**

A library for WAV-set sample playback used by the generator.

- **Where to get it**

Provided alongside the project in `spiwavsetlib_vs2017u`. If missing, clone or obtain from the project’s vendor repository.

- **Expected directory layout**

```plaintext
  spiwavsetlib_vs2017u/
    x86/
      debug\spiwavsetlib_vs2017.lib
      release\spiwavsetlib_vs2017.lib
    x64/
      debug\spiwavsetlib_vs2017.lib
      release\spiwavsetlib_vs2017.lib
  spiwavsetlib/                    ← public headers for spiwavsetlib
```

- **Include path**

Add:

```plaintext
  ..\spiwavsetlib
```

- **Library to link**
- Win32 Debug:

```plaintext
    ..\spiwavsetlib_vs2017u\x86\debug\spiwavsetlib_vs2017.lib
```

- Win32 Release:

```plaintext
    ..\spiwavsetlib_vs2017u\x86\release\spiwavsetlib_vs2017.lib
```

- x64 Release/Debug:

```plaintext
    ..\spiwavsetlib_vs2017u\x64\[debug|release]\spiwavsetlib_vs2017.lib
```

---

#### winmm.lib (Windows Multimedia)

- **What it is**

The Windows Multimedia library, provides timers and MIDI support.

- **Where it comes from**

Included with the Windows SDK; no external download required.

- **Library to link**

```plaintext
  winmm.lib
```

---

### Example Directory Structure

```text
spimidiminimalmusicgenerator_vs2017/
├─ lib-src/
│  ├─ portmidi/
│  │  ├─ pm_common/
│  │  │  └─ portmidi.h
│  │  ├─ release/
│  │  │  └─ portmidi_s.lib
│  │  └─ x64/
│  │     └─ x64 Release/
│  │        └─ portmidi-dynamic.lib
│  └─ portaudio/
│     ├─ include/
│     │  └─ portaudio.h
│     └─ build/
│        └─ msvc/
│           ├─ Win32/
│           │  ├─ Debug/portaudio_x86.lib
│           │  └─ Release/portaudio_x86.lib
│           └─ x64/
│              └─ Release/portaudio_x64.lib
├─ spiwavsetlib_vs2017u/
│  ├─ x86/
│  │  ├─ debug/spiwavsetlib_vs2017.lib
│  │  └─ release/spiwavsetlib_vs2017.lib
│  └─ x64/
│     ├─ debug/spiwavsetlib_vs2017.lib
│     └─ release/spiwavsetlib_vs2017.lib
├─ spiwavsetlib/                 ← headers for spiwavsetlib
│  └─ spiwavsetlib.h
├─ main.cpp
├─ stdafx.h
├─ stdafx.cpp
└─ spimidiminimalmusicgenerator.vcxproj
```

---

### Visual Studio Project Configuration

1. **Open** `spimidiminimalmusicgenerator.vcxproj` in Visual Studio 2017 or later.
2. **C/C++ → General → Additional Include Directories**

Add:

```plaintext
   .\;
   ..\lib-src\portmidi\pm_common;
   ..\lib-src\portaudio\include;
   ..\spiwavsetlib;
   %(AdditionalIncludeDirectories)
```

1. **Linker → Input → Additional Dependencies**

Add all required libraries:

```plaintext
   kernel32.lib;
   user32.lib;
   gdi32.lib;
   winspool.lib;
   shell32.lib;
   ole32.lib;
   oleaut32.lib;
   uuid.lib;
   comdlg32.lib;
   advapi32.lib;
   ..\lib-src\portmidi\release\portmidi_s.lib;
   winmm.lib;
   ..\spiwavsetlib_vs2017u\<Platform>\<Configuration>\spiwavsetlib_vs2017.lib;
   ..\lib-src\portaudio\build\msvc\<Platform>\<Configuration>\portaudio_<arch>.lib
```

Replace `<Platform>` (`Win32` or `x64`), `<Configuration>` (`Debug` or `Release`), and `<arch>` (`x86` or `x64`) accordingly.

1. **Linker → General → Additional Library Directories**

(Optional) Add the folders containing your `.lib` files, e.g.:

```plaintext
   ..\lib-src\portmidi\release;
   ..\spiwavsetlib_vs2017u\x64\release;
   ..\lib-src\portaudio\build\msvc\x64\Release
```

---

### Troubleshooting Build Errors

- **Missing header (**`**X.h**`** not found)**
- Verify the header exists under the include path (e.g., `lib-src\portmidi\pm_common\portmidi.h`).
- In Project Properties → C/C++ → General, ensure that directory is listed in **Additional Include Directories**.
- Rebuild the solution.

- **Unresolved external symbol (**`**xxx.lib**`** missing)**
- Confirm the `.lib` file is present in the expected folder.
- In Linker → Input → Additional Dependencies, add the precise filename (e.g., `portaudio_x64.lib`).
- If the library resides in a non-standard folder, add that folder under Linker → General → Additional Library Directories.
- Clean and rebuild.

---

```card
{
    "title": "Note on Platform Configurations",
    "content": "Be sure to match each library\u2019s architecture and build configuration (Debug vs Release, Win32 vs x64)."
}
```

With these settings in place, you should be able to compile and link **spimidiminimalmusicgenerator** successfully under Visual Studio 2017+.