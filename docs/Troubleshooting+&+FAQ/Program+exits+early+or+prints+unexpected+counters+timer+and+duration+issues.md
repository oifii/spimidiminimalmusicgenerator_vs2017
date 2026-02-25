## Troubleshooting & FAQ – Program exits early or prints unexpected counters

This section covers common runtime surprises related to the Win32 timer and loop-duration logic:

- The program exits immediately or returns exit code 16
- You see unexpected “Counter” messages or unusually rapid counter increments
- The run stops after an unexpected elapsed time

All of these typically trace back to how the timer is created via SetTimer and how TimerProc decides to shut down when the configured duration is reached.

---

### 1. Program exits immediately with exit code 16

**Symptom**

On startup you see:

```plaintext
TimerId: 0
```

and the console process terminates immediately, returning code 16.

**Cause**

`SetTimer` failed and returned 0. In main.cpp, the code is:

```cpp
global_TimerId = SetTimer(NULL, 1, timerelapse_ms, (TIMERPROC)&TimerProc);
cout << "TimerId: " << global_TimerId << '\n';
if (!global_TimerId)
    return 16;
```

A zero `timerelapse_ms` (or one below the minimum Windows resolution) causes `SetTimer` to fail .

**Resolution**

- Ensure your note-change period (`notechangeperiod_s`) multiplied by 1000 yields at least 1 ms. Because of integer truncation:

```cpp
UINT timerelapse_ms = (UINT)(notechangeperiod_s * 1000);
```

even `notechangeperiod_s = 0.0005f` becomes `0`. Use a larger period (e.g. ≥ 0.001 s) or add a small bias when casting:

```cpp
UINT timerelapse_ms = (UINT)(notechangeperiod_s * 1000 + 0.5);
```

Also remember that Windows’ minimum timer resolution is typically ~10 ms; requesting anything smaller may be rounded up or lead to unreliable behavior.

---

### 2. Unexpected “Counter” messages or rapid increments

**Symptom**

Rather than seeing roughly one counter increment per second (or per your period), you observe counters increasing at a very high rate, or printed in bursts.

**Cause**

If `timerelapse_ms` is too small (or zero), the system will queue WM_TIMER messages as fast as it can, flooding your message loop:

```cpp
while (GetMessage(&Msg, NULL, 0, 0)) {
    ++Counter;
    if (Msg.message == WM_TIMER) {
        cout << "Counter: " << Counter << "; timer message\n";
        …
    }
    else {
        cout << "Counter: " << Counter << "; message: " << Msg.message << '\n';
    }
    DispatchMessage(&Msg);
}
```

Each queued WM_TIMER bumps `Counter` and triggers note-change logic .

**Resolution**

- Choose a realistic `notechangeperiod_s`. For musical patterns, values under 0.01 s are uncommon.
- Guard against tiny periods by validation in main():

```cpp
if (notechangeperiod_s <= 0.005f) {
    cout << "invalid notechangeperiod_s, must be > 5 ms\n";
    Terminate();
}
```

This prevents timer flooding by enforcing a floor on the period .

---

### 3. Program stops after an unexpected duration

**Symptom**

The generator runs correctly for a short while, then stops and the console window closes—often with a final log line like:

```plaintext
Time: 12.3456 sec
```

or

```plaintext
Counter: 123; message: 16
```

**Cause**

The timer callback checks if the elapsed time exceeds the configured loop duration and, if so, kills the timer and posts a WM_CLOSE to the console window:

```cpp
VOID CALLBACK TimerProc(..., DWORD dwTime) {
    float totalduration_s = (dwTime - global_dwStartTime_ms) / 1000.0;
    if (global_loopduration_s > 0.0f
        && totalduration_s > global_loopduration_s)
    {
        KillTimer(NULL, global_TimerId);
        global_TimerId = 0;
        HWND hWnd = ::GetConsoleWindow();
        ::PostMessage(hWnd, WM_CLOSE, 0, 0);
    }
    else {
        cout << "Time: " << totalduration_s << " sec\n";
        cout.flush();
    }
}
```

When WM_CLOSE (value 16) is pulled from the queue, the main loop dispatches it and then exits `GetMessage`, leading to cleanup in `Terminate()` .

**Resolution**

- If you want the program to run indefinitely, set `global_loopduration_s` to a negative value (the default is 300 s).
- To extend or shorten the run, pass a third command-line argument:

```plaintext
spimidiminimalmusicgenerator_vs2017.exe "<noteset>" <period_s> <loop_duration_s>
```

Ensure the third argument is > 0 for timed runs, or < 0 for infinite looping.

---

## FAQ

**Q1: Why do I sometimes see “Counter: N; message: 16”?**

> That “16” is the WM_CLOSE message code, posted by TimerProc when the elapsed time exceeded global_loopduration_s . It signals the app to shut down.

**Q2: I set a very small period (e.g. 0.001 s) but the counter still jumps too fast. What gives?**

> Windows timers have a minimum granularity (often ~10 ms). Requesting less may either be rounded up or generate bursts. Pick a period ≥ 0.010 s, or enforce your own minimum in code.

**Q3: My timer never fires and the console sits idle—no errors.**

> Likely `SetTimer` returned 0 due to a zero/invalid period. Check the printed `TimerId`. If it’s “0”, fix the period calculation so `timerelapse_ms >= 1` .

---

Keeping these checks in mind will help avoid premature exits and keep your note-pattern generator running at the expected cadence.