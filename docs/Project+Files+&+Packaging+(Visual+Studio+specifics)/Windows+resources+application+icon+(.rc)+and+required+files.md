# Project Files & Packaging (Visual Studio specifics)

This section describes how the Windows resource files—specifically the application icon—are organized, referenced, and built in the `spimidiminimalmusicgenerator_vs2017` project. You’ll learn how `audio_spi.rc`, `resource.h`, and `audio_spi.ico` work together, how they’re included in the Visual Studio project, and how to troubleshoot common build errors if those files are missing or need to be replaced.

## Windows Resource Script (`audio_spi.rc`)

The Visual C++ resource compiler uses the `.rc` script to embed icons, cursors, dialogs, strings, and other resources into the executable. In this project, `audio_spi.rc` defines the main application icon.

```rc
// audio_spi.rc
#include <WinResRc.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
//
// Icon
//

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
IDI_MAIN_ICON           ICON    "audio_spi.ico"
```

- **IDI_MAIN_ICON**

The resource identifier for the application icon. It must match the definition in `resource.h`.

- **ICON "audio_spi.ico"**

The filename of the `.ico` file. This path is relative to the location of `audio_spi.rc`.

## Resource Header (`resource.h`)

`resource.h` provides symbolic constants for all resource IDs used by the `.rc` script.

```c
// resource.h
#define IDC_STATIC    -1
#define IDI_MAIN_ICON 101
```

- **IDC_STATIC**

A placeholder identifier, typically used for static controls.

- **IDI_MAIN_ICON**

Must match the same value used in `audio_spi.rc`. By convention, application icons often start at `101`.

## Icon File (`audio_spi.ico`)

- A standard Windows icon file containing one or more image formats (e.g., 16×16, 32×32, 48×48, 256×256).
- Placed in the project root alongside `audio_spi.rc` and `resource.h`.
- If you wish to replace the application icon:
- Create or obtain a new `.ico` file.
- Copy it into the project folder.
- Update the filename in `audio_spi.rc`:

```diff
     - IDI_MAIN_ICON ICON "audio_spi.ico"
     + IDI_MAIN_ICON ICON "my_new_icon.ico"
```

1. Right-click the `.rc` file in Solution Explorer and choose **Refresh**.

## Visual Studio Project Integration

### Including the Resource Script

In the `.vcxproj` file, ensure the resource script is compiled:

```xml
<ItemGroup>
  <ResourceCompile Include="audio_spi.rc" />
</ItemGroup>
```

If you view the project filters (`.vcxproj.filters`), you’ll see:

```xml
<ItemGroup>
  <ResourceCompile Include="audio_spi.rc">
    <Filter>Resource Files</Filter>
  </ResourceCompile>
</ItemGroup>
```

This places `audio_spi.rc` under the **Resource Files** filter in Solution Explorer.

### Build Settings

No additional linker flags are required for icon resources—the resource compiler embeds them automatically. Just verify under the **Linker → System** settings that the subsystem is set to **Console**, as this is a console application.

## Troubleshooting Build Errors

### Missing `resource.h`

Error:

```plaintext
fatal error RC1015: cannot open include file 'resource.h'
```

**Resolution:**

- Confirm that `resource.h` exists in the same directory as `audio_spi.rc`.
- In the resource script, ensure the include path is correct:

```rc
  #include "resource.h"
```

- If `resource.h` lives in a subfolder, adjust the path or add an Additional Include Directory under **Project → Properties → Resources → General**.

### Missing Icon File

Error:

```plaintext
fatal error RC1013: file name audio_spi.ico not found
```

**Resolution Options:**

1. **Restore or Replace the Icon**
2. Copy `audio_spi.ico` back into the project folder.
3. Or replace it with a new `.ico` and update the `.rc` script accordingly.

1. **Remove the Icon Resource**

If you do not need a custom icon:

- Open `audio_spi.rc` and comment out or delete the `IDI_MAIN_ICON` line:

```diff
     - IDI_MAIN_ICON ICON "audio_spi.ico"
```

- In the `.vcxproj`, remove the `<ResourceCompile Include="audio_spi.rc" />` entry, or remove only the icon block in `audio_spi.rc`.

## Summary

- **audio_spi.rc** declares the icon resource using the `IDI_MAIN_ICON` identifier.
- **resource.h** assigns the numeric ID (`101`) to `IDI_MAIN_ICON`.
- **audio_spi.ico** must exist alongside the `.rc` and `.h` files.
- The `.vcxproj` includes `audio_spi.rc` under **ResourceCompile** so that the icon is embedded.
- If either `resource.h` or the `.ico` file is missing, adjust include paths, restore the files, or remove the icon block to resolve build errors.