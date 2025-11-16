# Metropolix Accumulator Implementation Plan

## Overview

Implement Intellijel Metropolix-style accumulator functionality for PEW|FORMER. The accumulator incrementally adds/subtracts a value to transposition each time a sequence loops, creating evolving melodic phrases from simple patterns.

## Core Requirements

### Accumulator Parameters

1. **Accum Mode** (Stage vs Track)
   - Stage: Accumulate when specific stage is played
   - Track: Accumulate once per track cycle

2. **Accum Polarity** (Unipolar vs Bipolar)
   - Unipolar: 0 to +7 (one direction only)
   - Bipolar: -7 to +7 (both directions)

3. **Accum Dir** (Direction Control)
   - Up: Add value
   - Down: Subtract value
   - Freeze: Hold current value (performance feature)

4. **Accum Order** (Wrap Behavior)
   - Wrap: Wraps around (e.g., +7 → -7)
   - Pendulum: Reverses direction at limits
   - Random: Jumps to random value at limit
   - Hold: Stays at limit value

5. **Accum Reset** (Reset Behavior)
   - Auto: Reset when sequence resets
   - Manual: Never auto-reset, manual trigger only

6. **Accum Value** (Amount per accumulation)
   - Range: typically 0-7 semitones
   - Applied each loop iteration

---

## Architecture Analysis

### Current System

**Data Model:**
- `NoteTrack` - Track-level parameters (octave, transpose, rotate)
- `NoteSequence` - Sequence-level parameters (scale, divisor, firstStep, lastStep)
- `SequenceState` - Runtime state (step, iteration, direction)

**Engine:**
- `NoteTrackEngine::triggerStep()` - Triggers notes with transposition
- `evalTransposition()` - Combines octave and transpose
- `evalStepNote()` - Calculates final note voltage
- Iteration tracking already exists in `SequenceState::_iteration`

**Key Files:**
- `/src/apps/sequencer/model/NoteTrack.h` - Track parameters
- `/src/apps/sequencer/model/NoteSequence.h` - Sequence structure
- `/src/apps/sequencer/engine/NoteTrackEngine.cpp` - Note playback engine
- `/src/apps/sequencer/engine/SequenceState.h` - Sequence iteration state

---

## Implementation Steps

### Phase 1: Investigation & Planning ✓

```
✓ Understand codebase architecture
✓ Identify where transposition is applied
✓ Locate iteration tracking mechanism
✓ Review routing system
✓ Examine UI page structure
✓ Check serialization patterns
```

### Phase 2: Data Model Extensions

#### 2.1 Add Accumulator State Structure

**File:** `src/apps/sequencer/model/NoteTrack.h`

**Add new enums:**
```cpp
enum class AccumMode : uint8_t {
    Track,      // Accumulate once per track loop
    Stage,      // Accumulate when specific stage plays
    Last
};

enum class AccumPolarity : uint8_t {
    Unipolar,   // 0 to max only
    Bipolar,    // -max to +max
    Last
};

enum class AccumDir : uint8_t {
    Up,         // Add value
    Down,       // Subtract value
    Freeze,     // Hold current value
    Last
};

enum class AccumOrder : uint8_t {
    Wrap,       // Wrap around at limits
    Pendulum,   // Reverse direction at limits
    Random,     // Random value at limit
    Hold,       // Hold at limit
    Last
};

enum class AccumReset : uint8_t {
    Auto,       // Reset with sequence reset
    Manual,     // Manual reset only
    Last
};
```

**Add accumulator parameters to NoteTrack:**
```cpp
// Accumulator parameters
AccumMode accumMode() const;
void setAccumMode(AccumMode mode);

AccumPolarity accumPolarity() const;
void setAccumPolarity(AccumPolarity polarity);

AccumDir accumDir() const;
void setAccumDir(AccumDir dir);

AccumOrder accumOrder() const;
void setAccumOrder(AccumOrder order);

AccumReset accumReset() const;
void setAccumReset(AccumReset reset);

int accumValue() const;  // 0-7 semitones
void setAccumValue(int value);

int accumStage() const;  // Which stage triggers accumulation (for Stage mode)
void setAccumStage(int stage);
```

**Add private members:**
```cpp
private:
    AccumMode _accumMode;
    AccumPolarity _accumPolarity;
    AccumDir _accumDir;
    AccumOrder _accumOrder;
    AccumReset _accumReset;
    int8_t _accumValue;      // 0-7
    int8_t _accumStage;      // 0-15 (for Stage mode)
```

#### 2.2 Add Accumulator Runtime State

**File:** `src/apps/sequencer/engine/NoteTrackEngine.h`

**Add to NoteTrackEngine class:**
```cpp
private:
    // Accumulator state
    int32_t _accumCurrent;        // Current accumulated value
    uint32_t _lastAccumIteration; // Last iteration when accumulated
    bool _accumManualReset;       // Manual reset flag
```

#### 2.3 Serialization Updates

**File:** `src/apps/sequencer/model/NoteTrack.cpp`

**Update serialization methods:**
```cpp
void NoteTrack::write(VersionedSerializedWriter &writer) const {
    // ... existing code ...

    // Write accumulator parameters (new version)
    writer.write(uint8_t(_accumMode));
    writer.write(uint8_t(_accumPolarity));
    writer.write(uint8_t(_accumDir));
    writer.write(uint8_t(_accumOrder));
    writer.write(uint8_t(_accumReset));
    writer.write(_accumValue);
    writer.write(_accumStage);
}

void NoteTrack::read(VersionedSerializedReader &reader) {
    // ... existing code ...

    // Read accumulator parameters (version check)
    if (reader.dataVersion >= ProjectVersion::Version50_AccumulatorSupport) {
        _accumMode = AccumMode(reader.read<uint8_t>());
        _accumPolarity = AccumPolarity(reader.read<uint8_t>());
        _accumDir = AccumDir(reader.read<uint8_t>());
        _accumOrder = AccumOrder(reader.read<uint8_t>());
        _accumReset = AccumReset(reader.read<uint8_t>());
        reader.read(_accumValue);
        reader.read(_accumStage);
    } else {
        // Default values for older projects
        _accumMode = AccumMode::Track;
        _accumPolarity = AccumPolarity::Bipolar;
        _accumDir = AccumDir::Up;
        _accumOrder = AccumOrder::Wrap;
        _accumReset = AccumReset::Auto;
        _accumValue = 0;
        _accumStage = 0;
    }
}
```

**File:** `src/apps/sequencer/model/ProjectVersion.h`

**Add new version:**
```cpp
enum Version : uint16_t {
    // ... existing versions ...
    Version50_AccumulatorSupport = 50,
    Latest = Version50_AccumulatorSupport
};
```

### Phase 3: Engine Implementation

#### 3.1 Accumulator Logic Core

**File:** `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Add accumulator calculation function:**
```cpp
// Calculate accumulated transposition value
static int evalAccumulator(
    NoteTrack::AccumMode mode,
    NoteTrack::AccumPolarity polarity,
    NoteTrack::AccumDir dir,
    NoteTrack::AccumOrder order,
    int accumValue,
    int &accumCurrent,
    uint32_t iteration,
    uint32_t &lastIteration,
    int currentStep,
    int accumStage,
    Random &rng
) {
    // Determine if we should accumulate
    bool shouldAccumulate = false;

    if (mode == NoteTrack::AccumMode::Track) {
        // Accumulate once per track loop
        shouldAccumulate = (iteration != lastIteration);
    } else {
        // Stage mode: accumulate when specific step plays
        shouldAccumulate = (currentStep == accumStage) && (iteration != lastIteration);
    }

    if (!shouldAccumulate) {
        return accumCurrent;
    }

    // Update last iteration
    lastIteration = iteration;

    // Handle freeze
    if (dir == NoteTrack::AccumDir::Freeze) {
        return accumCurrent;
    }

    // Determine limits based on polarity
    int minLimit = (polarity == NoteTrack::AccumPolarity::Unipolar) ? 0 : -accumValue;
    int maxLimit = accumValue;

    // Apply accumulation based on direction
    int newValue = accumCurrent;
    if (dir == NoteTrack::AccumDir::Up) {
        newValue += 1;  // Increment by 1 semitone per loop
    } else if (dir == NoteTrack::AccumDir::Down) {
        newValue -= 1;
    }

    // Handle limits based on order
    if (newValue > maxLimit) {
        switch (order) {
            case NoteTrack::AccumOrder::Wrap:
                newValue = minLimit;
                break;
            case NoteTrack::AccumOrder::Pendulum:
                // Reverse direction (would need to store direction state)
                newValue = maxLimit;
                // TODO: Implement direction reversal
                break;
            case NoteTrack::AccumOrder::Random:
                newValue = rng.nextRange(maxLimit - minLimit + 1) + minLimit;
                break;
            case NoteTrack::AccumOrder::Hold:
                newValue = maxLimit;
                break;
        }
    } else if (newValue < minLimit) {
        switch (order) {
            case NoteTrack::AccumOrder::Wrap:
                newValue = maxLimit;
                break;
            case NoteTrack::AccumOrder::Pendulum:
                newValue = minLimit;
                // TODO: Implement direction reversal
                break;
            case NoteTrack::AccumOrder::Random:
                newValue = rng.nextRange(maxLimit - minLimit + 1) + minLimit;
                break;
            case NoteTrack::AccumOrder::Hold:
                newValue = minLimit;
                break;
        }
    }

    accumCurrent = newValue;
    return accumCurrent;
}
```

#### 3.2 Integrate into triggerStep

**File:** `src/apps/sequencer/engine/NoteTrackEngine.cpp`

**Modify triggerStep() function:**
```cpp
void NoteTrackEngine::triggerStep(uint32_t tick, uint32_t divisor) {
    int octave = _noteTrack.octave();
    int transpose = _noteTrack.transpose();

    // Calculate accumulator value
    int accumTranspose = evalAccumulator(
        _noteTrack.accumMode(),
        _noteTrack.accumPolarity(),
        _noteTrack.accumDir(),
        _noteTrack.accumOrder(),
        _noteTrack.accumValue(),
        _accumCurrent,
        _sequenceState.iteration(),
        _lastAccumIteration,
        _sequenceState.step(),
        _noteTrack.accumStage(),
        rng
    );

    // Add accumulator to transpose
    transpose += accumTranspose;

    // ... rest of existing triggerStep code ...
}
```

#### 3.3 Reset Handling

**Modify reset() function:**
```cpp
void NoteTrackEngine::reset() {
    // ... existing reset code ...

    // Reset accumulator if in Auto mode
    if (_noteTrack.accumReset() == NoteTrack::AccumReset::Auto) {
        _accumCurrent = 0;
        _lastAccumIteration = 0;
    }

    // Handle manual reset
    if (_accumManualReset) {
        _accumCurrent = 0;
        _lastAccumIteration = 0;
        _accumManualReset = false;
    }
}
```

**Add manual reset method:**
```cpp
void NoteTrackEngine::resetAccumulator() {
    _accumCurrent = 0;
    _lastAccumIteration = 0;
}
```

### Phase 4: UI Implementation

#### 4.1 Track Page Extensions

**File:** `src/apps/sequencer/ui/pages/TrackPage.cpp`

**Add accumulator parameters to track page:**
```cpp
enum class Function {
    // ... existing functions ...
    AccumMode,
    AccumPolarity,
    AccumDir,
    AccumOrder,
    AccumReset,
    AccumValue,
    AccumStage,
    AccumManualReset,  // Button to manually reset
};
```

**Add UI rendering and editing:**
```cpp
void TrackPage::drawAccumMode(Canvas &canvas, const NoteTrack &track) {
    FixedStringBuilder<16> str;
    switch (track.accumMode()) {
        case NoteTrack::AccumMode::Track:
            str("Track");
            break;
        case NoteTrack::AccumMode::Stage:
            str("Stage %d", track.accumStage() + 1);
            break;
    }
    canvas.drawText(/* position */, str);
}

void TrackPage::editAccumMode(NoteTrack &track, int value, bool shift) {
    auto mode = track.accumMode();
    if (value > 0) {
        mode = (mode == NoteTrack::AccumMode::Track) ?
               NoteTrack::AccumMode::Stage : NoteTrack::AccumMode::Track;
    } else {
        mode = (mode == NoteTrack::AccumMode::Stage) ?
               NoteTrack::AccumMode::Track : NoteTrack::AccumMode::Stage;
    }
    track.setAccumMode(mode);
}
```

#### 4.2 Add Accumulator Settings Page

**Create new file:** `src/apps/sequencer/ui/pages/AccumulatorPage.h`

```cpp
#pragma once

#include "ListPage.h"

class AccumulatorPage : public ListPage {
public:
    AccumulatorPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;
    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;
    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum class Function {
        Mode,
        Polarity,
        Direction,
        Order,
        Reset,
        Value,
        Stage,
        ManualReset,
        CurrentValue,  // Display only
        Last
    };

    // ... implementation ...
};
```

### Phase 5: Routing Integration (Optional)

**File:** `src/apps/sequencer/model/Routing.h`

**Add routing targets (optional - for CV control of accumulator):**
```cpp
enum class Target : uint8_t {
    // ... existing targets ...

    // Accumulator targets
    AccumValue,     // CV control of accumulation amount
    AccumDir,       // CV control of direction
    AccumReset,     // CV trigger for manual reset

    // ... rest of targets ...
};
```

### Phase 6: Testing

#### 6.1 Unit Tests

**Create:** `src/tests/unit/sequencer/AccumulatorTest.cpp`

```cpp
// Test accumulator wrap behavior
TEST(Accumulator, WrapMode) {
    // Setup
    int accumCurrent = 0;
    int accumValue = 7;

    // Accumulate up to limit and wrap
    for (int i = 0; i < 10; i++) {
        accumCurrent = evalAccumulator(
            /* Track mode, Bipolar, Up, Wrap */
        );
    }

    // Verify wrapping occurred
    EXPECT_EQ(accumCurrent, expected_wrapped_value);
}

// Test pendulum mode
// Test random mode
// Test hold mode
// Test freeze direction
// Test manual vs auto reset
```

#### 6.2 Integration Tests

**Test Scenarios:**

1. **Basic Accumulation:**
   - Create 8-step C major scale
   - Set accumulator to +1 semitone per loop
   - Verify transpose increases each loop
   - Loop 12 times, verify full octave transposition

2. **Wrap Behavior:**
   - Set accumValue = 7 (Bipolar: -7 to +7)
   - Accumulate up past +7
   - Verify wraps to -7
   - Continue and verify pattern

3. **Pendulum Behavior:**
   - Set Order = Pendulum
   - Accumulate to +7
   - Verify direction reverses
   - Accumulate back to -7
   - Verify direction reverses again

4. **Freeze Mode:**
   - Accumulate to +3
   - Switch Dir to Freeze
   - Loop several times
   - Verify stays at +3

5. **Stage Mode:**
   - Set Mode = Stage, Stage = 4
   - Verify accumulation only when step 4 plays
   - Verify no accumulation on other steps

6. **Reset Modes:**
   - Auto: Verify resets on sequence reset
   - Manual: Verify persists across resets until manual trigger

7. **Polarity:**
   - Unipolar: Verify range 0 to +7
   - Bipolar: Verify range -7 to +7

#### 6.3 Performance Tests

- Measure CPU impact of accumulator calculation
- Verify no timing jitter introduced
- Test with maximum accumulator complexity (Stage mode + Random order)
- Profile memory usage increase

#### 6.4 Edge Cases

1. **Project Loading:**
   - Old projects without accumulator
   - Projects with accumulator at limits
   - Mid-accumulation state save/load

2. **Pattern Switching:**
   - Accumulator state during pattern change
   - Reset behavior on pattern change
   - Accumulator with fill mode

3. **Linked Tracks:**
   - Accumulator behavior with track linking
   - Independent accumulator per track

4. **Extreme Values:**
   - accumValue = 0 (no accumulation)
   - Very long loops (1000+ iterations)
   - Rapid direction changes

### Phase 7: Documentation

#### 7.1 User Documentation

**Create:** `doc/accumulator.md`

```markdown
# Accumulator Feature

The Accumulator incrementally transposes sequences over time, creating
evolving melodic phrases from simple patterns.

## Parameters

### Accum Mode
- **Track**: Accumulates once per complete sequence loop
- **Stage**: Accumulates only when specified step plays

### Accum Polarity
- **Unipolar**: 0 to +7 semitones (upward only)
- **Bipolar**: -7 to +7 semitones (both directions)

### Accum Dir
- **Up**: Add 1 semitone per accumulation
- **Down**: Subtract 1 semitone per accumulation
- **Freeze**: Hold at current value (performance control)

### Accum Order
- **Wrap**: Wraps around at limits (e.g., +7 → -7)
- **Pendulum**: Reverses direction at limits
- **Random**: Jumps to random value at limit
- **Hold**: Stays at limit value

### Accum Reset
- **Auto**: Resets when sequence resets
- **Manual**: Only resets via manual trigger

### Accum Value
- Range: 0-7 semitones
- Maximum accumulation range

## Usage Examples

[Include musical examples and use cases]
```

#### 7.2 Developer Documentation

**Update:** `CLAUDE.md`

```markdown
## Accumulator Implementation

The accumulator feature adds evolving transposition to sequences:

- **Data Model**: `NoteTrack` accumulator parameters
- **Engine**: `NoteTrackEngine::evalAccumulator()` calculation
- **State**: `_accumCurrent` tracks current accumulated value
- **Reset**: Handled in `NoteTrackEngine::reset()`
- **UI**: AccumulatorPage for parameter control

### Key Files
- `src/apps/sequencer/model/NoteTrack.h` - Parameters
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` - Logic
- `src/apps/sequencer/ui/pages/AccumulatorPage.h` - UI

### Memory Impact
- ~7 bytes per track (parameters)
- 12 bytes per engine instance (runtime state)
```

### Phase 8: Cleanup & Optimization

#### 8.1 Code Review Checklist

- [ ] All accumulator parameters properly serialized
- [ ] Version migration tested
- [ ] Memory usage within limits
- [ ] No performance regression
- [ ] UI follows existing patterns
- [ ] Routing integration (if implemented)
- [ ] Documentation complete
- [ ] Tests passing

#### 8.2 Performance Optimization

- Profile `evalAccumulator()` execution time
- Optimize direction reversal in Pendulum mode
- Consider lookup tables for wrap calculations
- Minimize branching in hot path

---

## Implementation Checklist

### Phase 1: Investigation ✓
- [x] Understand codebase architecture
- [x] Identify transposition application points
- [x] Review iteration tracking
- [x] Examine routing system
- [x] Study UI patterns

### Phase 2: Data Model
- [ ] Add accumulator enums to NoteTrack.h
- [ ] Add accumulator parameters to NoteTrack class
- [ ] Add accumulator state to NoteTrackEngine
- [ ] Update NoteTrack serialization (write)
- [ ] Update NoteTrack serialization (read)
- [ ] Update ProjectVersion.h with new version
- [ ] Test serialization round-trip

### Phase 3: Engine Logic
- [ ] Implement evalAccumulator() function
- [ ] Add accumulator state variables to NoteTrackEngine
- [ ] Integrate into triggerStep()
- [ ] Implement Track mode accumulation
- [ ] Implement Stage mode accumulation
- [ ] Implement Wrap order behavior
- [ ] Implement Pendulum order behavior
- [ ] Implement Random order behavior
- [ ] Implement Hold order behavior
- [ ] Implement Freeze direction
- [ ] Implement Auto reset
- [ ] Implement Manual reset
- [ ] Add resetAccumulator() method

### Phase 4: UI
- [ ] Add accumulator functions to TrackPage
- [ ] Create AccumulatorPage.h
- [ ] Create AccumulatorPage.cpp
- [ ] Implement draw() methods
- [ ] Implement edit() methods
- [ ] Add accumulator page to page navigation
- [ ] Implement manual reset button
- [ ] Add current value display

### Phase 5: Routing (Optional)
- [ ] Add routing targets for accumulator
- [ ] Implement AccumValue routing
- [ ] Implement AccumDir routing
- [ ] Implement AccumReset trigger routing
- [ ] Test routing integration

### Phase 6: Testing
- [ ] Unit tests: Wrap mode
- [ ] Unit tests: Pendulum mode
- [ ] Unit tests: Random mode
- [ ] Unit tests: Hold mode
- [ ] Unit tests: Freeze direction
- [ ] Integration test: Basic accumulation
- [ ] Integration test: All order modes
- [ ] Integration test: Stage mode
- [ ] Integration test: Reset modes
- [ ] Integration test: Polarity modes
- [ ] Performance test: CPU usage
- [ ] Performance test: Memory usage
- [ ] Edge case test: Project loading
- [ ] Edge case test: Pattern switching
- [ ] Edge case test: Linked tracks
- [ ] Edge case test: Extreme values

### Phase 7: Documentation
- [ ] Create doc/accumulator.md user guide
- [ ] Update CLAUDE.md with implementation notes
- [ ] Add code comments
- [ ] Create usage examples
- [ ] Document parameter ranges

### Phase 8: Final
- [ ] Code review
- [ ] Performance optimization
- [ ] Final testing pass
- [ ] Update CHANGELOG.md
- [ ] Commit and push

---

## Testing Guide

### Test 1: Basic Accumulation
```
Setup:
1. Create new project
2. Track 1: Note track
3. Sequence: C-D-E-F-G-A-B-C (8 steps)
4. Accumulator:
   - Mode: Track
   - Polarity: Bipolar
   - Dir: Up
   - Order: Wrap
   - Reset: Auto
   - Value: 7

Procedure:
1. Start playback
2. Let loop 4 times
3. Observe transposition

Expected:
- Loop 1: C-D-E-F-G-A-B-C (no accumulation yet)
- Loop 2: C#-D#-F-F#-G#-A#-C-C# (+1 semitone)
- Loop 3: D-E-F#-G-A-B-C#-D (+2 semitones)
- Loop 4: D#-F-G-G#-A#-C-D-D# (+3 semitones)
```

### Test 2: Wrap Behavior
```
Setup:
- Same as Test 1
- Let accumulate to +7 (8 loops)

Expected:
- Loop 8: +7 semitones
- Loop 9: -7 semitones (wrapped)
- Loop 10: -6 semitones
```

### Test 3: Pendulum Mode
```
Setup:
- Accumulator Order: Pendulum

Expected:
- Accumulates up to +7
- Then accumulates down to -7
- Then up again
- Creates smooth pendulum motion
```

### Test 4: Freeze Performance
```
Procedure:
1. Start playback
2. Let accumulate to +3
3. While playing, set Dir: Freeze
4. Continue playback

Expected:
- Transposition stays at +3
- No further accumulation
- Pattern plays with constant +3 offset
```

### Test 5: Stage Mode
```
Setup:
- Accumulator Mode: Stage
- Accumulator Stage: 4 (5th step, 0-indexed)

Expected:
- Accumulation only increments when step 4 plays
- Takes 8x longer to complete full accumulation cycle
- Other steps don't trigger accumulation
```

### Test 6: Manual Reset
```
Setup:
- Accumulator Reset: Manual

Procedure:
1. Let accumulate to +5
2. Reset sequence (via reset button)
3. Observe accumulator value
4. Trigger manual accumulator reset
5. Observe accumulator value

Expected:
- Step 2: Accumulator stays at +5 (Manual mode)
- Step 4: Accumulator resets to 0
```

### Test 7: Project Save/Load
```
Procedure:
1. Setup accumulator with specific settings
2. Let accumulate to +3
3. Save project
4. Stop playback
5. Load different project
6. Load original project back
7. Start playback

Expected:
- Accumulator settings restored
- Accumulation continues from saved state (or resets based on reset mode)
```

### Test 8: Pattern Switching
```
Procedure:
1. Pattern 1: Setup with accumulator
2. Let accumulate to +4
3. Switch to Pattern 2 (different sequence)
4. Switch back to Pattern 1

Expected:
- Accumulator state behavior depends on reset mode
- If Auto: Should reset on pattern change
- If Manual: Should maintain state across patterns
```

---

## Memory Budget

**NoteTrack additions:**
- `_accumMode`: 1 byte
- `_accumPolarity`: 1 byte
- `_accumDir`: 1 byte
- `_accumOrder`: 1 byte
- `_accumReset`: 1 byte
- `_accumValue`: 1 byte
- `_accumStage`: 1 byte
**Subtotal: 7 bytes per track**

**NoteTrackEngine additions:**
- `_accumCurrent`: 4 bytes (int32_t)
- `_lastAccumIteration`: 4 bytes (uint32_t)
- `_accumManualReset`: 1 byte (bool) + 3 padding
**Subtotal: 12 bytes per track engine**

**Total per track: ~19 bytes**
**Total for 8 tracks: ~152 bytes**

**Memory impact:** Minimal (~0.08% of 192KB RAM)

---

## Performance Considerations

### Critical Path Analysis

**evalAccumulator() execution:**
- Called once per step trigger
- Conditional logic (mode, order checks)
- Integer arithmetic only
- No floating point
- No memory allocation

**Estimated cycles:**
- Best case: ~50 cycles (no accumulation needed)
- Worst case: ~200 cycles (random order with RNG call)
- At 168MHz: < 1.2μs worst case

**Impact:** Negligible performance impact

---

## Known Limitations

1. **Pendulum Direction State:**
   - Current design doesn't track pendulum direction independently
   - Would need additional state variable for perfect pendulum behavior
   - Workaround: Use direction inference from current value

2. **Accumulator Range:**
   - Limited to ±7 semitones
   - Could be extended if needed
   - Sufficient for most musical applications

3. **Stage Mode Precision:**
   - Accumulates based on step index
   - May not perfectly match Metropolix if step skipping is involved

4. **No Per-Sequence Accumulator:**
   - Accumulator is per-track, not per-sequence
   - All sequences on a track share accumulator state
   - Design decision for simplicity

---

## Future Enhancements

1. **Accumulator Amount Control:**
   - Currently fixed at 1 semitone per loop
   - Could add parameter for variable amount (e.g., 2, 3 semitones)

2. **Multiple Accumulator Destinations:**
   - Currently only affects transposition
   - Could accumulate other parameters (length, gate probability)

3. **Accumulator Probability:**
   - Random chance of accumulation occurring
   - Adds generative variation

4. **Accumulator Display:**
   - Visual feedback of current accumulated value
   - Graph showing accumulation over time

5. **Routing Integration:**
   - CV control of accumulator parameters
   - External reset trigger
   - Direction control via CV

---

## References

- **Metropolix Manual:** https://intellijel.com/support/metropolix-support/metropolix-manual/
- **Boards of Canada chord changes:** Classic use case for accumulator
- **Original Code Analysis:** See investigation phase above

---

**Document Version:** 1.0
**Created:** 2025-11-16
**Author:** Claude (AI Assistant)
**Status:** Planning Complete - Ready for Implementation
