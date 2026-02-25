## Quick Start: Generate MIDI Minimal-Music Patterns

Follow these examples to start generating minimalist MIDI patterns in either RANDOM or SEQUENTIAL mode. Substitute your actual MIDI output device name as listed by the “Enumerate MIDI Outputs” step.

### Prerequisites

- Build the project in Visual Studio 2017 (v141) to produce `spimidiminimalmusicgenerator.exe`.
- Ensure PortMidi is installed and your MIDI interface (e.g. MIDI Yoke, USB-MIDI) is connected and enabled.
- Note the exact device name from the enumeration tool (for example, `Out To MIDI Yoke:  1`).

### Argument Order

When invoking the executable, supply arguments in this exact order:

1. **noteset**
2. **note-change period** (seconds)
3. **loop duration** (seconds)
4. **MIDI output device name**
5. **output MIDI channel** (0–15)
6. **number of simultaneous notes**
7. **number of notes to change per tick**
8. **mode** (`RANDOM` or `SEQUENTIAL`)
9. **MIDI channel range** (0–16)

### Example: RANDOM Mode

This example plays 3 simultaneous notes chosen randomly from all octaves, swapping one note every 0.5 s, and runs for 60 s on MIDI channel 0. New notes will be assigned across channels 0–4.

```bash
spimidiminimalmusicgenerator.exe \
  ALL              \
  0.5              \
  60               \
  "Out To MIDI Yoke:  1" \
  0                \
  3                \
  1                \
  RANDOM           \
  4
```

- **noteset** = ALL (every note name across all octaves)
- **note-change period** = 0.5 s
- **loop duration** = 60 s
- **device** = Out To MIDI Yoke:  1
- **output channel** = 0
- **simultaneous notes** = 3
- **notes changed per tick** = 1
- **mode** = RANDOM
- **channel range** = 4 (cycles channels 0–4)

### Example: SEQUENTIAL Mode

This example walks through the C4 octave in order, playing a single note at a time, changing every 1 s for 30 s on MIDI channel 1:

```bash
spimidiminimalmusicgenerator.exe \
  C4               \
  1.0              \
  30               \
  "Out To MIDI Yoke:  1" \
  1                \
  1                \
  1                \
  SEQUENTIAL       \
  1
```

- **noteset** = C4 (all notes in octave 4)
- **note-change period** = 1.0 s
- **loop duration** = 30 s
- **device** = Out To MIDI Yoke:  1
- **output channel** = 1
- **simultaneous notes** = 1
- **notes changed per tick** = 1
- **mode** = SEQUENTIAL
- **channel range** = 1 (uses channel 1 only)

### What You’ll Hear & See

- On startup, the program builds Note On/Off events for the chosen noteset and opens the specified MIDI device.
- During initialization, it turns on the configured number of notes (Random or Sequential) with an initial delay equal to the note-change period.
- A Win32 timer fires every (note-change period) seconds, invoking `TimerProc`, which:
- Turns off the appropriate notes (`TurnRandomNoteOff` / `TurnNextNoteOff`).
- Turns on new notes (`PlayRandomNoteOn` / `PlayNextNoteOn`).
- In **RANDOM** mode, notes are selected and retired at random from the non-playing/playing pools.
- In **SEQUENTIAL** mode, notes advance through the ordered vector built from the noteset.
- The console prints the timer ID and continues until the global loop duration elapses, then all notes are turned off and the program exits.