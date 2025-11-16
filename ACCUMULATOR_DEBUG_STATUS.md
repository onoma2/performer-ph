# Accumulator Debug Status Update

**Date**: 2025-11-16
**Branch**: `claude/test-accumulator-mvp-01VANBTMxfReFi66x8h3PsKD`
**Issue**: UI parameter changes not affecting accumulator behavior

## Changes Made

### 1. Code Analysis Completed ✅

**Finding**: The implementation appears architecturally correct:
- UI properly connected to model via `NoteTrackListModel` (src/apps/sequencer/ui/model/NoteTrackListModel.h:191-196)
- Model properly stores parameters in `_accumDir` and `_accumValue` (src/apps/sequencer/model/NoteTrack.h:360-361)
- Engine reads fresh values from model on each iteration (src/apps/sequencer/engine/NoteTrackEngine.cpp:306-327)
- Serialization properly implemented with Version33 compatibility (src/apps/sequencer/model/NoteTrack.cpp:69-70, 89-90)

### 2. Debug Logging Added ✅

Added extensive debug output to trace the data flow:

**File**: `src/apps/sequencer/model/NoteTrack.h`
- Line 8: Added `#include "core/Debug.h"`
- Line 287: Log when `setAccumDir()` is called
- Line 303: Log when `setAccumValue()` is called

**File**: `src/apps/sequencer/engine/NoteTrackEngine.cpp`
- Lines 306-310: Read and log accumulator parameters from model
- Line 327: Log accumulator value after update

### 3. Documentation Created ✅

Created `ACCUMULATOR_DEBUG_GUIDE.md` with:
- Complete debugging procedure
- Expected debug output examples
- Diagnostic scenarios for common issues
- Code flow reference diagrams
- Timing and behavior notes

## Debug Output Reference

### When Parameters Change (Expected)
```
NoteTrack::setAccumDir(1) called     // 0=Up, 1=Down, 2=Freeze
NoteTrack::setAccumValue(5) called   // 0-7
```

### Each Iteration (Expected)
```
Track 0: Iteration 2, AccumDir=0, AccumValue=1, AccumCurrent (before)=1
Track 0: AccumCurrent (after)=2
```

## Next Steps for User

### 1. Build Firmware

**For hardware:**
```bash
cd build/stm32/release
make -j sequencer
make flash_sequencer
```

**For simulator (if available):**
```bash
cd build/sim/debug
make -j sequencer
./src/apps/sequencer/sequencer
```

### 2. Connect to Debug Output

- **Hardware**: Connect to debug UART (115200 baud, 8N1)
- **Simulator**: Check terminal/console output

### 3. Run Test Sequence

1. Start with simple 4-step sequence
2. Start playback, observe baseline (should increment by 1)
3. Change `Accum Value` from 1 to 5
4. Check for `setAccumValue(5)` message
5. Wait for next iteration
6. Check if `AccumValue=5` appears in iteration log
7. Verify accumulator increases by 5

### 4. Report Findings

Collect and share:
- Complete debug output from test session
- Which messages appear/don't appear
- Observed vs expected behavior
- Any patterns or anomalies

## Possible Outcomes

### Outcome A: Setters Not Called
- **Symptom**: No `setAccumDir/Value()` messages appear
- **Cause**: UI → Model connection issue
- **Next**: Investigate NoteTrackListModel event handling

### Outcome B: Setters Called, Wrong Values in Engine
- **Symptom**: `setAccumValue(5)` appears but engine shows `AccumValue=1`
- **Cause**: Model value not persisting or wrong instance
- **Next**: Add logging to verify `_accumValue` member persistence

### Outcome C: Correct Values, No Behavior Change
- **Symptom**: `AccumValue=5` in log but accumulator only increases by 1
- **Cause**: Switch statement or logic error
- **Next**: Inspect compiled code or add more granular logging

### Outcome D: Works After Restart Only
- **Symptom**: Changes only apply after stop/start
- **Cause**: Values cached or only read during initialization
- **Next**: Check when `_noteTrack` reference is set/updated

## Code Architecture Notes

### Data Flow: UI → Model → Engine

```
┌─────────────────┐
│  User Input     │ (Encoder turn on Track page)
└────────┬────────┘
         │
         ▼
┌─────────────────────────────┐
│  NoteTrackListModel::edit() │ (UI layer)
└────────┬────────────────────┘
         │
         ▼
┌──────────────────────────────┐
│  NoteTrack::setAccumValue()  │ (Model layer)
│  _accumValue = new_value     │
│  [DBG OUTPUT HERE]           │
└──────────────────────────────┘
         │
         │ (stored in model)
         │
         ▼
┌─────────────────────────────────┐
│  NoteTrackEngine::triggerStep() │ (Engine layer)
│  reads _noteTrack.accumValue()  │
│  [DBG OUTPUT HERE]              │
│  applies to _accumCurrent       │
└─────────────────────────────────┘
```

### Important Timing Constraint

**Accumulator updates ONLY on iteration change:**
- An iteration = one complete loop through the sequence
- For a 4-step sequence, iteration changes every 4 steps
- Parameter changes take effect on NEXT iteration boundary
- This is by design, not a bug

**Example Timeline:**
```
Step 1,2,3,4 → Iteration 1 starts → Accum updates → Steps play
Step 1,2,3,4 → Iteration 2 starts → Accum updates → Steps play
    ↑
    └─ If you change parameter at step 2,
       it won't apply until Iteration 3
```

## Implementation Correctness

After thorough code review:

✅ Model correctly stores parameters
✅ Model correctly serializes parameters (Version33)
✅ UI correctly connected to model setters
✅ Engine reads fresh values each iteration (not cached)
✅ Switch statement logic is correct
✅ Accumulator properly initialized in reset/restart

**Conclusion**: The code *should* work as designed. Debug output will reveal where the disconnect is happening.

## Files in This Debug Session

1. **ACCUMULATOR_DEBUG_GUIDE.md** - Complete debugging guide
2. **ACCUMULATOR_DEBUG_STATUS.md** - This status document
3. **src/apps/sequencer/model/NoteTrack.h** - Added debug logging
4. **src/apps/sequencer/engine/NoteTrackEngine.cpp** - Added debug logging

## Git Commits

```
97fcab5 - Add comprehensive debugging guide for accumulator parameter issue
4bc7464 - Add debug logging to diagnose accumulator parameter issue
```

## Questions to Answer with Debug Output

1. ✅ Are the UI setters being called when parameters change?
2. ✅ What values are being stored in the model?
3. ✅ What values is the engine reading from the model?
4. ✅ Is the switch statement executing with the correct enum value?
5. ✅ Is _accumCurrent being modified correctly?
6. ✅ Is there a timing issue (changes mid-iteration vs iteration boundary)?

The debug output will definitively answer all of these questions.

## Contact

If you need additional debugging, modifications, or have questions about the output, provide:
- Complete debug log from your test session
- Description of what you did (step by step)
- What you expected vs what you observed
