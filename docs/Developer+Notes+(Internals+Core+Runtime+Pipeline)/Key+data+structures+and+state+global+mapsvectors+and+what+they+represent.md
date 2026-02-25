# Developer Notes (Internals: Core Runtime Pipeline)

## Infrastructure Dependencies

This feature is implemented as a standalone C++ console application and does not inject or depend on external infrastructure services such as logging frameworks, message buses, or background job dispatchers. All interactions are with the PortMidi library (for MIDI I/O) and the Win32 timer/message‐loop API.

## Key Data Structures and State

The core runtime pipeline revolves around a small set of global maps, vectors, and counters that together maintain the current and pending MIDI note state, device lookup, and playback mode. Future maintainers can extend behavior safely by respecting the invariants described below.

### Variable Declarations

All of these globals are declared at the top of `main.cpp`:

```cpp
PmStream*                 global_pPmStream          = NULL;      // PortMidi output stream
map<int, PmEvent*>        global_noteonmap;                     // midinote → Note-On event
map<int, PmEvent*>        global_noteoffmap;                    // midinote → Note-Off event
vector<int>               global_nonplayingnotesvector;        // notes available to turn on
vector<int>               global_nowplayingnotesvector;        // notes currently on
map<string,int>           global_midioutputdevicemap;          // name → device ID
int                       global_sequential_integer = 0;       // next index into sequential list
vector<int>               global_sequential_notesvector;       // ordered list for SEQUENTIAL mode
string                    global_modestring         = "RANDOM";// playback mode
UINT                      global_TimerId            = 0;         // Win32 timer ID
DWORD                     global_dwStartTime_ms;               // loop start timestamp
float                     global_loopduration_s;                // optional loop duration
int                       global_outputmidichannel  = -1;       // MIDI channel (0–15)
```

---

### 1. global_noteonmap / global_noteoffmap

- **Type:** `map<int,PmEvent*>`
- **Purpose:** Prebuilt `PmEvent` pointers for each configured MIDI note:
- `global_noteonmap[n]` holds a `PmEvent` whose `.message` is a Note-On (velocity > 0) for note `n`.
- `global_noteoffmap[n]` holds a `PmEvent` whose `.message` is a Note-On with velocity 0 (i.e., Note-Off) for note `n`.
- **Invariant:**
- Both maps contain exactly one entry per note in the initial note set.
- Keys range from 0 to 127 and match those inserted during setup.
- **Initialization:**

In the MIDI‐definition loop, for each `midinotenumber` parsed from `noteset`:

```cpp
  // Note-On event
  pPmEvent = new PmEvent{0, Pm_Message(0x90+outputmidichannel, midinotenumber, 100)};
  global_noteonmap.insert({midinotenumber, pPmEvent});
  // Note-Off event
  pPmEvent = new PmEvent{0, Pm_Message(0x90+outputmidichannel, midinotenumber,   0)};
  global_noteoffmap.insert({midinotenumber, pPmEvent});
```

---

### 2. global_nonplayingnotesvector / global_nowplayingnotesvector

- **Type:** `vector<int>`
- **Purpose:**
- `global_nonplayingnotesvector` holds notes that are available to be turned on.
- `global_nowplayingnotesvector` holds notes that have been turned on and not yet turned off.
- **Invariant:**
- The two vectors form a partition of the entire note set: every note is either in the “nonplaying” vector or the “nowplaying” vector, never both.
- At startup, all notes appear in `global_nonplayingnotesvector`, and `global_nowplayingnotesvector` is empty.
- **Runtime Behavior:**
- **Random Mode:**
- `PlayRandomNoteOn()`
- Pick a random index in `global_nonplayingnotesvector`.
- Remove that note from `global_nonplayingnotesvector` and append it to `global_nowplayingnotesvector`.
- Look up its Note-On event in `global_noteonmap` and send via `Pm_Write`.
- `TurnRandomNoteOff()` does the inverse: pick a random note from `global_nowplayingnotesvector`, swap it back to the nonplaying vector, and send the Note-Off event from `global_noteoffmap`.
- **Sequential Mode:**
- See section 4 below for interaction with `global_sequential_integer` and `global_sequential_notesvector`.

---

### 3. global_midioutputdevicemap

- **Type:** `map<string,int>`
- **Purpose:** Maps the human‐readable PortMidi device name to its integer device ID.
- **Initialization:**

Enumerated once at startup:

```cpp
  for (int i = 0; i < Pm_CountDevices(); i++) {
      const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
      if (info->output) {
          global_midioutputdevicemap.insert({info->name, i});
      }
  }
```

- **Lookup Invariant:** If the user’s chosen `midioutputdevicename` exists in the map, it overrides the default ID.

---

### 4. global_sequential_integer / global_sequential_notesvector

- **Type:**
- `int global_sequential_integer`
- `vector<int> global_sequential_notesvector`
- **Purpose:**
- Provides deterministic, ordered note selection in SEQUENTIAL mode.
- `global_sequential_notesvector` mirrors the initial note set in the order parsed from the `noteset` string.
- `global_sequential_integer` tracks the next 1-based index into that vector.
- **Invariant:**
- `1 ≤ global_sequential_integer ≤ global_sequential_notesvector.size()`.
- After each `PlayNextNoteOn()`, the integer is incremented and wraps back to 1 when exceeding the vector length.
- **Runtime Methods:**

```cpp
  bool PlayNextNoteOn() {
      global_sequential_integer++;
      if (global_sequential_integer > global_sequential_notesvector.size())
          global_sequential_integer = 1;
      int note = global_sequential_notesvector[global_sequential_integer - 1];
      // ... move note from nonplaying→nowplaying, send Note-On ...
  }
  bool TurnNextNoteOff() {
      if (global_sequential_integer < 1 ||
          global_sequential_integer > global_sequential_notesvector.size())
          return false;
      int note = global_sequential_notesvector[global_sequential_integer - 1];
      // ... move note from nowplaying→nonplaying, send Note-Off ...
  }
```

---

## Invariants & Extension Guidelines

- **Mutual Exclusivity:** Always enforce that each note exists in exactly one of `global_nonplayingnotesvector` or `global_nowplayingnotesvector`. Any future routine that rebalances notes must preserve this invariant.
- **Map Integrity:** Do not modify or delete entries in `global_noteonmap` / `global_noteoffmap` after initialization; they are deallocated only once in `Terminate()`.
- **Sequential Counter Bounds:** Any future operations that manipulate `global_sequential_integer` must wrap within `[1, N]` where `N == global_sequential_notesvector.size()`.
- **Mode Switching:** If runtime support for switching `global_modestring` between `"RANDOM"` and `"SEQUENTIAL"` is added, ensure consistent state migration—e.g., reconcile `global_nowplayingnotesvector` with the current index in the sequential list.

Maintaining these core data‐structure invariants will ensure predictable and safe extension of playback behaviors, whether adding new modes, supporting polyphony > 1 in sequential mode, or integrating dynamic note‐set updates.