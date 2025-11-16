# VS_MEBITEK.md - Feature Comparison with Mebitek Fork

## Overview

This document compares the **PEW|FORMER** firmware (current repository, based on djphazer's fork) with the **mebitek/performer** fork. Both forks use the same hardware platform (STM32F405-based Eurorack sequencer) but have diverged significantly in features and approach.

**Repository Links:**
- **This Repository (PEW|FORMER):** https://github.com/djphazer/performer
- **Mebitek Fork:** https://github.com/mebitek/performer

---

## Philosophy & Approach

### PEW|FORMER (This Repository)
- **Conservative approach:** Based on known-good master branch from original westlicht firmware
- **Selective integration:** Carefully cherry-picking stable improvements from other forks (e.g., jackpf)
- **Stability focus:** Updated toolchain (GCC 14.2) and modernized codebase while maintaining compatibility
- **Clean implementation:** Minimal feature additions, focus on refinement and stability

### Mebitek Fork
- **Feature-rich approach:** Extensive new functionality and track types
- **Experimental features:** Multiple new sequencing paradigms (stochastic, logic, arpeggiator)
- **Active development:** Regular feature additions and enhancements
- **Expanded sequencing:** Focus on new creative sequencing methods

---

## Track Types Comparison

### Current Repository (PEW|FORMER)

| Track Type | Description | Status |
|------------|-------------|--------|
| **Note Track** | Traditional step sequencer for melodic sequences | ✅ Core feature |
| **Curve Track** | CV modulation sequences with shapes and envelopes | ✅ Core feature |
| **MIDI/CV Track** | MIDI-to-CV conversion with arpeggiator support | ✅ Core feature |

**Total Track Types:** 3

### Mebitek Fork

| Track Type | Description | Version Introduced |
|------------|-------------|-------------------|
| **Note Track** | Traditional step sequencer for melodic sequences | Original |
| **Curve Track** | CV modulation sequences with shapes and envelopes | Original |
| **MIDI/CV Track** | MIDI-to-CV conversion with arpeggiator support | Original |
| **Stochastic Track** | Probabilistic/algorithmic sequence generation | v0.2.0 (Feb 2024) |
| **Logic Track** | Per-step logic operations on gates and notes | v0.2.2 (Mar 2024) |
| **Arpeggiator Track** | Dedicated arpeggiator sequencing track | v0.3.0 (May 2024) |

**Total Track Types:** 6 (3 additional)

---

## Detailed Feature Comparison

### Stochastic Track (Mebitek Only)

**Introduced:** v0.2.0 (February 29, 2024)

A completely new track type focused on generative and probabilistic sequencing:

**Core Features:**
- **Algorithmic sequence generation** - Create sequences using probability and randomness
- **Global octave modifier** - Shift entire sequence by octaves
- **Gate length control** - Per-pattern gate duration settings
- **Loop functionality** - Define loop regions within sequences
- **Reseed capability** - Reset random seed for repeatable "random" patterns
- **Rest probability** - Configurable probability for rests at 2, 4, 8, and 15-step intervals
- **Sequence library** - Save and load stochastic patterns from library
- **Clipboard support** - Copy/paste stochastic sequences
- **Generator algorithms** - Built-in pattern generation algorithms

**Use Cases:**
- Generative ambient sequences
- Controlled randomness with repeatable results
- Euclidean-style rhythm generation with probability
- Algorithmic composition

**Current Repository Status:** ❌ Not implemented

---

### Logic Track (Mebitek Only)

**Introduced:** v0.2.2 (March 20, 2024)

A track type that applies logical operations to gates and notes on a per-step basis:

**Core Features:**
- **Per-step gate logic operators** - Apply boolean logic to gate signals
  - AND, OR, XOR, NOT operations
  - Combine multiple gate sources
  - Create complex rhythmic relationships
- **Per-step note logic operators** - Transform note values using logic
  - Note filtering and masking
  - Conditional note triggers
  - Note transformation per step
- **Fast input visualization** - Visual feedback using shift key for quick editing
- **Trigger curve shapes** - Define curve responses to logic conditions
- **Note filter parameters** - Per-step filtering of note values

**Enhanced Capabilities (v0.3.1):**
- Improved visualization for faster workflow
- Fixed gate and probability calculations
- Better clipboard integration

**Use Cases:**
- Complex polyrhythmic patterns
- Gate multiplication/division
- Conditional sequencing
- Cross-track gate logic

**Current Repository Status:** ❌ Not implemented

---

### Arpeggiator Track (Mebitek Only)

**Introduced:** v0.3.0 (May 2, 2024)

A dedicated track type specifically for arpeggiator functionality (distinct from MIDI/CV arpeggiator):

**Core Features:**
- **Dedicated arpeggiator sequencer** - Full track devoted to arp patterns
- **Pattern management** - Copy, paste, and store arp patterns
- **Integration with stochastic** - Can combine with probabilistic elements
- **Launchpad support** - Full controller integration
- **Clipboard operations** - Standard copy/paste workflow

**Bug Fixes (v0.3.1-v0.3.2):**
- Arpeggiator pattern copy functionality fixed
- Launchpad X row 3 note change issues resolved
- General arp stability improvements

**Differences from MIDI/CV Arpeggiator:**
- Full track with independent sequencing
- More complex pattern capabilities
- Better integration with other sequencer features
- Dedicated UI and controls

**Use Cases:**
- Complex arpeggiated sequences
- Multi-octave arpeggio patterns
- Rhythmic arpeggio variations
- Dedicated arpeggiator per track (up to 8 simultaneous)

**Current Repository Status:** ⚠️ Partially - MIDI/CV track has basic arpeggiator, but not a dedicated track type

---

## Enhanced Features in Mebitek Fork

### Curve Track Enhancements

**Mebitek Additions (v0.2.2):**
- ✅ **CV-controllable minimum values** - Modulate curve min via CV input
- ✅ **CV-controllable maximum values** - Modulate curve max via CV input
- ✅ **Trigger curve shapes** - More responsive curve triggering
- ✅ **Multi-curve CV recording** - Record multiple curves simultaneously

**Current Repository:**
- ✅ Basic curve track functionality
- ✅ Shape variations and probability
- ✅ Gate and gate probability layers
- ✅ Offset parameter
- ✅ Slide time parameter
- ❌ CV-controllable min/max (not implemented)

---

### Launchpad Controller Support

**Mebitek Additions:**
- ✅ **Performance Mode** - Dedicated performance view on Launchpad (v0.2.0)
- ✅ **Circuit Mode** - Circuit-style note editing
- ✅ **Follow Mode** - Launchpad follows pattern changes (v0.3.1)
- ✅ **Pattern Follow Persistence** - Follows saved in projects (v0.3.1)
- ✅ **Responsive Design** - Improved LaunchPad X responsiveness (v0.3.2)
- ✅ Support for: Launchpad S, Mini, Mk2, Pro, Mini Mk3, X, Pro Mk3

**Current Repository:**
- ✅ Basic Launchpad support (S, Mini, Mk2, Pro, Mini Mk3, X, Pro Mk3)
- ✅ Pattern page functionality
- ✅ Sequence editing on Launchpad
- ✅ Double-tap gate toggle
- ❌ Performance Mode (not implemented)
- ❌ Circuit Mode (not implemented)
- ⚠️ Follow mode may differ

---

### Sequence Manipulation

**Mebitek Additions:**
- ✅ **Sequence Library** - Save/load sequences to library (v0.2.0)
- ✅ **Enhanced step rotation** - Improved rotate functionality (v0.3.0)
- ✅ **Step recorder shortcuts** - "Move step forward" shortcuts (v0.2.2)
- ✅ **Enhanced clipboard** - Better copy/paste for all track types (v0.3.0)

**Current Repository:**
- ✅ Basic clipboard functionality
- ✅ Copy/paste sequences
- ✅ Duplicate sequences
- ❌ Sequence library (not implemented)
- ⚠️ Step rotation available but may differ

---

### User Interface Enhancements

**Mebitek Additions:**
- ✅ **F1-F5 keyboard shortcuts** - Direct key access for functions
- ✅ **Shift key visualization** - Fast input visual feedback
- ✅ **Responsive LaunchPad pages** - Better controller responsiveness
- ✅ **Follow pattern persistence** - UI state saved in projects

**Current Repository:**
- ✅ **Double-click track buttons** - Edit from most pages (v0.5)
- ✅ **Version mode on Monitor page** - Simplified build info display
- ✅ **Screensaver with Intro animation** - Visual feedback with gate pulses
- ✅ Standard function key support
- ⚠️ Different UI enhancements focus

---

### MIDI Features

**Mebitek Additions:**
- ✅ **Program change reception** - Respond to MIDI program changes (v0.3.1)
- ✅ Standard MIDI I/O functionality

**Current Repository:**
- ✅ **Forward MIDI program changes** - Pass-through to output (from jackpf)
- ✅ Standard MIDI I/O functionality
- ✅ USB-MIDI support
- ✅ MIDI routing system

---

### Integration Features

**Mebitek Only:**
- ✅ **Malekko integration** - Support for Malekko hardware (v0.3.0)

**Current Repository:**
- ❌ Malekko integration not present

---

## Stability & Bug Fixes

### Mebitek Fork Recent Fixes (v0.3.x)

- Program change functionality restored
- Launchpad follow mode corrected
- Follow pattern persistence
- Curve CV routing issues resolved
- File stack overflow fixed
- Stochastic reseed behavior normalized
- Arpeggiator bugs addressed
- LaunchPad X compatibility issues fixed

### Current Repository Focus

- **Toolchain stability** - GCC 14.2, libopencm3 Oct 2024
- **Code modernization** - C++11 improvements (typedef → using)
- **Compiler warnings** - Clean compilation
- **Build system** - Improved CMake, clangd support
- **Known-good base** - Started from stable master branch

---

## Version Comparison

### Mebitek Fork

- **Current Version:** v0.3.2 (October 2024)
- **Major Releases:**
  - v0.3.0 (May 2024) - Arpeggiator Track
  - v0.2.2 (March 2024) - Logic Track enhancements
  - v0.2.0 (February 2024) - Stochastic Track
  - v0.1.x - Base features + enhancements

### Current Repository (PEW|FORMER)

- **Current Version:** v0.5 (November 2024)
- **Based on:** westlicht master + jackpf improvements
- **Major Changes:**
  - Updated toolchain (GCC 14.2)
  - Modern libopencm3 (October 2024)
  - Code modernization
  - Selective feature integration

---

## Technical Differences

### Code Complexity

**Mebitek:**
- 73+ files in model directory
- 6 track types with dedicated implementations
- More complex data model
- Larger feature surface area
- More potential for edge cases

**PEW|FORMER:**
- Fewer files in model directory
- 3 core track types
- Simpler data model
- Focused feature set
- Conservative approach reduces bugs

### Memory Usage

**Mebitek:**
- Larger memory footprint (more track types)
- More complex state management
- Additional sequence libraries

**PEW|FORMER:**
- Smaller memory footprint
- Streamlined for 192KB RAM constraint
- Efficient core implementation

### Toolchain

**Mebitek:**
- Standard ARM GCC toolchain
- Standard build process

**PEW|FORMER:**
- **GCC 14.2** (latest)
- **libopencm3 October 2024** (latest)
- Modernized build system
- Compilation database support (clangd)

---

## Migration Considerations

### Moving from Mebitek to PEW|FORMER

**⚠️ BREAKING CHANGES:**

If you have projects using Mebitek firmware, they will **NOT** work on PEW|FORMER:

- ❌ **Stochastic tracks** will not load
- ❌ **Logic tracks** will not load
- ❌ **Arpeggiator tracks** will not load
- ❌ **Sequence library data** will be lost
- ❌ **Project file format** may be incompatible

**What You Keep:**
- ✅ Note track sequences
- ✅ Curve track sequences
- ✅ MIDI/CV track settings
- ✅ Clock and routing configuration
- ⚠️ May require manual conversion

### Moving from PEW|FORMER to Mebitek

**✅ Generally Compatible:**

PEW|FORMER projects should load on Mebitek (simpler → complex migration):

- ✅ Note tracks should work
- ✅ Curve tracks should work
- ✅ MIDI/CV tracks should work
- ⚠️ Some settings may need adjustment
- ⚠️ Test thoroughly after migration

---

## Feature Matrix

| Feature | PEW|FORMER | Mebitek | Notes |
|---------|------------|---------|-------|
| **Track Types** | | | |
| Note Track | ✅ | ✅ | Core feature |
| Curve Track | ✅ | ✅ | Core feature |
| MIDI/CV Track | ✅ | ✅ | Core feature |
| Stochastic Track | ❌ | ✅ | Mebitek only |
| Logic Track | ❌ | ✅ | Mebitek only |
| Arpeggiator Track | ❌ | ✅ | Mebitek only |
| **Sequencing Features** | | | |
| 64 steps per sequence | ✅ | ✅ | Both |
| 16 patterns per project | ✅ | ✅ | Both |
| Step variations | ✅ | ✅ | Both |
| Probability layers | ✅ | ✅ | Both |
| Song mode | ✅ | ✅ | Both |
| Euclidean generator | ✅ | ✅ | Both |
| Sequence library | ❌ | ✅ | Mebitek only |
| **Curve Track** | | | |
| Shape variations | ✅ | ✅ | Both |
| Gate layers | ✅ | ✅ | Both |
| Slide time | ✅ | ✅ | Both |
| Offset parameter | ✅ | ✅ | Both |
| CV-controlled min/max | ❌ | ✅ | Mebitek only |
| Multi-curve recording | ❌ | ✅ | Mebitek only |
| **MIDI Features** | | | |
| MIDI I/O | ✅ | ✅ | Both |
| USB-MIDI | ✅ | ✅ | Both |
| MIDI routing | ✅ | ✅ | Both |
| Program change forward | ✅ | ❌ | PEW\|FORMER only |
| Program change receive | ❌ | ✅ | Mebitek only |
| **Launchpad Support** | | | |
| Basic support | ✅ | ✅ | Both |
| Multiple models | ✅ | ✅ | Both |
| Performance Mode | ❌ | ✅ | Mebitek only |
| Circuit Mode | ❌ | ✅ | Mebitek only |
| Follow Mode | ⚠️ | ✅ | Different impl. |
| **User Interface** | | | |
| Double-click track edit | ✅ | ❌ | PEW\|FORMER only |
| Screensaver animation | ✅ | ❌ | PEW\|FORMER only |
| Version on Monitor page | ✅ | ❌ | PEW\|FORMER only |
| F1-F5 shortcuts | ⚠️ | ✅ | Different impl. |
| Shift visualization | ❌ | ✅ | Mebitek only |
| **Development** | | | |
| GCC 14.2 | ✅ | ❌ | PEW\|FORMER only |
| Modern libopencm3 | ✅ | ⚠️ | PEW\|FORMER newer |
| C++11 modernization | ✅ | ⚠️ | PEW\|FORMER focus |
| Compilation database | ✅ | ❌ | PEW\|FORMER only |
| **Hardware Support** | | | |
| STM32F405 platform | ✅ | ✅ | Both |
| 8 CV outputs | ✅ | ✅ | Both |
| 4 CV inputs | ✅ | ✅ | Both |
| USB host | ✅ | ✅ | Both |
| SD card | ✅ | ✅ | Both |
| Malekko integration | ❌ | ✅ | Mebitek only |

---

## Use Case Recommendations

### Choose PEW|FORMER (This Repository) If:

- ✅ You want **maximum stability** and a known-good base
- ✅ You prefer **simpler, focused features** over extensive options
- ✅ You need **latest toolchain** and modernized codebase
- ✅ You want **easier debugging** with modern tools
- ✅ You're starting **new projects** without legacy dependencies
- ✅ You prefer **conservative updates** with careful testing
- ✅ You value **clean, maintainable code** over feature count

### Choose Mebitek Fork If:

- ✅ You need **Stochastic Track** for generative sequences
- ✅ You need **Logic Track** for complex gate logic
- ✅ You need **dedicated Arpeggiator Track**
- ✅ You want **sequence library** functionality
- ✅ You use **Malekko hardware** integration
- ✅ You want **Launchpad Performance/Circuit modes**
- ✅ You prefer **feature-rich** over minimal approach
- ✅ You're willing to accept **more complexity** for more features
- ✅ You have **existing Mebitek projects** to maintain

---

## Future Considerations

### Potential Integration from Mebitek → PEW|FORMER

Features that could potentially be integrated if proven stable:

1. **Sequence Library** - Useful for pattern management
2. **CV-controlled Curve Min/Max** - Powerful modulation feature
3. **Launchpad Performance Mode** - Enhanced controller integration
4. **Step recorder shortcuts** - Workflow improvement

Features that would require major changes (unlikely):

1. **Stochastic Track** - Entirely new track type, large codebase addition
2. **Logic Track** - Complex feature requiring extensive testing
3. **Arpeggiator Track** - Already have MIDI/CV arp, redundant

### Development Philosophy Going Forward

**PEW|FORMER Approach:**
- Continue conservative feature integration
- Focus on stability and toolchain updates
- Cherry-pick proven improvements from other forks
- Maintain clean, maintainable codebase
- Prioritize existing features over new additions

**Mebitek Approach:**
- Continue adding innovative sequencing methods
- Expand track type capabilities
- Enhance controller integration
- Provide maximum creative options
- Accept complexity for feature richness

---

## Community & Support

### PEW|FORMER (This Repository)

- **Maintainer:** djphazer (Phazerville)
- **Philosophy:** "From Phazerville with Love"
- **Base:** westlicht master + jackpf improvements
- **Status:** Active maintenance, selective features
- **Support:** GitHub issues

### Mebitek Fork

- **Maintainer:** mebitek
- **Philosophy:** Feature-rich creative sequencer
- **Base:** westlicht original + extensive additions
- **Status:** Active development, regular updates
- **Support:** GitHub issues

---

## Conclusion

Both firmwares are excellent choices depending on your needs:

- **PEW|FORMER** is ideal for those who want a **stable, focused sequencer** with modern toolchain and conservative feature set
- **Mebitek** is ideal for those who want **maximum creative options** and are willing to accept additional complexity

**They are not directly compatible** due to different track types and data models. Choose based on your workflow requirements and willingness to embrace complexity vs. stability.

---

## Quick Decision Guide

**Choose PEW|FORMER if you answer YES to most:**

- [ ] I want the most stable firmware available
- [ ] I prefer simpler, proven features
- [ ] I'm starting a new project from scratch
- [ ] I want the latest development tools
- [ ] I value code quality and maintainability
- [ ] I don't need generative/stochastic sequencing
- [ ] I don't need complex logic operations

**Choose Mebitek if you answer YES to most:**

- [ ] I need generative/stochastic sequencing
- [ ] I need complex gate logic operations
- [ ] I want maximum creative possibilities
- [ ] I have existing Mebitek projects
- [ ] I need sequence library functionality
- [ ] I use Malekko hardware
- [ ] I'm willing to accept additional complexity

---

**Document Version:** 1.0
**Last Updated:** 2025-11-16
**Current PEW|FORMER Version:** v0.5
**Current Mebitek Version:** v0.3.2

---

## References

- **Mebitek Repository:** https://github.com/mebitek/performer
- **PEW|FORMER Repository:** https://github.com/djphazer/performer
- **Original Performer:** https://github.com/westlicht/performer
- **Hardware Design:** https://github.com/westlicht/performer-hardware
