# PEW|FORMER Testing Plan

**Document Version:** 1.0
**Date:** 2025-11-16
**Status:** Draft

## Executive Summary

This document provides a comprehensive analysis of the current test coverage in the PEW|FORMER codebase and outlines a strategic plan to improve testing across all critical components. The analysis reveals that while driver-level integration testing is strong (80% coverage), there are critical gaps in unit testing for core sequencer functionality, particularly in the engine, data model, and algorithmic components.

**Current Overall Coverage:** ~15-20%
**Target Coverage (6 months):** 55-60%
**Highest Priority:** Clock engine, data model serialization, MIDI parsing, pattern generators

---

## Current Test Coverage Summary

### Test Files by Category

| Category | Files | Coverage | Status |
|----------|-------|----------|--------|
| **Unit Tests** | 13 C++ files | ~30% of core utilities | ✅ Partial |
| **Integration Tests** | 11 C++ files | ~80% of drivers | ✅ Good |
| **Python UI Tests** | 6 Python files | ~10% of UI workflows | ⚠️ Minimal |
| **Total** | **30 test files** | **~15-20% overall** | ⚠️ Needs Improvement |

### What's Currently Tested

#### Unit Tests ✅ (Good Coverage)
- **Core I/O Serialization** (`src/tests/unit/core/io/`)
  - ✅ Versioned data migration (`TestVersionedSerialization.cpp`)
  - ✅ Basic serialization (`TestSerialization.cpp`)

- **Core Utilities** (`src/tests/unit/core/utils/`)
  - ✅ String utilities (`TestStringUtils.cpp`)
  - ✅ Object pool (`TestObjectPool.cpp`)
  - ✅ Random number generation (`TestRandom.cpp`)
  - ✅ Moving average (`TestMovingAverage.cpp`)

- **Sequencer Logic** (`src/tests/unit/sequencer/`)
  - ✅ Scale system (`TestScale.cpp`)
  - ✅ Curve calculations (`TestCurve.cpp`)

#### Integration Tests ✅ (Good Coverage)
- **Hardware Drivers** (`src/tests/integration/drivers/`)
  - ✅ ADC, DAC, LCD, MIDI, USB MIDI
  - ✅ SD Card, Encoder, Gate Output
  - ✅ Button/LED Matrix, DIO

- **File System** (`src/tests/integration/fs/`)
  - ✅ Basic file operations (`TestFileSystem.cpp`)

#### Python UI Tests ⚠️ (Minimal Coverage)
- **Project Page** (`src/apps/sequencer/tests/ui/project.py`)
  - ✅ Name editing workflow
  - ✅ Tempo adjustment
  - ✅ Swing adjustment

---

## Critical Coverage Gaps

### 1. Sequencer Engine (HIGH PRIORITY) ⚠️⚠️⚠️

**Files:** 49 engine files
**Current Coverage:** 0%
**Location:** `src/apps/sequencer/engine/`

#### Missing Tests

**Clock System** - `Clock.cpp:1`, `Clock.h:1`
- ❌ Master/slave mode switching
- ❌ BPM accuracy and drift calculations
- ❌ Swing timing calculations
- ❌ MIDI clock synchronization
- ❌ PPQN tick generation
- **Risk:** Clock drift, timing inaccuracies, synchronization failures in live performance

**Track Engines** (9 engine implementations)
- ❌ `NoteTrackEngine` - Core note sequencing logic
- ❌ `CurveTrackEngine` - CV curve generation
- ❌ `ArpeggiatorEngine` - Arpeggiator note generation
- ❌ `MidiCvTrackEngine` - MIDI-to-CV conversion
- ❌ `MidiOutputEngine` - MIDI output routing
- ❌ `RoutingEngine` - Signal routing matrix
- **Risk:** Silent bugs in core sequencing behavior, incorrect CV output

**Pattern Generators** - `src/apps/sequencer/engine/generators/`
- ❌ `EuclideanGenerator.cpp:1` - Euclidean rhythm algorithm
- ❌ `RandomGenerator.cpp:1` - Probabilistic pattern generation
- ❌ `Rhythm.cpp:1` - Rhythm pattern parsing and manipulation
- ❌ `SequenceBuilder.h:1` - Pattern builder interface
- **Risk:** Incorrect pattern generation, off-by-one errors, mathematical errors

**Other Engine Components**
- ❌ `StepRecorder.h:1` - Live recording with quantization
- ❌ `Groove.h:1` - Timing humanization
- ❌ `SortedQueue.h:1` - Event scheduling data structure
- ❌ `EngineState.h:1` - Engine state management
- **Risk:** Timing quantization bugs, dropped notes, state corruption

---

### 2. Data Model & Serialization (HIGH PRIORITY) ⚠️⚠️⚠️

**Files:** 59 model files
**Current Coverage:** ~5%
**Location:** `src/apps/sequencer/model/`

#### Missing Tests

**Core Sequences** - Critical for data integrity
- ❌ `NoteSequence.h:1` - 13-layer sequence data structure
  - Gate, GateProbability, GateOffset, Slide
  - Retrigger, RetriggerProbability
  - Length, LengthVariationRange, LengthVariationProbability
  - Note, NoteVariationRange, NoteVariationProbability
  - Condition
  - **Risk:** Bit-packing errors, layer data corruption

- ❌ `CurveSequence.h:1` - CV curve sequence data
  - Curve shapes and timing
  - **Risk:** CV output errors, curve interpolation bugs

**Project State**
- ❌ `Project.h:1` - Complete project state management
  - Multi-track data
  - Global settings (tempo, swing, clock)
  - **Risk:** Project file corruption, data loss

- ❌ `Track.h:1` - Individual track configuration
  - NoteTrack, CurveTrack, MidiCvTrack
  - **Risk:** Track state corruption

**Version Migration**
- ❌ Complete save/load cycle tests
- ❌ Version migration scenarios (v1 → v2 → v3)
- ❌ Backward compatibility verification
- ❌ Data integrity after migration
- **Risk:** User project files become unreadable after updates

**Other Model Components**
- ❌ `Song.h:1` - Song arrangement and patterns
- ❌ `ClipBoard.h:1` - Copy/paste functionality
- ❌ `Arpeggiator.h:1` - Arpeggiator settings
- ❌ `MidiConfig.h:1` - MIDI configuration
- ❌ `Routing.h:1` - CV/Gate routing matrix
- ❌ `UserScale.h:1` - User-defined scales

---

### 3. Core Libraries (MEDIUM PRIORITY) ⚠️⚠️

**Files:** 54 core library files
**Current Coverage:** ~30%
**Location:** `src/core/`

#### Missing Tests

**MIDI Library** - `src/core/midi/`
- ❌ `MidiParser.h:1` - MIDI byte stream parsing
  - Running status handling
  - SysEx message parsing
  - Malformed input handling
  - **Risk:** MIDI parsing failures, dropped messages

- ❌ `MidiMessage.h:1` - MIDI message construction
  - All message types (Note On/Off, CC, Program Change, etc.)
  - Channel handling
  - **Risk:** Malformed MIDI output

**Math Library** - `src/core/math/`
- ❌ `Vec2.h:1`, `Vec3.h:1`, `Vec4.h:1` - Vector operations
  - Addition, subtraction, scaling
  - Dot product, cross product
  - Normalization
  - **Risk:** Graphics glitches, calculation errors

- ❌ `Mat3.h:1`, `Mat4.h:1` - Matrix operations
  - Matrix multiplication
  - Transformations
  - **Risk:** UI rendering errors

**Graphics Library** - `src/core/gfx/`
- ❌ `Canvas.h:1` - Canvas drawing operations
- ❌ `FrameBuffer.h:1` - Frame buffer management
- ❌ `Blit.h:1` - Blitting operations
- **Risk:** Display artifacts, rendering bugs

**File System** - `src/core/fs/`
- ⚠️ `FileSystem.h:1` - Limited edge case coverage
- ❌ Error recovery scenarios
- ❌ Concurrent access patterns
- ❌ SD card failure handling
- **Risk:** Data loss, file corruption

---

### 4. UI Components (LOW-MEDIUM PRIORITY) ⚠️

**Files:** 129 UI files
**Current Coverage:** ~10%
**Location:** `src/apps/sequencer/ui/`

#### Missing Tests

**UI Pages** - `src/apps/sequencer/ui/pages/` (32 page implementations)
- ⚠️ Only Project page has automated tests
- ❌ Sequence editor page
- ❌ Track configuration page
- ❌ Song arrangement page
- ❌ Clock setup page
- ❌ Routing page
- ❌ MIDI configuration page
- ❌ System settings page
- **Risk:** UI state bugs, navigation issues, data entry errors

**Controllers** - `src/apps/sequencer/ui/controllers/launchpad/`
- ❌ Launchpad S, Mini, Mk2, Pro, Mini Mk3, X, Pro Mk3
- ❌ Button mapping verification
- ❌ LED feedback correctness
- **Risk:** Controller input errors, LED display bugs

**Painters** - `src/apps/sequencer/ui/painters/`
- ❌ Sequence painter
- ❌ Track painter
- ❌ Various display painters
- **Risk:** Display rendering bugs (lower priority, visual testing needed)

---

## Phased Implementation Plan

### Phase 1: Critical Path (Weeks 1-2) 🔴

**Goal:** Test the most critical, high-risk components that could cause data loss or timing issues.

#### 1.1 Clock Engine Tests
**Priority:** Critical
**Effort:** Medium (3-4 days)
**File to create:** `src/tests/unit/sequencer/TestClock.cpp`

**Test Coverage:**
- Master mode tick generation
- Slave mode synchronization
- BPM accuracy (float precision)
- Swing calculations (50-75%)
- Mode switching (Auto/Master/Slave)
- MIDI clock message generation
- Event handling (Start/Stop/Continue/Reset)
- Edge cases: BPM limits (1-1000), wrap-around

**Success Criteria:**
- Clock accuracy within 0.1% over 1000 ticks
- Swing timing matches expected offsets
- Mode transitions don't lose ticks

---

#### 1.2 MIDI Parser Tests
**Priority:** Critical
**Effort:** Small (2 days)
**Files to create:**
- `src/tests/unit/core/midi/TestMidiParser.cpp`
- `src/tests/unit/core/midi/TestMidiMessage.cpp`

**Test Coverage:**
- All message types:
  - Note On/Off
  - Control Change
  - Program Change
  - Pitch Bend
  - System Real-Time (Clock, Start, Stop)
  - System Exclusive
- Running status handling
- Malformed input (truncated messages, invalid status bytes)
- Buffer overflow protection
- Message construction and serialization

**Success Criteria:**
- 100% message type coverage
- No crashes on malformed input
- Correct byte-level encoding/decoding

---

#### 1.3 NoteSequence Serialization Tests
**Priority:** Critical
**Effort:** Medium (3 days)
**File to create:** `src/tests/unit/sequencer/model/TestNoteSequence.cpp`

**Test Coverage:**
- Save/load all 13 layers independently
- Bit-packing integrity for compact types
- Step data round-trip (write → read → verify)
- Default value initialization
- Layer value ranges and clamping
- Sequence length variations (1-64 steps)
- Clear/reset operations

**Success Criteria:**
- All layer data preserved across save/load
- No bit corruption in packed fields
- Handles min/max sequence lengths

---

### Phase 2: Engine & Generators (Weeks 3-4) 🟡

**Goal:** Ensure pattern generation and sequencing logic correctness.

#### 2.1 Pattern Generator Tests
**Priority:** High
**Effort:** Medium (4-5 days)
**Files to create:**
- `src/tests/unit/sequencer/generators/TestEuclidean.cpp`
- `src/tests/unit/sequencer/generators/TestRandom.cpp`
- `src/tests/unit/sequencer/generators/TestRhythm.cpp`

**Test Coverage:**

**Euclidean Generator:**
- Algorithm correctness (compare to known patterns)
- Edge cases: steps=1, beats=0, beats>steps
- Offset rotation
- Pattern consistency

**Random Generator:**
- Probability distribution (run 10,000 iterations, verify distribution)
- Seed reproducibility
- Density parameters
- Edge case probabilities (0%, 100%)

**Rhythm:**
- Rhythm string parsing ("x-x-x---")
- Pattern conversion
- Error handling (invalid strings)

**Success Criteria:**
- Euclidean patterns match published reference patterns
- Random distribution within 5% of expected probability
- All rhythm string formats parse correctly

---

#### 2.2 Track Engine Tests
**Priority:** High
**Effort:** Large (5-6 days)
**Files to create:**
- `src/tests/unit/sequencer/engine/TestNoteTrackEngine.cpp`
- `src/tests/unit/sequencer/engine/TestCurveTrackEngine.cpp`

**Test Coverage:**

**NoteTrackEngine:**
- Step advancement
- Gate on/off timing
- Note value output
- Retrigger behavior
- Slide functionality
- Condition evaluation
- Probability gate triggering
- Note variation (range + probability)

**CurveTrackEngine:**
- Curve interpolation
- Sequence playback
- Timing accuracy

**Success Criteria:**
- Gate timing matches expected clock divisions
- Probabilistic features statistically correct
- No off-by-one errors in step advancement

---

#### 2.3 Project Serialization Tests
**Priority:** High
**Effort:** Medium (4 days)
**File to create:** `src/tests/unit/sequencer/model/TestProjectSerialization.cpp`

**Test Coverage:**
- Complete project save/load cycle
- Multi-track projects (8 tracks with different types)
- Song arrangements
- Global settings (tempo, swing, clock setup)
- Version migration paths:
  - v1 → v2
  - v2 → v3
  - v1 → v3
- Backward compatibility
- Data integrity checksums

**Success Criteria:**
- 100% data preservation for current version
- All supported version migrations succeed
- No data loss across migrations

---

### Phase 3: Math & Utilities (Week 5) 🟢

**Goal:** Ensure foundational math and utility correctness.

#### 3.1 Math Library Tests
**Priority:** Medium
**Effort:** Small (2-3 days)
**Files to create:**
- `src/tests/unit/core/math/TestVec.cpp`
- `src/tests/unit/core/math/TestMat.cpp`

**Test Coverage:**

**Vector Tests:**
- Vec2, Vec3, Vec4 operations
- Addition, subtraction, scaling
- Dot product, cross product (Vec3)
- Length/normalization
- Zero vector edge cases
- Component access

**Matrix Tests:**
- Mat3, Mat4 operations
- Matrix multiplication
- Identity matrix
- Transformation matrices
- Inverse calculations (if used)

**Success Criteria:**
- All operations numerically correct
- Edge cases handled (zero vectors, singular matrices)

---

#### 3.2 Arpeggiator Tests
**Priority:** Medium
**Effort:** Medium (3 days)
**File to create:** `src/tests/unit/sequencer/TestArpeggiator.cpp`

**Test Coverage:**
- All arpeggiator modes (Up, Down, Up-Down, Random, etc.)
- Octave range handling
- Gate length
- Note ordering
- Rhythm integration
- Hold functionality

**Success Criteria:**
- All modes produce correct note sequences
- Octave transitions are smooth
- Random mode is reproducible with seed

---

### Phase 4: UI & Data Structures (Week 6) 🟢

**Goal:** Expand UI test coverage and test remaining data structures.

#### 4.1 Python UI Tests Expansion
**Priority:** Medium
**Effort:** Medium (4 days)
**Files to create:**
- `src/apps/sequencer/tests/ui/sequence.py`
- `src/apps/sequencer/tests/ui/track.py`
- `src/apps/sequencer/tests/ui/song.py`
- `src/apps/sequencer/tests/ui/routing.py`

**Test Coverage:**

**Sequence Editing:**
- Step entry/editing
- Layer switching
- Copy/paste operations
- Clear/fill operations
- Pattern generator invocation

**Track Configuration:**
- Track mode switching (Note/Curve/MidiCv)
- Track settings editing
- Mute/solo functionality

**Song Mode:**
- Pattern arrangement
- Chain playback
- Pattern copy/duplicate

**Routing:**
- CV/Gate assignment
- MIDI routing configuration

**Success Criteria:**
- All major UI workflows have automated tests
- Screenshot regression testing for visual verification

---

#### 4.2 Data Structure Tests
**Priority:** Low-Medium
**Effort:** Small (2 days)
**Files to create:**
- `src/tests/unit/sequencer/TestSortedQueue.cpp`
- `src/tests/unit/sequencer/model/TestClipBoard.cpp`
- `src/tests/unit/sequencer/model/TestObservable.cpp`

**Test Coverage:**

**SortedQueue:**
- Insert/remove operations
- Ordering correctness
- Time-based scheduling
- Queue overflow handling

**ClipBoard:**
- Copy/paste step data
- Copy/paste sequences
- Copy/paste tracks
- Data integrity

**Observable:**
- Observer registration/unregistration
- Notification delivery
- Multiple observers

**Success Criteria:**
- All operations preserve data integrity
- No memory leaks (verify with valgrind in simulator)

---

## Testing Infrastructure Improvements

### 1. Test Helpers & Fixtures

**File to create:** `src/tests/unit/sequencer/SequencerTestHelpers.h`

**Contents:**
- Mock Clock implementation for deterministic testing
- Test fixture for common model objects
- Helper functions for sequence creation
- Assertion macros for floating-point comparison
- Random seed management for reproducible tests

### 2. Coverage Reporting

**Integration:**
- Add gcov/lcov to CMake build configuration
- Generate HTML coverage reports
- Add coverage badge to README.md

**CI/CD Integration:**
- Run coverage analysis on every commit
- Fail CI if coverage drops below threshold
- Upload coverage reports to CodeCov or Coveralls

**Target Metrics:**
- Phase 1 completion: 25% overall coverage
- Phase 2 completion: 40% overall coverage
- Phase 3 completion: 50% overall coverage
- Phase 4 completion: 55-60% overall coverage

### 3. Property-Based Testing

**Recommendation:** Consider integrating Catch2 generators or RapidCheck

**Use Cases:**
- MIDI parser fuzzing (random byte sequences)
- Serialization round-trip testing (random data)
- Pattern generator output validation
- Math library property verification (associativity, commutativity)

### 4. Performance Tests

**Location:** `src/tests/performance/` (new directory)

**Files to create:**
- `PerformanceBenchmark.h` - Benchmark framework
- `ClockBenchmark.cpp` - Clock jitter measurements
- `EngineBenchmark.cpp` - Engine processing time
- `SerializationBenchmark.cpp` - Save/load performance

**Metrics to Track:**
- Clock tick jitter (should be < 100µs)
- Engine processing time per tick (should fit in 1ms window)
- Serialization time for large projects
- Memory allocation tracking

### 5. Regression Test Suite

**Process:**
- For every bug filed, create a test that reproduces it
- Fix the bug
- Verify test passes
- Keep test in suite to prevent regression

**Location:** `src/tests/regression/`

**Naming:** `Issue_XXX_Description.cpp` (where XXX is GitHub issue number)

---

## Metrics & Goals

### Current Coverage Estimate

| Component | Current | Target (6mo) | Priority |
|-----------|---------|--------------|----------|
| Core Utilities | 40% | 80% | Medium |
| Core Math | 0% | 80% | Medium |
| Core MIDI | 0% | 90% | High |
| Core Graphics | 0% | 40% | Low |
| Core File System | 30% | 70% | Medium |
| Engine | 0% | 60% | **Critical** |
| Model | 5% | 70% | **Critical** |
| UI | 10% | 40% | Medium |
| Drivers (integration) | 80% | 85% | Low |
| **Overall** | **15-20%** | **55-60%** | - |

### Success Metrics

**Quantitative:**
- Line coverage: 55-60%
- Branch coverage: 45-50%
- Function coverage: 70-75%
- Zero crashes in test suite
- Test execution time < 60 seconds

**Qualitative:**
- All critical bugs have regression tests
- New features include tests (>50% coverage for new code)
- Test failures block merges to main branch
- Documentation includes test examples

---

## Risk Assessment

### Highest Risk Areas (Low Coverage + High Complexity)

| Risk Level | Component | Current Coverage | Impact | Likelihood |
|------------|-----------|------------------|---------|------------|
| ⚠️⚠️⚠️ Critical | Data Model Serialization | 5% | Data loss, corruption | High |
| ⚠️⚠️⚠️ Critical | Clock Engine | 0% | Timing failures | High |
| ⚠️⚠️ High | Pattern Generators | 0% | Incorrect patterns | Medium |
| ⚠️⚠️ High | MIDI Parsing | 0% | MIDI failures | Medium |
| ⚠️ Medium | Track Engines | 0% | Sequencing bugs | Low-Medium |
| ⚠️ Medium | UI Workflows | 10% | UX issues | Medium |

### Mitigation Strategy

**Immediate (Weeks 1-2):**
- Focus on Critical risk areas
- Implement basic smoke tests for all components
- Set up CI/CD test automation

**Short-term (Weeks 3-6):**
- Complete High risk area testing
- Establish coverage reporting
- Create test writing guidelines

**Medium-term (Months 2-3):**
- Expand to Medium risk areas
- Implement performance testing
- Add fuzzing for input validation

**Long-term (Months 4-6):**
- Maintain and expand coverage
- Add property-based testing
- Regular regression test review

---

## Quick Wins (High Impact, Low Effort)

### Week 1 Quick Wins

1. **MIDI Message Tests** ⚡
   - **Effort:** 1 day
   - **Impact:** High (prevents MIDI output bugs)
   - **File:** `src/tests/unit/core/midi/TestMidiMessage.cpp`

2. **Math Vector Tests** ⚡
   - **Effort:** 1 day
   - **Impact:** Medium (catches calculation errors)
   - **File:** `src/tests/unit/core/math/TestVec.cpp`

3. **Euclidean Generator Tests** ⚡
   - **Effort:** 1-2 days
   - **Impact:** High (algorithmically complex, easy to break)
   - **File:** `src/tests/unit/sequencer/generators/TestEuclidean.cpp`

4. **Expand MovingAverage Tests** ⚡
   - **Effort:** 0.5 day
   - **Impact:** Low-Medium (already has tests, add edge cases)
   - **File:** `src/tests/unit/core/utils/TestMovingAverage.cpp`

---

## CI/CD Integration

### GitHub Actions Workflow Updates

**File:** `.github/workflows/ci.yml`

**Additions:**
1. Run tests on all platforms (STM32 simulator, Linux, macOS)
2. Generate coverage reports (Linux only)
3. Upload coverage to CodeCov
4. Fail if coverage drops > 1%
5. Performance regression detection

**Test Matrix:**
```yaml
test:
  runs-on: ${{ matrix.os }}
  strategy:
    matrix:
      os: [ubuntu-latest, macos-latest]
      build-type: [Debug, Release]
  steps:
    - name: Run Tests
      run: cd build/sim/${{ matrix.build-type }} && make test
    - name: Generate Coverage (Debug only)
      if: matrix.build-type == 'Debug'
      run: |
        lcov --capture --directory . --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage.info
        lcov --list coverage.info
    - name: Upload Coverage
      if: matrix.build-type == 'Debug' && matrix.os == 'ubuntu-latest'
      uses: codecov/codecov-action@v3
```

---

## Test Writing Guidelines

### General Principles

1. **Test Naming:** Use descriptive names that explain what is being tested
   - ✅ Good: `testClockTickGenerationAt120BPM`
   - ❌ Bad: `testClock1`

2. **Test Independence:** Each test should be independent and not rely on others
   - Use setup/teardown for initialization
   - Don't share state between tests

3. **One Assertion Per Test (when practical):** Focus tests on single behaviors
   - Exception: Complex state setup may require multiple assertions

4. **Test Coverage:** Aim for edge cases, not just happy path
   - Min/max values
   - Zero, negative, overflow
   - Null/empty inputs
   - Boundary conditions

5. **Deterministic Tests:** Avoid flaky tests
   - Use fixed seeds for random generators
   - Mock time-dependent code
   - Don't rely on exact timing in unit tests

### Example Test Structure

```cpp
#include "UnitTest.h"
#include "apps/sequencer/engine/Clock.h"

// Test fixture
class ClockTest : public UnitTest {
protected:
    // Setup runs before each test
    void setUp() override {
        // Initialize test objects
    }

    // Teardown runs after each test
    void tearDown() override {
        // Cleanup
    }
};

// Individual test
TEST_F(ClockTest, testMasterTickGenerationAt120BPM) {
    Clock clock(timer);
    clock.setMode(Clock::Mode::Master);
    clock.setMasterBpm(120.0f);
    clock.masterStart();

    // Test expectation
    EXPECT_EQ(clock.bpm(), 120.0f);
    EXPECT_TRUE(clock.isRunning());
}
```

---

## Resources & References

### Testing Tools

- **Test Framework:** Catch2 (or existing framework)
- **Coverage:** gcov, lcov
- **Profiling:** gprof, Valgrind
- **Fuzzing:** libFuzzer, AFL
- **CI/CD:** GitHub Actions

### Documentation

- [CLAUDE.md](CLAUDE.md) - Project overview and guidelines
- [README.md](README.md) - Development setup
- [src/tests/](src/tests/) - Existing test examples

### Testing Best Practices

- Google Test Best Practices
- Effective Unit Testing (Martin Fowler)
- Test-Driven Development (Kent Beck)

---

## Maintenance & Review

### Regular Reviews

- **Weekly:** Review test failures, flaky tests
- **Monthly:** Review coverage reports, identify gaps
- **Quarterly:** Update this plan based on progress

### Test Maintenance

- Keep tests updated with code changes
- Remove obsolete tests
- Refactor tests when duplicated logic appears
- Document test-specific conventions

### Success Indicators

- CI/CD pipeline is green
- Coverage trend is increasing
- Bug regression rate is decreasing
- Developer confidence in refactoring is high

---

## Appendix A: Test File Locations

### Proposed Directory Structure

```
src/tests/
├── unit/
│   ├── core/
│   │   ├── io/                    ✅ EXISTS
│   │   │   ├── TestSerialization.cpp
│   │   │   └── TestVersionedSerialization.cpp
│   │   ├── utils/                 ✅ EXISTS
│   │   │   ├── TestStringUtils.cpp
│   │   │   ├── TestObjectPool.cpp
│   │   │   ├── TestRandom.cpp
│   │   │   └── TestMovingAverage.cpp
│   │   ├── midi/                  ⚠️ TO CREATE
│   │   │   ├── TestMidiParser.cpp
│   │   │   └── TestMidiMessage.cpp
│   │   ├── math/                  ⚠️ TO CREATE
│   │   │   ├── TestVec.cpp
│   │   │   └── TestMat.cpp
│   │   └── gfx/                   ⚠️ TO CREATE (Phase 4+)
│   │       ├── TestCanvas.cpp
│   │       └── TestFrameBuffer.cpp
│   └── sequencer/
│       ├── TestScale.cpp          ✅ EXISTS
│       ├── TestCurve.cpp          ✅ EXISTS
│       ├── TestClock.cpp          ⚠️ TO CREATE (Phase 1)
│       ├── TestArpeggiator.cpp    ⚠️ TO CREATE (Phase 3)
│       ├── model/                 ⚠️ TO CREATE
│       │   ├── TestNoteSequence.cpp      (Phase 1)
│       │   ├── TestCurveSequence.cpp     (Phase 2)
│       │   ├── TestProjectSerialization.cpp (Phase 2)
│       │   ├── TestClipBoard.cpp         (Phase 4)
│       │   └── TestObservable.cpp        (Phase 4)
│       ├── engine/                ⚠️ TO CREATE
│       │   ├── TestNoteTrackEngine.cpp   (Phase 2)
│       │   ├── TestCurveTrackEngine.cpp  (Phase 2)
│       │   └── TestSortedQueue.cpp       (Phase 4)
│       └── generators/            ⚠️ TO CREATE
│           ├── TestEuclidean.cpp         (Phase 2)
│           ├── TestRandom.cpp            (Phase 2)
│           └── TestRhythm.cpp            (Phase 2)
├── integration/                   ✅ EXISTS (Good coverage)
│   ├── drivers/
│   │   ├── TestAdc.cpp
│   │   ├── TestDac.cpp
│   │   ├── TestMidi.cpp
│   │   └── ... (10 driver tests)
│   └── fs/
│       └── TestFileSystem.cpp
├── performance/                   ⚠️ TO CREATE (Phase 3-4)
│   ├── ClockBenchmark.cpp
│   ├── EngineBenchmark.cpp
│   └── SerializationBenchmark.cpp
└── regression/                    ⚠️ TO CREATE (As needed)
    └── Issue_XXX_Description.cpp

src/apps/sequencer/tests/
├── runner.py                      ✅ EXISTS
├── manual-screenshots.py          ✅ EXISTS
├── testframework/                 ✅ EXISTS
│   ├── __init__.py
│   └── controller.py
└── ui/
    ├── __init__.py                ✅ EXISTS
    ├── project.py                 ✅ EXISTS
    ├── sequence.py                ⚠️ TO CREATE (Phase 4)
    ├── track.py                   ⚠️ TO CREATE (Phase 4)
    ├── song.py                    ⚠️ TO CREATE (Phase 4)
    └── routing.py                 ⚠️ TO CREATE (Phase 4)
```

---

## Appendix B: Estimated Effort Summary

| Phase | Duration | Files to Create | Estimated LOC | Priority |
|-------|----------|-----------------|---------------|----------|
| Phase 1 | 2 weeks | 3 test files | ~1,500 | Critical |
| Phase 2 | 2 weeks | 6 test files | ~2,500 | High |
| Phase 3 | 1 week | 3 test files | ~1,000 | Medium |
| Phase 4 | 1 week | 8 test files | ~1,500 | Medium |
| **Total** | **6 weeks** | **20 test files** | **~6,500** | - |

**Additional Infrastructure:**
- Test helpers and fixtures: ~500 LOC
- Coverage reporting setup: 2-3 days
- CI/CD integration: 1-2 days

**Grand Total:** ~7 weeks of focused testing effort for core coverage improvements

---

## Document Changelog

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-11-16 | Claude | Initial comprehensive testing plan |

---

**Next Steps:**
1. Review and approve this plan
2. Create GitHub issues for each phase
3. Assign priorities and owners
4. Begin Phase 1 implementation
5. Set up coverage reporting infrastructure

**Questions or Feedback:**
- Open GitHub issues for plan modifications
- Tag with `testing` label
- Link to this document in discussions
