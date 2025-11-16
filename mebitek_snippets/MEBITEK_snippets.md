# MEBITEK Fork Code Snippets

This directory contains annotated code snippets from the mebitek/performer fork, focusing on features not present in the PEW|FORMER firmware. These snippets are provided for educational and reference purposes.

**Repository:** https://github.com/mebitek/performer

---

## Overview

The mebitek fork extends the original performer firmware with three additional track types, enhanced curve functionality, and improved Launchpad integration. These snippets document the key architectural patterns and implementation approaches used for these features.

**Important:** These are **reference snippets only**, not drop-in code. They demonstrate concepts and patterns from the mebitek fork but are not directly usable in PEW|FORMER without significant integration work.

---

## Directory Structure

```
mebitek_snippets/
├── stochastic_track/          # Stochastic Track (generative sequencing)
├── arp_track/                 # Arpeggiator Track (dedicated arp)
├── logic_track/               # Logic Track (gate/note logic operators)
├── curve_enhancements/        # Curve Track CV control features
├── launchpad_modes/           # Launchpad Performance & Follow modes
└── MEBITEK_snippets.md        # This file
```

---

## 1. Stochastic Track

**Introduced:** v0.2.0 (February 2024)

Stochastic Track enables probabilistic and algorithmic sequence generation with controllable randomness.

### Files

- **`StochasticTrack_overview.h`** - Track-level properties and global parameters
- **`StochasticSequence_structure.h`** - Sequence and step data structures

### Key Features

| Feature | Description |
|---------|-------------|
| **Rest Probability** | Probabilistic rests at 2, 4, 8, and 15-step intervals |
| **Loop with Reseed** | Repeatable "random" patterns using seed values |
| **Octave Range** | Constrain random generation to specific octave ranges |
| **Global Octave Modifier** | Transpose entire sequence by octaves |
| **Sequence Library** | Save and load stochastic patterns |
| **Generator Algorithms** | Built-in pattern generation algorithms |
| **Full Routing** | CV control of all major parameters |

### Use Cases

- **Generative Ambient:** Non-repeating evolving sequences
- **Controlled Randomness:** Musical chaos within constraints
- **Algorithmic Composition:** Computer-generated patterns
- **Euclidean Variations:** Probability-based rhythm patterns

### Code Highlights

```cpp
// Rest probability at different intervals
int restProbability2();   // Every 2 steps
int restProbability4();   // Every 4 steps
int restProbability8();   // Every 8 steps
int restProbability15();  // Every 15 steps

// Loop controls with reseed for reproducible randomness
int loopFirstStep();
int loopLastStep();

// Octave range constraints
int octaveRangeLow();   // Minimum octave
int octaveRangeHigh();  // Maximum octave
```

### Memory Impact

- Adds ~6 new track types worth of code
- Additional RAM per sequence for probability data
- Loop and seed state storage
- Consider 192KB STM32 RAM limitation

---

## 2. Arpeggiator Track

**Introduced:** v0.3.0 (May 2024)

Dedicated track type for arpeggiator functionality, distinct from MIDI/CV track's basic arpeggiator support.

### Files

- **`ArpTrack_overview.h`** - Track properties and arpeggiator integration

### Key Features

| Feature | Description |
|---------|-------------|
| **Dedicated Arpeggiator** | Full track devoted to arp patterns |
| **Pattern Management** | Copy/paste arp sequences |
| **MIDI Keyboard Toggle** | Enable/disable MIDI keyboard input |
| **Embedded Arpeggiator** | Independent arpeggiator instance per track |
| **Launchpad Integration** | Full controller support |
| **Clipboard Support** | Standard copy/paste workflow |

### Differences from MIDI/CV Arpeggiator

| Aspect | MIDI/CV Track | ArpTrack |
|--------|---------------|----------|
| **Scope** | Feature of MIDI/CV track | Dedicated track type |
| **Complexity** | Basic arpeggiator | Full sequence capabilities |
| **Patterns** | Single arp pattern | Multiple storable patterns |
| **Independence** | Tied to MIDI/CV | Fully independent |
| **Count** | 1 per MIDI/CV track | Up to 8 simultaneous |

### Use Cases

- **Complex Arpeggios:** Multi-octave patterns with probability
- **Independent Arps:** Different arp per track
- **Pattern Variations:** Store and recall arp patterns
- **Rhythmic Arps:** Gate probability and retriggering

### Code Highlights

```cpp
// Embedded arpeggiator instance (one per ArpTrack)
const Arpeggiator &arpeggiator() const;

// MIDI keyboard integration toggle
bool midiKeyboard() const;
void setMidiKeyboard(bool enabled);

// Probability biases for variation
int gateProbabilityBias();
int retriggerProbabilityBias();
int noteProbabilityBias();
```

### Memory Impact

- Each ArpTrack contains full Arpeggiator instance
- ArpSequence array for pattern storage
- Significant RAM per track
- Limited by 192KB total RAM

---

## 3. Logic Track

**Introduced:** v0.2.2 (March 2024)

Logic Track applies boolean and mathematical operations to gates and notes from other tracks.

### Files

- **`LogicTrack_overview.h`** - Track properties and input configuration
- **`LogicSequence_operators.h`** - Per-step logic operators and evaluation

### Key Features

| Feature | Description |
|---------|-------------|
| **Input Track Selection** | Process outputs from 2 input tracks |
| **Gate Logic Operators** | AND, OR, XOR, NAND, NOR, INVERT, PASS |
| **Note Logic Operators** | Filter, transpose, combine, mask notes |
| **Per-Step Logic** | Different operator for each step |
| **Trigger Curve Shapes** | How curves respond to logic results |
| **Note Filtering** | Global and per-step note filtering |
| **Detailed View** | Enhanced visualization (v0.3.0) |

### Logic Operators

#### Gate Logic

```cpp
enum class GateLogic {
    PASS,    // Output = Input A
    INVERT,  // Output = NOT A
    AND,     // Output = A AND B
    OR,      // Output = A OR B
    XOR,     // Output = A XOR B (exclusive or)
    NAND,    // Output = NOT (A AND B)
    NOR,     // Output = NOT (A OR B)
};
```

#### Note Logic

```cpp
enum class NoteLogic {
    PASS,          // Pass note from input A
    TRANSPOSE_A,   // Transpose note from A
    TRANSPOSE_B,   // Transpose note from B
    COMBINE,       // Combine notes from A and B
    FILTER_HIGH,   // Only pass high notes
    FILTER_LOW,    // Only pass low notes
    FILTER_RANGE,  // Only pass notes in range
    MASK,          // Block notes based on input B
};
```

### Use Cases

- **Polyrhythms:** Combine rhythmic patterns via logic
- **Gate Multiplication:** Fast gates when slow gate is high
- **Conditional Notes:** Notes only when control pattern allows
- **Rhythm Inversion:** Convert gates to rests and vice versa
- **Cross-Track Logic:** Complex inter-track relationships

### Examples

```cpp
// Example 1: Euclidean rhythm combination
// Input A: [1,0,0,1,0,0,1,0]  (3/8)
// Input B: [1,0,1,0,1,0,1,0]  (4/8)
// Logic: AND
// Result:  [1,0,0,0,0,0,1,0]  (polyrhythm)

// Example 2: Gate inversion for counter-rhythm
// Input A: [1,0,1,0,1,0,1,0]
// Logic: INVERT
// Result:  [0,1,0,1,0,1,0,1]  (off-beats)
```

### Memory Impact

- LogicSequence stores operator per step
- Input/output buffer overhead
- Real-time logic evaluation
- Minimal memory, moderate CPU

---

## 4. Curve Track Enhancements

**Introduced:** v0.2.2 (March 2024)

Enhancements to the standard curve track, adding CV-controllable min/max and trigger shapes.

### Files

- **`CurveTrack_cv_control.h`** - CV-controllable minimum and maximum values
- **`CurveSequence_trigger_shapes.h`** - Trigger curve shape types and note filtering

### Key Features

| Feature | Description | Version |
|---------|-------------|---------|
| **CV-Controllable Min** | Modulate curve minimum via CV | v0.2.2 |
| **CV-Controllable Max** | Modulate curve maximum via CV | v0.2.2 |
| **Trigger Curve Shapes** | Rise, Fall, Both, Gate triggers | v0.2.2 |
| **Note Filter** | Filter which notes trigger curves | v0.2.2 |
| **Multi-Curve Recording** | Record multiple CV inputs as curves | v0.2.0 |

### Differences from Original

| Aspect | Original/PEW\|FORMER | Mebitek Enhancement |
|--------|---------------------|---------------------|
| **Min/Max** | Sequence parameter (static) | Track parameter (routable) |
| **Range Control** | Manual adjustment only | CV-controllable in real-time |
| **Triggering** | Continuous playback | Trigger-based (envelope-like) |
| **Note Filter** | Not available | Global and per-step filtering |

### Use Cases

- **Dynamic LFO Depth:** CV controls modulation intensity
- **Envelope Following:** Curve depth follows audio dynamics
- **Performance Control:** Mod wheel controls curve range
- **Trigger Envelopes:** Curves act like AD envelopes
- **Conditional Curves:** Only trigger for specific notes

### Code Highlights

```cpp
// CV-controllable minimum value
float min() const {
    return _min.get(isRouted(Routing::Target::CurveMin));
}

// CV-controllable maximum value
float max() const {
    return _max.get(isRouted(Routing::Target::CurveMax));
}

// Trigger curve shapes
enum class TriggerCurveShape {
    Rise,   // Trigger on rising edge
    Fall,   // Trigger on falling edge
    Both,   // Trigger on both edges
    Gate,   // Follow gate duration
};

// Note filtering
bool notePassesFilter(int note) const {
    return (note >= _noteFilterMin && note <= _noteFilterMax);
}
```

### Examples

```cpp
// Dynamic LFO depth control
min = 0V (fixed)
max = routed to CV Input 1 (0-5V)
// Result: CV Input 1 controls LFO depth from 0% to 100%

// Envelope-like behavior
triggerCurveShape = Rise
curve = rising exponential
// Result: Attack envelope on gate-on

// Pitch-dependent modulation
noteFilterMin = 60 (C4)
noteFilterMax = 72 (C5)
// Result: Curves only trigger for middle octave notes
```

---

## 5. Launchpad Modes

**Introduced:** v0.2.0 (Performance Mode), v0.3.1 (Follow Mode)

Enhanced Launchpad controller integration with performance-oriented modes and synchronization.

### Files

- **`LaunchpadPerformanceMode_overview.h`** - Performance mode layout and features
- **`LaunchpadFollowMode_overview.h`** - Follow mode synchronization

### Performance Mode Features

| Feature | Description |
|---------|-------------|
| **Pattern Grid** | 16 patterns on main grid |
| **Track Control Rows** | Dedicated rows for mute/solo/fill |
| **Scheduled Actions** | Sync changes to bar boundaries |
| **Fill Amount** | Control fill intensity (0-100%) |
| **Visual Feedback** | Color-coded status LEDs |
| **Transport Controls** | Play/stop/record access |

### Performance Mode Layout

```
┌──────────────────────────────────────┐
│  PATTERN SELECTION    │  FUNCTIONS   │
│  [1][2][3][4][5][6][7][8]  │ [Func]   │
│  [9][10][11][12][13][14][15][16] │   │
│───────────────────────────────────│
│  TRACK CONTROLS                      │
│  [M1][M2][M3][M4][M5][M6][M7][M8]    │  M = Mute
│  [S1][S2][S3][S4][S5][S6][S7][S8]    │  S = Solo
│  [F1][F2][F3][F4][F5][F6][F7][F8]    │  F = Fill
│───────────────────────────────────│
│  TRANSPORT & GLOBAL                  │
│  [Play][Stop][Rec][<<][>>][All]...   │
└──────────────────────────────────────┘
```

### Follow Mode Features

| Feature | Description | Version |
|---------|-------------|---------|
| **Pattern Follow** | Launchpad follows pattern changes | v0.2.0 |
| **Page Follow** | Launchpad follows page navigation | v0.2.0 |
| **Track Follow** | Launchpad follows track selection | v0.2.0 |
| **Persistent Follow** | Follow state saved in projects | v0.3.1 |
| **Fixed Bugs** | Follow mode corrected | v0.3.1 |

### Use Cases

#### Performance Mode

- **Live Sets:** Quick pattern switching and track control
- **Improvisation:** Experiment with combinations
- **Jamming:** Multiple performers collaborate
- **Arrangement:** Build/breakdown control

#### Follow Mode

- **Synchronized Control:** Launchpad mirrors main unit
- **Reduced Confusion:** Always shows current context
- **Collaborative Performance:** Multiple performers stay synced
- **Teaching:** Visual feedback for students

### Code Highlights

```cpp
// Performance Mode - Scheduled actions
struct ScheduledAction {
    enum Type {
        None,
        PatternChange,
        MuteTrack,
        UnmuteTrack,
        SoloTrack,
    };
    Type type;
    uint8_t track;
    uint8_t pattern;
    bool pending;
};

// Fill amount control
int fillAmount() const { return _fillAmount; }
void setFillAmount(int amount) {
    _fillAmount = clamp(amount, 0, 100);
}

// Follow Mode - Persistent settings
bool patternFollow() const;
bool pageFollow() const;
bool trackFollow() const;

// Save/load follow state
void write(VersionedSerializedWriter &writer) const;
void read(VersionedSerializedReader &reader);
```

---

## Integration Challenges

### Why These Features Aren't in PEW|FORMER

1. **Complexity vs Stability:**
   - PEW|FORMER prioritizes known-good stable base
   - These features add significant complexity
   - More edge cases and potential bugs

2. **Memory Constraints:**
   - STM32 has only 192KB RAM
   - 3 additional track types significantly increase usage
   - Each sequence stores more data
   - May limit project size or track count

3. **Code Size:**
   - More track types = more code
   - May approach flash size limits
   - Increases build time and complexity

4. **Maintenance Burden:**
   - More features = more to maintain
   - More potential bug surfaces
   - Harder to test all combinations

5. **Data Model Compatibility:**
   - Adding track types breaks file format
   - Projects not compatible between forks
   - Migration path complex

### What Could Be Integrated

**Lower Hanging Fruit:**

1. **CV-Controllable Curve Min/Max** ✓ Possible
   - Relatively isolated feature
   - Uses existing routing system
   - Moderate complexity
   - High value for modular users

2. **Launchpad Follow Mode** ✓ Possible
   - UI enhancement, not data model change
   - Low memory overhead
   - Improves workflow significantly
   - No file format impact

3. **Fill Amount Parameter** ✓ Possible
   - Single parameter addition
   - Uses existing fill infrastructure
   - Minimal complexity

**Higher Complexity:**

4. **Trigger Curve Shapes** ⚠️ Moderate
   - Changes curve playback logic
   - Adds sequence-level parameters
   - Moderate implementation effort

5. **Performance Mode** ⚠️ Moderate
   - Launchpad layout changes
   - No data model impact
   - UI/controller work

**Very Difficult:**

6. **Stochastic Track** ❌ Difficult
   - Entirely new track type
   - Large codebase addition
   - Significant memory usage
   - Data model changes
   - Complex testing required

7. **Logic Track** ❌ Difficult
   - New track type with complex logic
   - Real-time processing overhead
   - Inter-track dependencies
   - Data model changes

8. **Arpeggiator Track** ❌ Difficult
   - Redundant with MIDI/CV arpeggiator
   - Adds track type complexity
   - Questionable value-add

---

## Learning from These Snippets

### Architecture Patterns

1. **Routable Parameters:**
   - Template-based routing system
   - Separate local and routed values
   - Used consistently across features

2. **Bitfield Packing:**
   - Efficient storage using unions
   - Critical for RAM-limited embedded systems
   - Carefully aligned to 32/64-bit boundaries

3. **Layered Sequence Data:**
   - Step class with packed bitfields
   - Sequence class with global parameters
   - Track class with routing integration

4. **Version-Aware Serialization:**
   - All features use versioned read/write
   - Enables backward compatibility
   - Critical for long-term maintenance

### Design Decisions

1. **Track Types as Classes:**
   - Each track type is separate class
   - Inheritance from BaseTrack
   - Polymorphic access via Track wrapper
   - Clean separation of concerns

2. **Probability Throughout:**
   - Most parameters have probability variants
   - Consistent UI pattern
   - Generative music focus

3. **Routing Integration:**
   - CV modulation of nearly everything
   - Modular synthesis workflow
   - Performance-oriented

4. **Controller Integration:**
   - Launchpad given first-class support
   - Dedicated modes for different workflows
   - Visual feedback emphasis

---

## Conclusion

These snippets demonstrate the mebitek fork's approach to extending the performer firmware with generative, logical, and performance-oriented features. While these features add significant creative possibilities, they come with complexity, memory, and compatibility trade-offs.

The PEW|FORMER firmware takes a different approach: stability, toolchain modernization, and careful feature selection. Both approaches are valid and serve different user needs.

### For Developers

- **Study the patterns:** Routing, bitfield packing, serialization
- **Understand trade-offs:** Features vs stability, memory vs capability
- **Learn from integration:** How features interact with existing systems
- **Consider alternatives:** Could similar goals be achieved differently?

### For Users

- **Choose the right firmware:** Based on your needs (see VS_MEBITEK.md)
- **Understand limitations:** File format incompatibility between forks
- **Plan accordingly:** Stick with one fork for project continuity

---

## File Listing

```
mebitek_snippets/
│
├── stochastic_track/
│   ├── StochasticTrack_overview.h
│   └── StochasticSequence_structure.h
│
├── arp_track/
│   └── ArpTrack_overview.h
│
├── logic_track/
│   ├── LogicTrack_overview.h
│   └── LogicSequence_operators.h
│
├── curve_enhancements/
│   ├── CurveTrack_cv_control.h
│   └── CurveSequence_trigger_shapes.h
│
├── launchpad_modes/
│   ├── LaunchpadPerformanceMode_overview.h
│   └── LaunchpadFollowMode_overview.h
│
└── MEBITEK_snippets.md (this file)
```

---

**Document Version:** 1.0
**Last Updated:** 2025-11-16
**Mebitek Version Referenced:** v0.3.2
**PEW|FORMER Version:** v0.5

---

## References

- **Mebitek Repository:** https://github.com/mebitek/performer
- **Mebitek Changelog:** https://github.com/mebitek/performer/blob/master/CHANGELOG.md
- **PEW|FORMER Repository:** https://github.com/djphazer/performer
- **Original Performer:** https://github.com/westlicht/performer
- **Comparison Document:** ../VS_MEBITEK.md
