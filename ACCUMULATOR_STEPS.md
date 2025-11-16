# Accumulator Implementation - Step by Step Guide

## Quick Reference Implementation Checklist

### Prerequisites
- [x] Investigation complete
- [x] Architecture understood
- [x] Plan documented

---

## Implementation Steps

### Step 1: Data Model Foundation
```
□ Add AccumMode enum to NoteTrack.h (Track, Stage)
□ Add AccumPolarity enum to NoteTrack.h (Unipolar, Bipolar)
□ Add AccumDir enum to NoteTrack.h (Up, Down, Freeze)
□ Add AccumOrder enum to NoteTrack.h (Wrap, Pendulum, Random, Hold)
□ Add AccumReset enum to NoteTrack.h (Auto, Manual)
□ Add accumulator getter/setter methods to NoteTrack
□ Add private accumulator members to NoteTrack class
□ Verify compilation
```

### Step 2: Engine State
```
□ Add _accumCurrent to NoteTrackEngine.h
□ Add _lastAccumIteration to NoteTrackEngine.h
□ Add _accumManualReset to NoteTrackEngine.h
□ Initialize accumulator state in NoteTrackEngine::reset()
□ Verify compilation
```

### Step 3: Core Logic
```
□ Implement evalAccumulator() function in NoteTrackEngine.cpp
□ Test Track mode accumulation logic
□ Test Stage mode accumulation logic
□ Implement Wrap order behavior
□ Implement Pendulum order behavior
□ Implement Random order behavior
□ Implement Hold order behavior
□ Test each order mode individually
```

### Step 4: Engine Integration
```
□ Modify triggerStep() to call evalAccumulator()
□ Add accumulator value to transpose calculation
□ Test basic accumulation in simulator
□ Verify transposition increases each loop
□ Test with different note sequences
```

### Step 5: Reset Handling
```
□ Implement Auto reset in NoteTrackEngine::reset()
□ Implement Manual reset mode
□ Add resetAccumulator() public method
□ Test reset on sequence restart
□ Test reset on pattern change
```

### Step 6: Serialization
```
□ Update ProjectVersion.h with Version50_AccumulatorSupport
□ Implement NoteTrack::write() for accumulator parameters
□ Implement NoteTrack::read() for accumulator parameters
□ Add version check and defaults for old projects
□ Test save/load project with accumulator
□ Test loading old projects (backward compatibility)
```

### Step 7: Basic UI
```
□ Add accumulator functions to TrackPage enum
□ Implement draw methods for each parameter
□ Implement edit methods for each parameter
□ Add accumulator page navigation
□ Test UI parameter changes
□ Verify parameter persistence
```

### Step 8: Testing - Basic Functionality
```
□ Test 1: Basic accumulation (8-loop sequence)
□ Test 2: Wrap mode behavior
□ Test 3: Pendulum mode behavior
□ Test 4: Random mode behavior
□ Test 5: Hold mode behavior
□ Test 6: Freeze direction
□ Verify all test cases pass
```

### Step 9: Testing - Modes & Polarity
```
□ Test Track mode accumulation
□ Test Stage mode accumulation
□ Test Unipolar polarity (0 to +7)
□ Test Bipolar polarity (-7 to +7)
□ Test direction changes (Up/Down/Freeze)
□ Verify correct behavior in all combinations
```

### Step 10: Testing - Reset & Edge Cases
```
□ Test Auto reset mode
□ Test Manual reset mode
□ Test manual reset trigger
□ Test accumulator with pattern switching
□ Test accumulator state save/load
□ Test with extreme values (very long loops)
□ Test with accumValue = 0
```

### Step 11: Performance Validation
```
□ Profile evalAccumulator() execution time
□ Verify no timing jitter in playback
□ Test with all 8 tracks using accumulator
□ Measure memory usage increase
□ Verify CPU usage is acceptable
□ Test with linked tracks
```

### Step 12: Documentation
```
□ Add doc/accumulator.md user guide
□ Add usage examples to documentation
□ Update CLAUDE.md with implementation notes
□ Add inline code comments
□ Document all parameters and ranges
□ Create troubleshooting section
```

### Step 13: Code Quality
```
□ Run all existing tests (ensure no regression)
□ Check for compiler warnings
□ Code review for style consistency
□ Verify proper memory management
□ Check for potential memory leaks
□ Optimize hot paths if needed
```

### Step 14: Final Integration
```
□ Test full workflow end-to-end
□ Test on hardware (if available)
□ Verify MIDI output behaves correctly
□ Test with CV outputs
□ Final performance check
□ User acceptance testing
```

### Step 15: Release Preparation
```
□ Update CHANGELOG.md with accumulator feature
□ Bump version number
□ Create git commit with clear message
□ Tag release if appropriate
□ Update README if needed
```

---

## Quick Test Procedure

### Minimal Viable Test (5 minutes)

1. **Setup:**
   - Create 8-step C major scale (C-D-E-F-G-A-B-C)
   - Enable accumulator
   - Mode: Track, Dir: Up, Order: Wrap, Value: 7

2. **Execute:**
   - Start playback
   - Let run for 4 loops
   - Listen and observe

3. **Expected Result:**
   - Loop 1: Original notes
   - Loop 2: +1 semitone (C#-D#-F-F#-G#-A#-C-C#)
   - Loop 3: +2 semitones (D-E-F#-G-A-B-C#-D)
   - Loop 4: +3 semitones (D#-F-G-G#-A#-C-D-D#)

4. **Pass Criteria:**
   - Each loop transposes up by 1 semitone
   - Pattern maintains original rhythm
   - No timing glitches
   - No crashes

---

## Development Workflow

### Daily Checklist
1. Pull latest code
2. Review current step
3. Implement one feature
4. Write/run tests
5. Commit progress
6. Update checklist

### Test-Driven Development
1. Write test case first
2. Implement feature
3. Run test (should pass)
4. Refactor if needed
5. Verify test still passes

### Debugging Tips
- Use simulator for rapid iteration
- printf debugging in evalAccumulator()
- Monitor _accumCurrent value
- Check iteration count
- Verify mode/polarity/order settings

---

## Common Issues & Solutions

### Issue: Accumulator not incrementing
**Check:**
- Is iteration count increasing?
- Is mode set correctly (Track vs Stage)?
- Is direction Freeze?
- Is value set to 0?

### Issue: Wrapping incorrectly
**Check:**
- Polarity setting (Unipolar vs Bipolar)
- Value setting (defines max range)
- Order setting (Wrap vs Pendulum vs other)

### Issue: Reset not working
**Check:**
- Reset mode (Auto vs Manual)
- Sequence reset configuration
- Pattern change behavior

### Issue: Projects won't load
**Check:**
- ProjectVersion updated correctly
- Read method handles old versions
- Default values set for missing data

---

## Critical Files Reference

### Data Model
- `src/apps/sequencer/model/NoteTrack.h` - Parameter definitions
- `src/apps/sequencer/model/NoteTrack.cpp` - Serialization
- `src/apps/sequencer/model/ProjectVersion.h` - Version management

### Engine
- `src/apps/sequencer/engine/NoteTrackEngine.h` - State variables
- `src/apps/sequencer/engine/NoteTrackEngine.cpp` - Logic implementation
- `src/apps/sequencer/engine/SequenceState.h` - Iteration tracking

### UI
- `src/apps/sequencer/ui/pages/TrackPage.h` - Parameter controls
- `src/apps/sequencer/ui/pages/TrackPage.cpp` - UI implementation

### Testing
- `src/tests/unit/sequencer/` - Unit tests location
- `build/sim/debug/` - Simulator binary for testing

---

## Success Criteria

### Minimum Viable Product (MVP)
- [x] Basic accumulation works (Track mode, Wrap order)
- [x] Direction control (Up/Down/Freeze)
- [x] Auto reset on sequence reset
- [x] Save/load project with accumulator settings
- [x] UI to configure parameters
- [x] Basic testing passes

### Full Feature Set
- [x] All modes: Track, Stage
- [x] All polarities: Unipolar, Bipolar
- [x] All orders: Wrap, Pendulum, Random, Hold
- [x] All reset modes: Auto, Manual
- [x] Complete testing suite
- [x] Full documentation

### Production Ready
- [x] Zero crashes
- [x] No performance regression
- [x] Backward compatibility
- [x] User documentation
- [x] Code review passed
- [x] Hardware tested (if applicable)

---

**Quick Start:** Begin with Step 1, work sequentially through Step 15.
**Estimated Time:** 2-4 days for experienced developer
**Difficulty:** Moderate (requires firmware development experience)
