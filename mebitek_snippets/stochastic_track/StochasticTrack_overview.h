/**
 * MEBITEK FORK - Stochastic Track Overview
 *
 * The Stochastic Track is a completely new track type introduced in v0.2.0
 * that enables probabilistic and algorithmic sequence generation.
 *
 * KEY FEATURES:
 * - Generative sequence creation with probability controls
 * - Global octave modifiers for transposition
 * - Loop functionality with reseed capability
 * - Rest probability at multiple intervals (2, 4, 8, 15 steps)
 * - Sequence library save/load functionality
 * - Built-in generator algorithms
 * - Full clipboard support
 *
 * DIFFERENCES FROM NOTE TRACK:
 * - Note Track: Traditional step sequencer with fixed steps
 * - Stochastic Track: Algorithmic/generative with probability-based variation
 */

#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Routing.h"
#include "StochasticSequence.h"

class StochasticTrack : public BaseTrack, public BaseTrackPatternFollow {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    // Fill modes control how fills affect playback
    enum class FillMode : uint8_t {
        None,           // No fill behavior
        Gates,          // Fill affects gate patterns
        NextPattern,    // Fill triggers pattern change
        Condition,      // Fill based on conditions
        Last
    };

    // CV update modes determine when CV outputs are updated
    enum class CvUpdateMode : uint8_t {
        Gate,   // Update CV only when gate is active
        Always, // Always update CV regardless of gate state
        Last
    };

    //----------------------------------------
    // STOCHASTIC-SPECIFIC PROPERTIES
    //----------------------------------------

    // Global octave modifier - shifts entire sequence by octaves
    // This is different from per-step octave in that it applies
    // to the whole pattern for quick transposition
    int octave() const { return _octave.get(isRouted(Routing::Target::Octave)); }
    void setOctave(int octave, bool routed = false) {
        _octave.set(clamp(octave, -10, 10), routed);
    }

    // Gate length affects all gates in the sequence
    // Routable parameter allows external CV control
    int gateLength() const { return _gateLength.get(isRouted(Routing::Target::GateLength)); }
    void setGateLength(int gateLength, bool routed = false) {
        _gateLength.set(clamp(gateLength, 0, 100), routed);
    }

    // Probability biases affect the likelihood of variations occurring
    // Higher values increase probability of variations being applied
    int gateProbabilityBias() const { return _gateProbabilityBias; }
    void setGateProbabilityBias(int bias) {
        _gateProbabilityBias = clamp(bias, -1, StochasticSequence::GateProbability::Max);
    }

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias; }
    void setRetriggerProbabilityBias(int bias) {
        _retriggerProbabilityBias = clamp(bias, -1, StochasticSequence::RetriggerProbability::Max);
    }

    int lengthModifier() const { return _lengthModifier; }
    void setLengthModifier(int modifier) {
        _lengthModifier = clamp(modifier, -1, 100);
    }

    int noteProbabilityBias() const { return _noteProbabilityBias; }
    void setNoteProbabilityBias(int bias) {
        _noteProbabilityBias = clamp(bias, -1, StochasticSequence::NoteProbability::Max);
    }

    //----------------------------------------
    // Sequence Management
    //----------------------------------------

    // Access to individual stochastic sequences
    const StochasticSequence &sequence(int index) const {
        return _sequences[index];
    }
    StochasticSequence &sequence(int index) {
        return _sequences[index];
    }

    // Sequence array contains both patterns and snapshots
    const StochasticSequenceArray &sequences() const { return _sequences; }
    StochasticSequenceArray &sequences() { return _sequences; }

private:
    // Routable properties can be modulated by external CV sources
    Routable<uint8_t> _playMode;
    Routable<uint8_t> _fillMode;
    Routable<uint8_t> _cvUpdateMode;
    Routable<int8_t> _octave;        // Global octave shift
    Routable<int8_t> _transpose;     // Semitone transposition
    Routable<int8_t> _rotate;        // Pattern rotation
    Routable<uint8_t> _gateLength;   // Global gate length

    // Probability biases affect how likely variations are to occur
    int8_t _gateProbabilityBias;
    int8_t _retriggerProbabilityBias;
    int8_t _lengthModifier;
    int8_t _noteProbabilityBias;

    // Array of sequences (patterns + snapshots)
    StochasticSequenceArray _sequences;
};

/**
 * IMPLEMENTATION NOTES:
 *
 * 1. GENERATIVE SEQUENCING:
 *    - Uses probability to determine which steps play
 *    - Can create evolving, non-repeating patterns
 *    - Reseed function allows reproducible "randomness"
 *
 * 2. ROUTING SYSTEM:
 *    - Most parameters support external modulation via CV
 *    - Routable<T> template maintains both local and routed values
 *    - isRouted() checks if parameter is under external control
 *
 * 3. MEMORY CONSIDERATIONS:
 *    - Additional track type increases RAM usage
 *    - Each StochasticSequence stores probabilistic data
 *    - Consider impact on STM32's 192KB RAM limit
 *
 * 4. USE CASES:
 *    - Ambient generative sequences
 *    - Controlled randomness with musical constraints
 *    - Evolving patterns that don't repeat exactly
 *    - Algorithmic composition tools
 */
