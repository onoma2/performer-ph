/**
 * MEBITEK FORK - Logic Sequence Operators
 *
 * This file demonstrates the per-step logic operators available in Logic Track.
 * Each step can have its own gate logic and note logic operation.
 */

#pragma once

#include "Config.h"
#include "Types.h"
#include "Bitfield.h"

class LogicSequence {
public:
    //----------------------------------------
    // Layer Types
    //----------------------------------------

    enum class Layer : uint8_t {
        Gate,                    // Gate on/off
        GateProbability,         // Probability of gate
        GateOffset,              // Gate timing offset
        GateLogic,               // LOGIC-SPECIFIC: Gate logic operator
        Retrigger,               // Retrigger count
        RetriggerProbability,    // Retrigger probability
        Length,                  // Note/gate length
        LengthVariationRange,    // Length variation
        LengthVariationProbability,
        Note,                    // Note value
        NoteLogic,               // LOGIC-SPECIFIC: Note logic operator
        NoteVariationRange,      // Note variation range
        NoteVariationProbability,
        Condition,               // Conditional playback
        Slide,                   // Slide/glide
        NoteVariationProbabilityAndBypassScale, // Combined control
        RepeatMode,              // Step repeat behavior
        Last
    };

    //----------------------------------------
    // LOGIC OPERATORS
    //----------------------------------------

    /**
     * GATE LOGIC OPERATORS
     *
     * These operators determine how input gates are combined:
     *
     * PASS - Pass through input A unchanged
     * INVERT - Invert input A (NOT operation)
     * AND - Output high only if both A AND B are high
     * OR - Output high if either A OR B is high
     * XOR - Output high if A or B is high, but NOT both (exclusive or)
     * NAND - NOT AND (opposite of AND)
     * NOR - NOT OR (opposite of OR)
     */
    enum class GateLogic : uint8_t {
        PASS = 0,     // A
        INVERT,       // NOT A
        AND,          // A AND B
        OR,           // A OR B
        XOR,          // A XOR B
        NAND,         // NOT (A AND B)
        NOR,          // NOT (A OR B)
        Last
    };

    /**
     * NOTE LOGIC OPERATORS
     *
     * These operators determine how input notes are processed:
     *
     * FILTER - Only pass notes that meet certain criteria
     * TRANSFORM - Modify note values based on logic
     * COMBINE - Merge notes from multiple inputs
     * MASK - Block certain notes based on conditions
     */
    enum class NoteLogic : uint8_t {
        PASS = 0,        // Pass note from input A
        TRANSPOSE_A,     // Transpose note from A
        TRANSPOSE_B,     // Transpose note from B
        COMBINE,         // Combine notes from A and B
        FILTER_HIGH,     // Only pass high notes
        FILTER_LOW,      // Only pass low notes
        FILTER_RANGE,    // Only pass notes in range
        MASK,            // Block notes based on input B
        Last
    };

    //----------------------------------------
    // Step Structure with Logic Operators
    //----------------------------------------

    class Step {
    public:
        /**
         * PACKED BIT STORAGE
         *
         * Efficiently stores step data including logic operators
         */
        union {
            uint32_t _data0;
            struct {
                uint32_t gate : 1;
                uint32_t gateProbability : 7;
                uint32_t gateOffset : 4;
                uint32_t gateLogic : 3;         // LOGIC-SPECIFIC: 8 possible operators
                uint32_t retrigger : 4;
                uint32_t retriggerProbability : 7;
                uint32_t length : 4;
                uint32_t lengthVariationRange : 2;
            };
        };

        union {
            uint32_t _data1;
            struct {
                uint32_t note : 7;
                uint32_t noteLogic : 3;          // LOGIC-SPECIFIC: Note logic operator
                uint32_t noteVariationRange : 5;
                uint32_t noteVariationProbability : 7;
                uint32_t condition : 4;
                uint32_t slide : 1;
                uint32_t bypassScale : 1;
                uint32_t repeatMode : 2;
                uint32_t lengthVariationProbability : 2;
            };
        };

        //----------------------------------------
        // Gate Logic Operations
        //----------------------------------------

        GateLogic gateLogic() const {
            return GateLogic(gateLogic);
        }

        void setGateLogic(GateLogic logic) {
            gateLogic = uint8_t(logic);
        }

        /**
         * EVALUATE GATE LOGIC
         *
         * Takes two input gates (from tracks A and B)
         * Applies the logic operator
         * Returns the resulting gate state
         */
        bool evaluateGateLogic(bool inputA, bool inputB) const {
            switch (gateLogic()) {
                case GateLogic::PASS:
                    return inputA;
                case GateLogic::INVERT:
                    return !inputA;
                case GateLogic::AND:
                    return inputA && inputB;
                case GateLogic::OR:
                    return inputA || inputB;
                case GateLogic::XOR:
                    return inputA != inputB;  // True if different
                case GateLogic::NAND:
                    return !(inputA && inputB);
                case GateLogic::NOR:
                    return !(inputA || inputB);
                default:
                    return inputA;
            }
        }

        //----------------------------------------
        // Note Logic Operations
        //----------------------------------------

        NoteLogic noteLogic() const {
            return NoteLogic(noteLogic);
        }

        void setNoteLogic(NoteLogic logic) {
            noteLogic = uint8_t(logic);
        }

        /**
         * EVALUATE NOTE LOGIC
         *
         * Takes two input notes and applies note logic
         * Returns the resulting note value
         */
        int evaluateNoteLogic(int noteA, int noteB, int transposeA = 0, int transposeB = 0) const {
            switch (noteLogic()) {
                case NoteLogic::PASS:
                    return noteA;
                case NoteLogic::TRANSPOSE_A:
                    return clamp(noteA + transposeA, 0, 127);
                case NoteLogic::TRANSPOSE_B:
                    return clamp(noteB + transposeB, 0, 127);
                case NoteLogic::COMBINE:
                    // Average of both notes
                    return (noteA + noteB) / 2;
                case NoteLogic::FILTER_HIGH:
                    // Only pass if note is above threshold (noteB)
                    return (noteA > noteB) ? noteA : 0;
                case NoteLogic::FILTER_LOW:
                    // Only pass if note is below threshold (noteB)
                    return (noteA < noteB) ? noteA : 0;
                case NoteLogic::FILTER_RANGE:
                    // Only pass if note is within range
                    {
                        int low = std::min(noteA, noteB);
                        int high = std::max(noteA, noteB);
                        return (note() >= low && note() <= high) ? note() : 0;
                    }
                case NoteLogic::MASK:
                    // Block note if input B is active
                    return (noteB > 0) ? 0 : noteA;
                default:
                    return noteA;
            }
        }

        void clear() {
            _data0 = 0;
            _data1 = 0;
        }
    };

    //----------------------------------------
    // Sequence-Level Logic Parameters
    //----------------------------------------

    /**
     * TRIGGER CURVE SHAPES (v0.2.2)
     *
     * Defines how curve responds to logic conditions
     * Allows curve tracks to be controlled by logic results
     */
    enum class TriggerCurveShape : uint8_t {
        Rise,       // Rising edge trigger
        Fall,       // Falling edge trigger
        Both,       // Both edges
        Gate,       // Gate high duration
        Last
    };

    TriggerCurveShape triggerCurveShape() const {
        return _triggerCurveShape;
    }

    void setTriggerCurveShape(TriggerCurveShape shape) {
        _triggerCurveShape = shape;
    }

    /**
     * NOTE FILTER PARAMETER (v0.2.2)
     *
     * Global note filtering in addition to per-step logic
     * Can filter based on:
     * - Note range
     * - Scale membership
     * - Interval relationships
     */
    int noteFilterMin() const { return _noteFilterMin; }
    void setNoteFilterMin(int min) {
        _noteFilterMin = clamp(min, 0, 127);
    }

    int noteFilterMax() const { return _noteFilterMax; }
    void setNoteFilterMax(int max) {
        _noteFilterMax = clamp(max, 0, 127);
    }

    // Array of steps with logic operators
    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;
    const StepArray &steps() const { return _steps; }
    StepArray &steps() { return _steps; }

private:
    // Standard sequence properties
    int8_t _slot = 0;
    char _name[NameLength + 1];
    Routable<uint8_t> _scale;
    Routable<uint8_t> _rootNote;
    Routable<uint16_t> _divisor;
    uint8_t _resetMeasure;
    Routable<uint8_t> _runMode;
    uint8_t _firstStep;
    uint8_t _lastStep;

    // LOGIC-SPECIFIC: Trigger curve behavior
    TriggerCurveShape _triggerCurveShape;

    // LOGIC-SPECIFIC: Note filtering
    int8_t _noteFilterMin;
    int8_t _noteFilterMax;

    // Step array with logic operators
    StepArray _steps;
};

/**
 * PRACTICAL EXAMPLES:
 *
 * 1. EUCLIDEAN RHYTHM COMBINATION:
 *    Track A: [1,0,0,1,0,0,1,0]  (3 hits in 8 steps)
 *    Track B: [1,0,1,0,1,0,1,0]  (4 hits in 8 steps)
 *    Logic: AND
 *    Result:  [1,0,0,0,0,0,1,0]  (Complex polyrhythm)
 *
 * 2. GATE INVERSION FOR COUNTER-RHYTHM:
 *    Track A: [1,0,1,0,1,0,1,0]  (Main beat)
 *    Logic: INVERT
 *    Result:  [0,1,0,1,0,1,0,1]  (Off-beats)
 *
 * 3. CONDITIONAL MELODY:
 *    Track A: Melody notes [C, D, E, F, G, A, B, C]
 *    Track B: Gate pattern [1,0,1,0,0,1,0,1]
 *    Note Logic: FILTER (pass only when B is high)
 *    Result: Melody only plays on steps where B has gates
 *
 * 4. NOTE RANGE FILTERING:
 *    Track A: Wide-ranging melody
 *    Note Filter: Min=60 (Middle C), Max=72 (High C)
 *    Result: Only notes within one octave range pass through
 *
 * PERFORMANCE NOTES:
 * - Logic evaluation happens every step
 * - Simple operators (AND, OR) are very fast
 * - Complex note transformations may take more CPU
 * - Consider impact when using multiple logic tracks
 * - Real-time requirements: must complete within step timing
 */
