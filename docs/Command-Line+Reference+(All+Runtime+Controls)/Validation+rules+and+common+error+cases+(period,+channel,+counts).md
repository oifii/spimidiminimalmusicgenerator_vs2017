## Command-Line Reference (All Runtime Controls)

This section documents the practical constraints imposed by the application on its runtime controls. Violating any of these validation rules causes the program to print an error message and immediately terminate. If you encounter one of these errors, adjust the corresponding command-line argument as described below.

### Affected Arguments

- **noteset** (argv[1])
- **notechangeperiod_s** (argv[2])
- **outputmidichannel** (argv[5])
- **numberofsimultaneousnotes** (argv[6])
- **numberofnotesonchange** (argv[7])

### Validation Rules

1. **Noteset Size vs. Simultaneous Notes**

The total number of notes in the chosen `noteset` must be at least equal to the requested `numberofsimultaneousnotes`.

```cpp
   if (noteset.size() < numberofsimultaneousnotes)
   {
       cout << "invalid noteset, the set of notes must be at least equal to the number of sultaneous notes to be played" << endl;
       Terminate();
   }
```

1. **Notes-on-Change Count vs. Simultaneous Notes**

The number of notes to change on each tick (`numberofnotesonchange`) must not exceed the number of notes played simultaneously.

```cpp
   if (numberofnotesonchange > numberofsimultaneousnotes)
   {
       cout << "invalid numberofnotesonchange, the number of notes to be changed must be at most equal to the number of sultaneous notes to be played" << endl;
       Terminate();
   }
```

1. **Minimum Note-Change Period**

To avoid overloading the MIDI stream, the period between note-change events (`notechangeperiod_s`) must be strictly greater than 0.005 seconds (5 ms).

```cpp
   if (notechangeperiod_s <= 0.005)
   {
       cout << "invalid notechangeperiod_s, note change should not occur faster than 5 ms" << endl;
       Terminate();
   }
```

1. **MIDI Channel Bounds**

MIDI channels are numbered 0–15. The chosen channel (`outputmidichannel`) must fall within this inclusive range.

```cpp
   if (outputmidichannel < 0 || outputmidichannel > 15)
   {
       cout << "invalid outputmidichannel, midi channel must range from 0 to 15 inclusively." << endl;
       Terminate();
   }
```

### Common Error Messages and Remedies

| Error Message | Cause | Resolution |  |
| --- | --- | --- | --- |
| invalid noteset, the set of notes must be at least equal to the number of sultaneous notes to be played | `noteset.size() < numberofsimultaneousnotes` | Increase the size of your `noteset` (e.g., include more note names or use `ALL`), or reduce `numberofsimultaneousnotes`. |  |
| invalid numberofnotesonchange, the number of notes to be changed must be at most equal to the number of sultaneous notes to be played | `numberofnotesonchange > numberofsimultaneousnotes` | Decrease `numberofnotesonchange` so it does not exceed `numberofsimultaneousnotes`. |  |
| invalid notechangeperiod_s, note change should not occur faster than 5 ms | `notechangeperiod_s <= 0.005` | Specify a larger period (e.g., `0.01` for 10 ms) as the second command-line argument. |  |
| invalid outputmidichannel, midi channel must range from 0 to 15 inclusively. | `outputmidichannel < 0 | outputmidichannel > 15` | Choose a valid MIDI channel number between 0 and 15 (inclusive) for the sixth command-line argument. |


Whenever an error is printed, the function `Terminate()` is called immediately after the message, causing the application to clean up and exit. Adjust your command-line arguments according to the guidelines above to avoid these immediate-termination cases.