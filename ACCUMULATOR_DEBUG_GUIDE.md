# Accumulator Debugging Guide

## Issue Description

**Problem**: UI parameter changes for `Accum Dir` and `Accum Value` display correctly in the Track page, but don't affect actual accumulator behavior. The accumulator continues to increment by 1 on each loop regardless of parameter changes.

**Expected Behavior**:
- Changing `Accum Dir` should change direction (Up/Down/Freeze)
- Changing `Accum Value` should change the increment/decrement amount (0-7)
- Changes should take effect on the next sequence iteration (loop)

## Debug Changes Added

### Files Modified

1. **src/apps/sequencer/model/NoteTrack.h**
   - Added `#include "core/Debug.h"` (line 8)
   - Added debug logging to `setAccumDir()` (line 287)
   - Added debug logging to `setAccumValue()` (line 303)

2. **src/apps/sequencer/engine/NoteTrackEngine.cpp**
   - Added debug logging in `triggerStep()` accumulator update section (lines 306-327)
   - Logs accumulator state before and after each iteration change

### Debug Output Messages

#### 1. UI Parameter Changes

When you change accumulator parameters via the UI, you should see:

```
NoteTrack::setAccumDir(0) called    // 0=Up, 1=Down, 2=Freeze
NoteTrack::setAccumValue(5) called  // 0-7
```

**What this tells us:**
- Confirms the UI is calling the setter methods
- Shows the actual value being stored in the model

#### 2. Engine Iteration Updates

On each sequence loop iteration, you should see:

```
Track 0: Iteration 2, AccumDir=0, AccumValue=1, AccumCurrent (before)=1
Track 0: AccumCurrent (after)=2
```

**What this tells us:**
- Which track is updating
- Current iteration number
- Value of AccumDir being read from model (0=Up, 1=Down, 2=Freeze)
- Value of AccumValue being read from model (0-7)
- Accumulator value before the update
- Accumulator value after the update

## How to Use This Debug Output

### Step 1: Build and Flash

```bash
# For hardware
cd build/stm32/release
make -j sequencer
make flash_sequencer

# For simulator (if SDL2 is available)
cd build/sim/debug
make -j sequencer
./src/apps/sequencer/sequencer
```

### Step 2: Connect to Debug Output

**Hardware:**
- Connect USB serial adapter to debug UART pins
- Open serial terminal (115200 baud, 8N1)
- Or use OpenOCD + GDB with semihosting

**Simulator:**
- Debug output appears in terminal/console where simulator was launched

### Step 3: Test Sequence

1. **Start with defaults:**
   - Create a simple note sequence (e.g., 4 steps)
   - Set sequence to loop (not one-shot)
   - Start playback
   - Observe initial debug output

2. **Verify baseline behavior:**
   - Let it loop 3-4 times
   - Watch the iteration counter and accumulator value
   - Default should be: Dir=Up (0), Value=1
   - AccumCurrent should increment: 0, 1, 2, 3, 4...

3. **Test UI parameter changes:**
   - Navigate to Track page
   - Change `Accum Value` from 1 to 5
   - **Look for:** `NoteTrack::setAccumValue(5) called`
   - Wait for next loop iteration
   - **Look for:** `AccumValue=5` in iteration message
   - **Verify:** AccumCurrent increases by 5 instead of 1

4. **Test direction change:**
   - Change `Accum Dir` from Up to Down
   - **Look for:** `NoteTrack::setAccumDir(1) called`
   - Wait for next loop iteration
   - **Look for:** `AccumDir=1` in iteration message
   - **Verify:** AccumCurrent decreases instead of increases

5. **Test freeze:**
   - Change `Accum Dir` to Freeze
   - **Look for:** `NoteTrack::setAccumDir(2) called`
   - Wait for next loop iteration
   - **Look for:** `AccumDir=2` in iteration message
   - **Verify:** AccumCurrent stays the same

## Diagnostic Scenarios

### Scenario A: Setters Not Called

**Symptoms:**
- No `setAccumDir()` or `setAccumValue()` messages when changing UI

**Indicates:**
- UI is not properly connected to model
- Event handler not triggering
- Issue in NoteTrackListModel.cpp `editValue()` method

**Action:**
- Check if other track parameters work (transpose, octave, etc.)
- Verify UI page is editing the correct track instance

### Scenario B: Setters Called, But Wrong Values in Engine

**Symptoms:**
- `setAccumValue(5)` appears
- But iteration log shows `AccumValue=1`

**Indicates:**
- Model value not persisting
- Engine reading from wrong track instance
- Memory corruption or race condition

**Action:**
- Add more logging to verify `_accumValue` member variable
- Check if multiple NoteTrack instances exist
- Verify `_noteTrack` reference in engine points to correct model

### Scenario C: Correct Values, But No Behavior Change

**Symptoms:**
- `setAccumValue(5)` appears
- Iteration log shows `AccumValue=5`
- But `AccumCurrent (after)` only increases by 1

**Indicates:**
- Switch statement not executing correctly
- Compiler optimization issue
- Logic error in accumulator update code

**Action:**
- Verify enum values match switch cases
- Check if code is optimized out (unlikely with Debug.h)
- Inspect assembly/disassembly

### Scenario D: Changes Apply After Restart Only

**Symptoms:**
- Parameter changes don't take effect during playback
- But work correctly after stopping and restarting

**Indicates:**
- Values only read during reset/restart
- Caching issue
- `_noteTrack` reference not updated

**Action:**
- Check when `_noteTrack` reference is set
- Verify it's not copied/cached

## Important Timing Notes

### When Do Changes Take Effect?

The accumulator updates **only when the iteration changes**, which happens when:
- The sequence completes a full loop
- The `_sequenceState.iteration()` increments

**This means:**
1. Changes made during iteration N won't apply until iteration N+1
2. For a 4-step sequence at 120 BPM (1/16 notes):
   - Iteration changes every 4 steps = 1 beat
   - Parameter changes may take up to 1 beat to take effect

3. The accumulator value **persists and accumulates**:
   - If accumulator is at 10 and you change value from 1 to 5
   - It doesn't reset to 5
   - It continues from 10 and adds 5 on next iteration → 15

### Reset Behavior

The accumulator resets to 0 when:
- Track is reset (e.g., due to reset measure)
- Sequencer stops and restarts (calls `restart()`)
- Pattern changes

## Code Flow Reference

### UI → Model Flow

```
User turns encoder
  ↓
NoteTrackListModel::edit() called
  ↓
NoteTrackListModel::editValue(AccumValue, +1, false)
  ↓
_track->editAccumValue(+1, false)
  ↓
setAccumValue(accumValue() + 1)
  ↓
_accumValue = clamp(new_value, 0, 7)
  ↓
DBG message printed
```

### Engine Read Flow

```
NoteTrackEngine::tick() called each PPQN tick
  ↓
triggerStep() called when divisor aligned
  ↓
Check if (_sequenceState.iteration() != _lastAccumIteration)
  ↓
Read fresh values: _noteTrack.accumDir(), _noteTrack.accumValue()
  ↓
Switch on accumDir to update _accumCurrent
  ↓
Add _accumCurrent to transpose
```

## Expected Debug Output Example

### Successful Parameter Change

```
[User starts playback with defaults]
Track 0: Iteration 1, AccumDir=0, AccumValue=1, AccumCurrent (before)=0
Track 0: AccumCurrent (after)=1

[User changes Accum Value to 5]
NoteTrack::setAccumValue(5) called

[Next iteration]
Track 0: Iteration 2, AccumDir=0, AccumValue=5, AccumCurrent (before)=1
Track 0: AccumCurrent (after)=6

[Next iteration]
Track 0: Iteration 3, AccumDir=0, AccumValue=5, AccumCurrent (before)=6
Track 0: AccumCurrent (after)=11

[User changes Dir to Down]
NoteTrack::setAccumDir(1) called

[Next iteration]
Track 0: Iteration 4, AccumDir=1, AccumValue=5, AccumCurrent (before)=11
Track 0: AccumCurrent (after)=6

[Next iteration]
Track 0: Iteration 5, AccumDir=1, AccumValue=5, AccumCurrent (before)=6
Track 0: AccumCurrent (after)=1
```

## Next Steps After Debugging

1. **Collect the debug output** from your test session
2. **Identify which scenario** matches your observations
3. **Report findings** including:
   - Which debug messages appear/don't appear
   - Exact sequence of events and output
   - Any patterns or anomalies

4. **Based on findings**, we can:
   - Fix the root cause (UI, model, or engine issue)
   - Add additional targeted debugging
   - Verify our assumptions about the implementation

## Removing Debug Output

Once the issue is resolved, the debug statements can be removed by:

1. Removing DBG() calls from `NoteTrack.h` (lines 287, 303)
2. Removing DBG() calls from `NoteTrackEngine.cpp` (lines 309-310, 327)
3. Removing `#include "core/Debug.h"` from `NoteTrack.h` (line 8)

Or, if CONFIG_ENABLE_DEBUG is 0, they will compile to nothing anyway.

## Files Modified

- `src/apps/sequencer/model/NoteTrack.h`
- `src/apps/sequencer/engine/NoteTrackEngine.cpp`

## Commit Reference

- Commit: "Add debug logging to diagnose accumulator parameter issue"
- Branch: `claude/test-accumulator-mvp-01VANBTMxfReFi66x8h3PsKD`
