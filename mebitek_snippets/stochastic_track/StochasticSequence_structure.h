/**
 * MEBITEK FORK - Stochastic Sequence Structure
 *
 * This file demonstrates the key data structures used in Stochastic Sequences.
 * The stochastic sequence differs from Note Sequence by adding probability
 * controls and algorithmic generation capabilities.
 */

#pragma once

#include "Config.h"
#include "Types.h"
#include "Bitfield.h"

class StochasticSequence {
public:
    //----------------------------------------
    // Layer Types - What can be edited per step
    //----------------------------------------

    enum class Layer : uint8_t {
        Gate,                    // Basic gate on/off
        GateProbability,         // Probability gate will trigger (0-100%)
        GateOffset,              // Timing offset for gate
        Retrigger,               // Number of retriggering events
        RetriggerProbability,    // Probability of retrigger occurring
        Length,                  // Note/gate length
        LengthVariationRange,    // How much length can vary
        LengthVariationProbability, // Probability of length variation
        Note,                    // Note value
        NoteVariationRange,      // How much note can vary
        NoteVariationProbability,// Probability of note variation
        Octave,                  // Octave shift per step
        Condition,               // Conditional playback logic
        Slide,                   // Slide/glide to next note
        NoteOctaveProbability,   // Combined note/octave probability control
        Last
    };

    //----------------------------------------
    // Step Data Structure
    //----------------------------------------

    class Step {
    public:
        /**
         * EFFICIENT STORAGE:
         * Uses bitfields to pack multiple parameters into 64 bits total
         * This is critical for embedded systems with limited RAM
         *
         * _data0: First 32 bits
         * _data1: Second 32 bits
         */
        union {
            uint32_t _data0;
            struct {
                uint32_t gate : 1;              // Gate on/off (1 bit)
                uint32_t gateProbability : 7;   // 0-100% (7 bits = 0-127)
                uint32_t gateOffset : 4;        // Timing offset (4 bits = 0-15)
                uint32_t retrigger : 4;         // Retrigger count (4 bits)
                uint32_t retriggerProbability : 7; // Retrigger probability
                uint32_t length : 4;            // Note length
                uint32_t lengthVariationRange : 4;    // Length variation amount
                uint32_t lengthVariationProbability : 1; // Simplified probability
            };
        };

        union {
            uint32_t _data1;
            struct {
                uint32_t note : 7;                     // Note value (0-127)
                uint32_t noteVariationRange : 5;       // Note variation range
                uint32_t noteVariationProbability : 7; // Note variation probability
                uint32_t octave : 4;                   // Octave shift per step
                uint32_t condition : 4;                // Conditional logic
                uint32_t slide : 1;                    // Slide/glide flag
                uint32_t bypassScale : 1;              // Ignore scale quantization
            };
        };

        // Gate probability getter/setter with range checking
        int gateProbability() const {
            return gateProbability;
        }
        void setGateProbability(int probability) {
            gateProbability = clamp(probability, 0, 100);
        }

        // Note variation controls
        int noteVariationRange() const { return noteVariationRange; }
        void setNoteVariationRange(int range) {
            noteVariationRange = clamp(range, 0, 24); // +/- 2 octaves
        }

        int noteVariationProbability() const { return noteVariationProbability; }
        void setNoteVariationProbability(int probability) {
            noteVariationProbability = clamp(probability, 0, 100);
        }

        // Octave per step (different from global octave)
        int octave() const { return octave; }
        void setOctave(int oct) {
            octave = clamp(oct, 0, 10);
        }

        /**
         * CONDITIONAL PLAYBACK:
         * Conditions determine when a step actually plays
         * Examples: "play on 1st loop only", "skip every other time", etc.
         */
        int condition() const { return condition; }
        void setCondition(int cond) {
            condition = clamp(cond, 0, 15);
        }

        void clear() {
            _data0 = 0;
            _data1 = 0;
        }
    };

    //----------------------------------------
    // Sequence-Level Properties
    //----------------------------------------

    /**
     * REST PROBABILITY - Key Stochastic Feature
     *
     * Allows setting probability of rests at different intervals:
     * - 2 steps: every other step might be a rest
     * - 4 steps: every 4th step might be a rest
     * - 8 steps: every 8th step might be a rest
     * - 15 steps: longer interval patterns
     *
     * This creates rhythmic variation and prevents static patterns
     */
    int restProbability2() const { return _restProbability2; }
    void setRestProbability2(int probability) {
        _restProbability2 = clamp(probability, 0, 100);
    }

    int restProbability4() const { return _restProbability4; }
    void setRestProbability4(int probability) {
        _restProbability4 = clamp(probability, 0, 100);
    }

    int restProbability8() const { return _restProbability8; }
    void setRestProbability8(int probability) {
        _restProbability8 = clamp(probability, 0, 100);
    }

    int restProbability15() const { return _restProbability15; }
    void setRestProbability15(int probability) {
        _restProbability15 = clamp(probability, 0, 100);
    }

    /**
     * LOOP FUNCTIONALITY WITH RESEED
     *
     * Loop controls allow defining a repeating section within the sequence.
     * Reseed capability lets you get reproducible "random" behavior:
     * - Same seed = same "random" pattern every time
     * - Different seed = different variation
     *
     * This is crucial for live performance where you want controlled chaos
     */
    int loopFirstStep() const { return _loopFirstStep; }
    void setLoopFirstStep(int step) {
        _loopFirstStep = clamp(step, 0, CONFIG_STEP_COUNT - 1);
    }

    int loopLastStep() const { return _loopLastStep; }
    void setLoopLastStep(int step) {
        _loopLastStep = clamp(step, 0, CONFIG_STEP_COUNT - 1);
    }

    /**
     * OCTAVE RANGE CONTROL
     *
     * Constrains random note generation to a specific octave range
     * Prevents sequences from wandering too far from musical center
     */
    int octaveRangeLow() const { return _octaveRangeLow; }
    void setOctaveRangeLow(int octave) {
        _octaveRangeLow = clamp(octave, -5, 5);
    }

    int octaveRangeHigh() const { return _octaveRangeHigh; }
    void setOctaveRangeHigh(int octave) {
        _octaveRangeHigh = clamp(octave, -5, 5);
    }

    // Array of steps
    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;
    const StepArray &steps() const { return _steps; }
    StepArray &steps() { return _steps; }

private:
    // Sequence identification
    int8_t _slot = 0;
    char _name[NameLength + 1];

    // Musical parameters
    Routable<uint8_t> _scale;
    Routable<uint8_t> _rootNote;
    Routable<uint16_t> _divisor;

    // Playback control
    uint8_t _resetMeasure;
    Routable<uint8_t> _runMode;
    uint8_t _firstStep;
    uint8_t _lastStep;

    // STOCHASTIC-SPECIFIC: Rest probabilities at different intervals
    int8_t _restProbability2;
    int8_t _restProbability4;
    int8_t _restProbability8;
    int8_t _restProbability15;

    // STOCHASTIC-SPECIFIC: Loop controls
    int8_t _loopFirstStep;
    int8_t _loopLastStep;

    // STOCHASTIC-SPECIFIC: Octave range constraints
    int8_t _octaveRangeLow;
    int8_t _octaveRangeHigh;

    // STOCHASTIC-SPECIFIC: Length modifier
    int8_t _lengthModifier;

    // Step data
    StepArray _steps;
};

/**
 * COMPARISON WITH NOTE SEQUENCE:
 *
 * NoteSequence:
 * - Fixed step sequencing
 * - Steps always play in order
 * - Variations are simple (note variation, gate probability)
 * - Predictable playback
 *
 * StochasticSequence:
 * - Algorithmic/generative sequencing
 * - Steps may or may not play based on multiple probability factors
 * - Rest probabilities at multiple intervals
 * - Loop with reseed for controlled randomness
 * - Octave range constraints
 * - Much more unpredictable, evolving behavior
 *
 * MEMORY IMPACT:
 * - Additional data per step (octave, more probability fields)
 * - Additional sequence-level data (rest probabilities, loop controls)
 * - Increases RAM usage per sequence
 * - Important consideration for 192KB STM32 limitation
 */
