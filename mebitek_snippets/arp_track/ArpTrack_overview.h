/**
 * MEBITEK FORK - Arpeggiator Track Overview
 *
 * Introduced in v0.3.0, this is a DEDICATED track type for arpeggiator functionality.
 * This is different from the MIDI/CV track's arpeggiator support.
 *
 * KEY DIFFERENCES:
 *
 * MIDI/CV Track Arpeggiator:
 * - Integrated into MIDI/CV track as a feature
 * - Processes incoming MIDI notes
 * - Limited to MIDI/CV track functionality
 *
 * ArpTrack (Dedicated Arpeggiator Track):
 * - Full track type devoted to arpeggiator
 * - Independent sequencing capabilities
 * - More complex pattern management
 * - Better integration with sequencer features
 * - Dedicated UI and controls
 * - Can have up to 8 arp tracks simultaneously (one per track slot)
 *
 * FEATURES:
 * - Pattern copy/paste functionality
 * - Full Launchpad integration
 * - Clipboard support
 * - Routing system integration
 * - Independent per-track arpeggiator
 */

#pragma once

#include "Config.h"
#include "Types.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Routing.h"
#include "ArpSequence.h"
#include "Arpeggiator.h"

class ArpTrack : public BaseTrack, public BaseTrackPatternFollow {
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
    // ARPEGGIATOR-SPECIFIC PROPERTIES
    //----------------------------------------

    /**
     * EMBEDDED ARPEGGIATOR INSTANCE
     *
     * Unlike MIDI/CV track which shares arpeggiator logic,
     * ArpTrack has its own dedicated Arpeggiator instance.
     * This allows for more complex arpeggio patterns and
     * independent operation from MIDI input.
     */
    const Arpeggiator &arpeggiator() const { return _arpeggiator; }
    Arpeggiator &arpeggiator() { return _arpeggiator; }

    /**
     * MIDI KEYBOARD INTEGRATION
     *
     * Controls whether MIDI keyboard input affects this arp track.
     * When enabled, incoming MIDI notes feed the arpeggiator.
     * When disabled, arpeggiator operates independently.
     */
    bool midiKeyboard() const { return _midiKeyboard; }
    void setMidiKeyboard(bool enabled) {
        _midiKeyboard = enabled;
    }

    //----------------------------------------
    // Track Parameters (Similar to other tracks)
    //----------------------------------------

    // Slide time for portamento/glide between notes
    int slideTime() const { return _slideTime.get(isRouted(Routing::Target::SlideTime)); }
    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

    // Octave transposition (-10 to +10 octaves)
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
     * PROBABILITY BIASES
     *
     * These affect the likelihood of variations occurring.
     * Higher values = higher probability of variation.
     * -1 value typically means "disabled" or "use default"
     */
    int gateProbabilityBias() const { return _gateProbabilityBias; }
    void setGateProbabilityBias(int bias) {
        _gateProbabilityBias = clamp(bias, -1, ArpSequence::GateProbability::Max);
    }

    int retriggerProbabilityBias() const { return _retriggerProbabilityBias; }
    void setRetriggerProbabilityBias(int bias) {
        _retriggerProbabilityBias = clamp(bias, -1, ArpSequence::RetriggerProbability::Max);
    }

    int lengthBias() const { return _lengthBias; }
    void setLengthBias(int bias) {
        _lengthBias = clamp(bias, -1, ArpSequence::Length::Max);
    }

    int noteProbabilityBias() const { return _noteProbabilityBias; }
    void setNoteProbabilityBias(int bias) {
        _noteProbabilityBias = clamp(bias, -1, ArpSequence::NoteProbability::Max);
    }

    //----------------------------------------
    // Sequence Management
    //----------------------------------------

    /**
     * PATTERN MANAGEMENT
     *
     * ArpTrack maintains an array of ArpSequence objects,
     * one for each pattern/snapshot. This allows:
     * - Storing different arp patterns per sequence
     * - Pattern switching during performance
     * - Copy/paste between sequences
     */
    const ArpSequence &sequence(int index) const {
        return _sequences[index];
    }
    ArpSequence &sequence(int index) {
        return _sequences[index];
    }

    const ArpSequenceArray &sequences() const { return _sequences; }
    ArpSequenceArray &sequences() { return _sequences; }

    //----------------------------------------
    // Play Mode
    //----------------------------------------

    /**
     * PLAY MODES
     *
     * Typical play modes include:
     * - Free: Play independently of global clock alignment
     * - Aligned: Sync to bar/measure boundaries
     */
    PlayMode playMode() const {
        return PlayMode(_playMode.get(isRouted(Routing::Target::PlayMode)));
    }
    void setPlayMode(PlayMode mode, bool routed = false) {
        _playMode.set(uint8_t(mode), routed);
    }

private:
    // Routable parameters (can be controlled by CV/routing)
    Routable<uint8_t> _playMode;
    Routable<uint8_t> _fillMode;
    Routable<uint8_t> _cvUpdateMode;
    Routable<uint8_t> _slideTime;
    Routable<int8_t> _octave;
    Routable<int8_t> _transpose;
    Routable<int8_t> _rotate;

    // ARPEGGIATOR-SPECIFIC: Dedicated arpeggiator engine
    Arpeggiator _arpeggiator;

    // ARPEGGIATOR-SPECIFIC: MIDI keyboard integration toggle
    bool _midiKeyboard;

    // Probability biases
    int8_t _gateProbabilityBias;
    int8_t _retriggerProbabilityBias;
    int8_t _lengthBias;
    int8_t _noteProbabilityBias;

    // Sequence array
    ArpSequenceArray _sequences;
};

/**
 * USE CASES FOR DEDICATED ARP TRACK:
 *
 * 1. COMPLEX ARPEGGIO PATTERNS:
 *    - Create intricate arpeggio sequences with full sequencer power
 *    - Store multiple arp patterns and switch between them
 *    - Use probability and variation for evolving arpeggios
 *
 * 2. MULTI-OCTAVE ARPEGGIOS:
 *    - Span multiple octaves with complex note patterns
 *    - Combine with octave parameter for wide range
 *
 * 3. RHYTHMIC ARPEGGIO VARIATIONS:
 *    - Use gate probability for rhythmic variation
 *    - Retrigger for stuttering effects
 *    - Length variation for dynamic note durations
 *
 * 4. INDEPENDENT ARPEGGIATION:
 *    - Each track can have its own arpeggiator pattern
 *    - Up to 8 simultaneous independent arpeggios
 *    - Different timing, octave, and note patterns per track
 *
 * MEMORY CONSIDERATIONS:
 * - Each ArpTrack adds significant RAM usage
 * - Contains full sequence array + arpeggiator instance
 * - On STM32 with 192KB RAM, this limits number of simultaneous arp tracks
 * - Consider using MIDI/CV arpeggiator if only need basic functionality
 */
