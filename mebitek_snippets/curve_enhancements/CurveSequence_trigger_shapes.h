/**
 * MEBITEK FORK - Curve Sequence Trigger Shapes
 *
 * Enhancement in v0.2.2: Trigger curve shape parameter
 *
 * This parameter defines how curves respond to trigger events,
 * particularly useful with Logic Track integration.
 */

#pragma once

#include "Config.h"
#include "Types.h"

// Snippet showing trigger curve shape implementation
class CurveSequence_TriggerShapes {
public:
    //----------------------------------------
    // TRIGGER CURVE SHAPE TYPES
    //----------------------------------------

    /**
     * TRIGGER CURVE SHAPES
     *
     * Defines how the curve track responds to trigger/gate events.
     * This is particularly useful when using curves with Logic Track
     * or when creating envelope-like behavior.
     */
    enum class TriggerCurveShape : uint8_t {
        Rise,       // Trigger on rising edge (gate goes high)
        Fall,       // Trigger on falling edge (gate goes low)
        Both,       // Trigger on both edges
        Gate,       // Follow gate duration (high while gate is high)
        Last
    };

    TriggerCurveShape triggerCurveShape() const {
        return _triggerCurveShape;
    }

    void setTriggerCurveShape(TriggerCurveShape shape) {
        _triggerCurveShape = shape;
    }

    static const char* triggerCurveShapeName(TriggerCurveShape shape) {
        switch (shape) {
            case TriggerCurveShape::Rise: return "Rise";
            case TriggerCurveShape::Fall: return "Fall";
            case TriggerCurveShape::Both: return "Both";
            case TriggerCurveShape::Gate: return "Gate";
            default: return nullptr;
        }
    }

    //----------------------------------------
    // NOTE FILTER PARAMETER
    //----------------------------------------

    /**
     * NOTE FILTER (v0.2.2)
     *
     * Allows filtering which notes/events trigger curve playback.
     * Works in conjunction with trigger shape to create complex
     * conditional curve behavior.
     *
     * Use cases:
     * - Only trigger curves for notes in specific range
     * - Filter out certain pitches
     * - Create note-dependent modulation
     */
    int noteFilterMin() const { return _noteFilterMin; }
    void setNoteFilterMin(int min) {
        _noteFilterMin = clamp(min, 0, 127);
    }

    int noteFilterMax() const { return _noteFilterMax; }
    void setNoteFilterMax(int max) {
        _noteFilterMax = clamp(max, 0, 127);
    }

    bool notePassesFilter(int note) const {
        return (note >= _noteFilterMin && note <= _noteFilterMax);
    }

private:
    TriggerCurveShape _triggerCurveShape;
    int8_t _noteFilterMin;
    int8_t _noteFilterMax;
};

/**
 * USAGE EXAMPLES:
 *
 * 1. ATTACK-RELEASE ENVELOPE:
 *    Trigger Shape: Rise
 *    Curve: Rising exponential
 *    Result: Envelope triggers on note-on, rises to peak
 *    Use: Attack portion of envelope
 *
 * 2. RELEASE ENVELOPE:
 *    Trigger Shape: Fall
 *    Curve: Falling exponential
 *    Result: Envelope triggers on note-off, falls to zero
 *    Use: Release portion of envelope
 *
 * 3. FULL AD ENVELOPE:
 *    Trigger Shape: Both
 *    Curve: Triangle or custom shape
 *    Result: Complete attack-decay envelope
 *    Use: Percussive modulation
 *
 * 4. GATE-FOLLOWING MODULATION:
 *    Trigger Shape: Gate
 *    Curve: Any shape
 *    Result: Curve output only when gate is high
 *    Use: Gated LFO, rhythmic modulation
 *
 * 5. LOGIC TRACK INTEGRATION:
 *    Logic Track: Generates complex gate pattern
 *    Curve Track: Responds to logic output
 *    Trigger Shape: Both
 *    Result: Curves triggered by logical combinations
 *    Use: Polyrhythmic modulation
 *
 * 6. NOTE-DEPENDENT MODULATION:
 *    Note Filter: Min=60 (C4), Max=72 (C5)
 *    Trigger Shape: Rise
 *    Result: Curves only trigger for notes in middle octave
 *    Use: Pitch-dependent filter modulation
 *
 * INTERACTION WITH LOGIC TRACK:
 *
 * Logic Track can output complex gate patterns through boolean operations.
 * Curve Track with trigger shapes responds to these patterns:
 *
 * Example:
 * - Logic Track: AND operation on two rhythmic patterns
 * - Curve Track: Trigger Shape = Rise
 * - Result: Envelope triggers only when both patterns align
 * - Creates polyrhythmic modulation patterns
 *
 * COMPARISON WITH ORIGINAL:
 *
 * Original Curve Track:
 * - Curves play continuously based on sequence steps
 * - No trigger-based behavior
 * - Always active during playback
 *
 * Mebitek Enhancement:
 * - Trigger-based curve playback
 * - Responds to gate edges
 * - More envelope-like behavior
 * - Better integration with rhythmic patterns
 * - Conditional triggering via note filter
 *
 * PRACTICAL APPLICATIONS:
 *
 * 1. RHYTHMIC FILTER SWEEPS:
 *    - Curve controls filter cutoff
 *    - Trigger Shape: Rise
 *    - Gate pattern determines when sweeps occur
 *    - Creates rhythmic filter movement
 *
 * 2. DYNAMIC ENVELOPES:
 *    - Traditional envelope generator replacement
 *    - Trigger Shape: Both (full AD envelope)
 *    - Customize envelope shape per step
 *    - Variable envelope shapes in sequence
 *
 * 3. CONDITIONAL MODULATION:
 *    - Note Filter restricts triggering
 *    - Only certain notes trigger modulation
 *    - Pitch-dependent effects
 *    - More musical modulation behavior
 *
 * 4. POLYRHYTHMIC MODULATION:
 *    - Logic Track generates complex rhythm
 *    - Curve Track responds with envelopes
 *    - Creates evolving modulation patterns
 *    - Non-repeating modulation sequences
 *
 * IMPLEMENTATION NOTES:
 *
 * 1. EDGE DETECTION:
 *    - System must detect rising/falling edges
 *    - Requires tracking previous gate state
 *    - Edge detection per step or per sample
 *
 * 2. TIMING:
 *    - Rise: Curve starts on gate high transition
 *    - Fall: Curve starts on gate low transition
 *    - Both: Curve restarts on any transition
 *    - Gate: Curve active while gate high
 *
 * 3. CURVE PLAYBACK:
 *    - Trigger determines when curve starts
 *    - Curve can complete or be interrupted
 *    - Behavior depends on curve length vs gate duration
 *
 * 4. MULTI-CURVE RECORDING (v0.2.0):
 *    - Can record multiple CV inputs as curves
 *    - Trigger shapes apply to recorded curves
 *    - Enables complex automation recording
 */
