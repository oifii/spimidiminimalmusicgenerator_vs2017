## Command-Line Reference (All Runtime Controls)

The application supports nine positional command-line arguments that control its runtime behavior. Any argument omitted will use its predefined default. Arguments must be supplied in order, without flags.

### Usage

```text
spimidiminimalmusicgenerator.exe
  [noteset]
  [notechangeperiod_s]
  [global_loopduration_s]
  [midioutputdevicename]
  [outputmidichannel]
  [numberofsimultaneousnotes]
  [numberofnotesonchange]
  [mode]
  [outputmidichannelrange]
```

### Arguments

| Position | Name | Default | Description |
| --- | --- | --- | --- |
| 1 | noteset | `"C3,D3,E3,F3,G3,A3,B3"` | Comma-separated list of note names or octave specifier. Special values: `ALL` or `-2` for all notes across all octaves; a single note name (e.g. `C#`) for all octaves; or a number (-2…9) for all notes in that octave . |
| 2 | notechangeperiod_s | `2.0` | Period (in seconds) between note-change events. Must be greater than 0.005 s or the application will terminate with an error . |
| 3 | global_loopduration_s | `300.0` | Total runtime (in seconds) before automatic termination. A negative value loops indefinitely . |
| 4 | midioutputdevicename | `"Out To MIDI Yoke:  1"` | Exact name of the PortMidi output device to use. The program enumerates available output devices at startup and maps this name to its device ID . |
| 5 | outputmidichannel | `1` | MIDI channel on which to send Note On/Off messages. Valid range: 0–15 inclusive. Values outside this range cause termination . |
| 6 | numberofsimultaneousnotes | `5` | Number of notes played at once. Must be ≤ size of the noteset or the application will terminate . |
| 7 | numberofnotesonchange | `2` | Number of notes to replace on each change event. Must be ≤ numberofsimultaneousnotes or the application will terminate . |
| 8 | mode | `"RANDOM"` | Note-selection mode. Supported values: |


  • `RANDOM` — each change picks notes at random.

  • `SEQUENTIAL` — each change advances through the noteset in order.

Any value other than `RANDOM` is coerced to `SEQUENTIAL` , using initial default `global_modestring = "RANDOM"` . |

| 9 | outputmidichannelrange | `0` | Range for randomizing the output MIDI channel. Parsed value is clamped to [0…16] . |
| --- | --- | --- | --- |


---

### Mode Parsing Behavior

When a `mode` argument is provided (`argv[8]`), the application assigns it to `global_modestring`. If the provided string is not exactly `"RANDOM"`, it will override `global_modestring` to `"SEQUENTIAL"` .

### MIDI Output Channel Range Clamping

The `outputmidichannelrange` argument (`argv[9]`) is parsed as an integer and then clamped:

- Values < 0 become 0.
- Values > 16 become 16.

The clamped result is stored in `global_outputmidichannelrange` .