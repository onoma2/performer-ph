/**
 * MEBITEK FORK - Logic Track Overview
 *
 * Introduced in v0.2.2, Logic Track applies logical operations to gates and notes
 * on a per-step basis. This enables complex rhythmic patterns through boolean logic.
 *
 * KEY CONCEPT:
 * - Takes input from other tracks
 * - Applies logic operators (AND, OR, XOR, NOT, etc.)
 * - Outputs modified gate and note patterns
 * - Per-step control of logic operations
 *
 * USE CASES:
 * - Polyrhythmic patterns from simple inputs
 * - Gate multiplication/division
 * - Conditional note triggering
 * - Complex cross-track gate relationships
 * - Note filtering and transformation
 *
 * DIFFERENCES FROM OTHER TRACKS:
 * - Note Track: Generates sequences directly
 * - Logic Track: Processes OTHER tracks' outputs through logic
 * - Acts as a "processor" rather than a "generator"
 */

#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Routing.h"
#include "LogicSequence.h"

class LogicTrack : public BaseTrack, public BaseTrackPatternFollow {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    enum class FillMode : uint8_t {
        None,
        Gates,
        NextPattern,
        Condition,
        Last
    };

    enum class CvUpdateMode : uint8_t {
        Gate,
        Always,
        Last
    };

    //----------------------------------------
    // LOGIC-SPECIFIC: INPUT TRACK SELECTION
    //----------------------------------------

    /**
     * INPUT TRACKS
     *
     * Logic Track processes outputs from other tracks.
     * It can take up to 2 input tracks and apply logic operations.
     *
     * Example:
     * - Input A: Track 1 gate pattern
     * - Input B: Track 3 gate pattern
     * - Logic: AND
     * - Result: Output gate only when both Track 1 AND Track 3 are high
     */

    // First input track (-1 = none, 0-7 = track index)
    int inputTrackA() const { return _inputTrackA; }
    void setInputTrackA(int track) {
        _inputTrackA = clamp(track, -1, 7);
    }

    // Second input track
    int inputTrackB() const { return _inputTrackB; }
    void setInputTrackB(int track) {
        _inputTrackB = clamp(track, -1, 7);
    }

    void printInputTrack(StringBuilder &str, int track) const {
        if (track == -1) {
            str("None");
        } else {
            str("Track%d", track + 1);
        }
    }

    //----------------------------------------
    // LOGIC-SPECIFIC: DETAILED VIEW
    //----------------------------------------

    /**
     * DETAILED VIEW MODE
     *
     * Logic operations can be complex. Detailed view provides
     * enhanced visualization of:
     * - Input states
     * - Logic operations per step
     * - Output results
     *
     * Uses shift key for fast input visualization (v0.3.0)
     */
    bool detailedView() const { return _detailedView; }
    void setDetailedView(bool enabled) {
        _detailedView = enabled;
    }

    //----------------------------------------
    // Standard Track Parameters
    //----------------------------------------

    // Slide time for smooth transitions
    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    // Octave transposition
    int octave() const { return _octave.get(isRouted(Routing::Target::Octave)); }
    void setOctave(int octave, bool routed = false) {
        _octave.set(clamp(octave, -10, 10), routed);
    }

    // Transpose in semitones
    int transpose() const { return _transpose.get(isRouted(Routing::Target::Transpose)); }
    void setTranspose(int transpose, bool routed = false) {
        _transpose.set(clamp(transpose, -100, 100), routed);
    }

    // Pattern rotation
    int rotate() const { return _rotate.get(isRouted(Routing::Target::Rotate)); }
    void setRotate(int rotate, bool routed = false) {
        _rotate.set(clamp(rotate, -64, 64), routed);
    }

    //----------------------------------------
    // Probability Biases
    //----------------------------------------

    /**
     * PROBABILITY BIASES IN LOGIC TRACK
     *
     * Even though logic track processes other tracks' outputs,
     * it can still apply probability to the results.
     *
     * Example:
     * - Input A AND Input B = true
     * - Gate probability bias = 50%
     * - Result: Only 50% chance the output gate will trigger
     *
     * This adds another layer of variation on top of logic operations.
     */
    int gateProbabilityBias() const { return _gateProbabilityBias; }
    void setGateProbabilityBias(int bias) {
        _gateProbabilityBias = clamp(bias, -1, LogicSequence::GateProbability::Max);
    }

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias; }
    void setRetriggerProbabilityBias(int bias) {
        _retriggerProbabilityBias = clamp(bias, -1, LogicSequence::RetriggerProbability::Max);
    }

    int lengthBias() const { return _lengthBias; }
    void setLengthBias(int bias) {
        _lengthBias = clamp(bias, -1, LogicSequence::Length::Max);
    }

    int noteProbabilityBias() const { return _noteProbabilityBias; }
    void setNoteProbabilityBias(int bias) {
        _noteProbabilityBias = clamp(bias, -1, LogicSequence::NoteProbability::Max);
    }

    //----------------------------------------
    // Sequence Management
    //----------------------------------------

    const LogicSequence &sequence(int index) const {
        return _sequences[index];
    }
    LogicSequence &sequence(int index) {
        return _sequences[index];
    }

    const LogicSequenceArray &sequences() const { return _sequences; }
    LogicSequenceArray &sequences() { return _sequences; }

private:
    // Routable parameters
    Routable<uint8_t> _playMode;
    Routable<uint8_t> _fillMode;
    Routable<uint8_t> _cvUpdateMode;
    Routable<uint8_t> _slideTime;
    Routable<int8_t> _octave;
    Routable<int8_t> _transpose;
    Routable<int8_t> _rotate;

    // LOGIC-SPECIFIC: Input track references
    int8_t _inputTrackA;  // First input track (-1 = none)
    int8_t _inputTrackB;  // Second input track (-1 = none)

    // LOGIC-SPECIFIC: Detailed view toggle for complex visualization
    bool _detailedView;

    // Probability biases
    int8_t _gateProbabilityBias;
    int8_t _retriggerProbabilityBias;
    int8_t _lengthBias;
    int8_t _noteProbabilityBias;

    // Sequence array
    LogicSequenceArray _sequences;
};

/**
 * EXAMPLE USE CASES:
 *
 * 1. POLYRHYTHM GENERATION:
 *    Input A: 4/4 kick pattern
 *    Input B: 3/4 snare pattern
 *    Logic: AND
 *    Result: Complex polyrhythmic pattern where both align
 *
 * 2. GATE MULTIPLICATION:
 *    Input A: Slow clock (quarter notes)
 *    Input B: Fast clock (16th notes)
 *    Logic: AND
 *    Result: Fast gates only when slow clock is high
 *
 * 3. CONDITIONAL TRIGGERING:
 *    Input A: Main melody
 *    Input B: Control pattern
 *    Logic: Note filter based on Input B
 *    Result: Melody only plays when control pattern allows
 *
 * 4. GATE INVERSION:
 *    Input A: Original pattern
 *    Logic: NOT
 *    Result: Inverted rhythm (rests become gates, gates become rests)
 *
 * 5. CROSS-TRACK RELATIONSHIPS:
 *    Input A: Track 1
 *    Input B: Track 4
 *    Logic: XOR (exclusive or)
 *    Result: Gates when either A or B is high, but not both
 *
 * IMPLEMENTATION NOTES:
 *
 * 1. PROCESSING ORDER:
 *    - Read inputs from specified tracks
 *    - Apply per-step logic operations
 *    - Apply probability biases
 *    - Output results
 *
 * 2. VISUALIZATION:
 *    - Detailed view shows input states and logic results
 *    - Shift key provides fast input visualization (v0.3.0)
 *    - Important for understanding complex logic chains
 *
 * 3. PERFORMANCE:
 *    - Logic operations must be evaluated in real-time
 *    - Consider CPU load when using multiple logic tracks
 *    - Simple logic (AND, OR) is fast
 *    - Complex per-step operations may impact performance
 *
 * 4. MEMORY:
 *    - LogicSequence stores logic operators per step
 *    - Additional RAM for input/output buffers
 *    - Track references (pointers/indices) minimal overhead
 */
