## How to Configure Musical Behavior (Practical Tuning Guide)

This section explains the two built-in note-selection modes—**RANDOM** and **SEQUENTIAL**—and how each evolves your MIDI pattern over time. You’ll learn what happens musically when you switch modes, which underlying functions drive the behavior, and a key current limitation of the sequential mode.

---

### RANDOM Mode

In **RANDOM** mode, each on/off event picks a note at random from the pool of currently available pitches, producing an ever-varying, unpredictable texture.

- **On-Event (**`**PlayRandomNoteOn**`**)**
- Chooses a random index into `global_nonplayingnotesvector`.
- Moves that MIDI note number into `global_nowplayingnotesvector`.
- Sends the corresponding Note-On `PmEvent`.
- Citation:

- **Off-Event (**`**TurnRandomNoteOff**`**)**
- Chooses a random index into `global_nowplayingnotesvector`.
- Moves that MIDI note number back into `global_nonplayingnotesvector`.
- Sends the corresponding Note-Off `PmEvent`.
- Citation:

Musical result:

- Notes never repeat until all others have been played (a “shuffle” of the active set).
- Multiple simultaneous notes (chords) are supported by simply invoking these functions in a loop at each timer tick.
- Use this mode for ambient washes or unpredictable melodic fragments.

---

### SEQUENTIAL Mode

In **SEQUENTIAL** mode, the pattern walks through your configured note set in order—much like a simple step-sequencer.

- **On-Event (**`**PlayNextNoteOn**`**)**
- Increments `global_sequential_integer`, wrapping back to 1 at the end of `global_sequential_notesvector`.
- Takes the note at that index and pushes it into `global_nowplayingnotesvector`.
- Sends its Note-On `PmEvent`.
- Citation:

- **Off-Event (**`**TurnNextNoteOff**`**)**
- Uses the same `global_sequential_integer` to identify which note to turn off.
- Removes the oldest (index 0) note from `global_nowplayingnotesvector` and returns it to `global_nonplayingnotesvector`.
- Sends its Note-Off `PmEvent`.
- Citation:

Musical result:

- You hear a continuous “round‐robin” traversal of your note list.
- Predictable, repeating cycles—ideal for patterns, ostinatos, or when you need strict pitch order.

**Current Constraint:**

Sequential mode always erases `global_nowplayingnotesvector.begin()` (index 0) in both on and off steps, effectively supporting only **one** simultaneous note. If you set `numberofsimultaneousnotes > 1`, Sequential will still play one note at a time, stepping through the list in order .

---

### Practical Tuning Tips

- **Choose RANDOM** when you want evolving, generative textures that never quite repeat.
- **Choose SEQUENTIAL** for mechanical, step-by-step patterns—like a simple arpeggiator.
- **Simultaneity vs. Change Rate**
- `numberofsimultaneousnotes`: how many notes sound together.
- `numberofnotesonchange`: how many notes you turn off and back on each tick.
- **Timebase**
- `notechangeperiod_s`: controls the groove—longer periods yield slow shifts, shorter yield rapid movement.
- Combine modes by switching at runtime (via command-line argument) to move between ordered and random behavior without restarting.

---

### Code Reference

```cpp
// RANDOM mode
bool PlayRandomNoteOn();    // picks and plays a random new note 
bool TurnRandomNoteOff();   // picks and stops a random currently playing note 

// SEQUENTIAL mode
bool PlayNextNoteOn();      // advances and plays the next note in sequence 
bool TurnNextNoteOff();     // stops the oldest playing note (only one supported) 
```

Use these tuning guidelines to sculpt your MIDI output—whether you want the unpredictability of shuffled note sets or the steady pulse of an ordered sequence.