/**
 * MEBITEK FORK - Curve Track CV Control Enhancements
 *
 * Key enhancement in v0.2.2: CV-controllable minimum and maximum values
 *
 * STANDARD CURVE TRACK (Original/PEW|FORMER):
 * - Min/max values are set manually per sequence
 * - Static range throughout pattern playback
 * - Offset parameter allows global voltage shift
 *
 * MEBITEK CURVE TRACK ENHANCEMENTS:
 * - Min and max can be modulated by external CV
 * - Dynamic range control during playback
 * - Enables complex modulation scenarios
 * - More expressive live performance capabilities
 *
 * USE CASES:
 * - LFO depth control via CV (modulate max value)
 * - Dynamic envelope range (modulate min and max together)
 * - Crossfading between curve ranges
 * - Performance control of modulation intensity
 */

#pragma once

#include "Config.h"
#include "Routing.h"
#include "ModelUtils.h"

// Snippet showing CV-controllable min/max implementation
class CurveTrack_CVControl {
public:
    //----------------------------------------
    // CV-CONTROLLABLE MIN/MAX (MEBITEK ENHANCEMENT)
    //----------------------------------------

    /**
     * MINIMUM VALUE WITH CV ROUTING
     *
     * The minimum value of the curve can be controlled by:
     * 1. Local setting (manual adjustment)
     * 2. External CV routing (dynamic modulation)
     *
     * Range: Typically -5V to +5V (depends on hardware calibration)
     *
     * Example routing:
     * - Route CV Input 1 to CurveMin on Track 3
     * - As CV Input 1 varies, the minimum value of curves on Track 3 changes
     * - This affects the bottom of the curve's range
     */
    float min() const {
        // Check if this parameter is being controlled by routing
        return _min.get(isRouted(Routing::Target::CurveMin));
    }

    void setMin(float min, bool routed = false) {
        // Clamp to valid voltage range
        _min.set(clamp(min, -5.0f, 5.0f), routed);

        // IMPORTANT: Maintain constraint that max >= min
        // If min is increased above current max, adjust max upward
        if (_min.get() > _max.get()) {
            _max.set(_min.get(), false);
        }
    }

    /**
     * MAXIMUM VALUE WITH CV ROUTING
     *
     * Similar to minimum, but controls the top of the curve range.
     *
     * Range: Typically -5V to +5V
     *
     * Example routing:
     * - Route CV Input 2 to CurveMax on Track 4
     * - Use an expression pedal on CV Input 2
     * - Control modulation depth in real-time during performance
     */
    float max() const {
        return _max.get(isRouted(Routing::Target::CurveMax));
    }

    void setMax(float max, bool routed = false) {
        // Clamp to valid voltage range
        _max.set(clamp(max, -5.0f, 5.0f), routed);

        // IMPORTANT: Maintain constraint that max >= min
        // If max is decreased below current min, adjust min downward
        if (_max.get() < _min.get()) {
            _min.set(_max.get(), false);
        }
    }

    //----------------------------------------
    // OFFSET PARAMETER (Original Feature)
    //----------------------------------------

    /**
     * OFFSET - Global voltage shift
     *
     * This was in the original firmware but works well with CV-controllable min/max.
     * Offset is applied AFTER curve calculation, so it shifts the entire range.
     *
     * Offset range: -5V to +5V (routable)
     *
     * Interaction with min/max:
     * - Curve is calculated within [min, max] range
     * - Offset is then added to the result
     * - Final output = curve_value + offset
     */
    float offset() const {
        return _offset.get(isRouted(Routing::Target::Offset));
    }

    void setOffset(float offset, bool routed = false) {
        _offset.set(clamp(offset, -5.0f, 5.0f), routed);
    }

    //----------------------------------------
    // SLIDE TIME (Original Feature, CV-controllable)
    //----------------------------------------

    /**
     * SLIDE TIME - Portamento/glide between curve values
     *
     * Controls interpolation speed between curve steps.
     * 0% = instant jumps
     * 100% = smooth glide
     *
     * Range: 0-100% (routable)
     */
    int slideTime() const {
        return _slideTime.get(isRouted(Routing::Target::SlideTime));
    }

    void setSlideTime(int slideTime, bool routed = false) {
        _slideTime.set(clamp(slideTime, 0, 100), routed);
    }

private:
    // Routable parameters - can be controlled locally or by routing
    Routable<float> _min;      // MEBITEK: CV-controllable minimum
    Routable<float> _max;      // MEBITEK: CV-controllable maximum
    Routable<float> _offset;   // Original: Global voltage offset
    Routable<int> _slideTime;  // Original: Portamento/glide amount

    /**
     * HELPER: Check if parameter is under routing control
     */
    bool isRouted(Routing::Target target) const {
        // Implementation would check routing configuration
        // Returns true if this target is actively routed
        return false; // Placeholder
    }
};

/**
 * PRACTICAL EXAMPLES:
 *
 * 1. DYNAMIC LFO DEPTH CONTROL:
 *    - Curve sequence generates LFO pattern
 *    - Min = 0V (fixed)
 *    - Max routed to CV Input 1 (0-5V)
 *    - Result: CV Input controls LFO depth from 0% to 100%
 *    - Use case: Expression pedal controls filter modulation depth
 *
 * 2. CROSSFADING MODULATION RANGES:
 *    - Two curve tracks with different min/max ranges
 *    - Route same CV to both tracks' min/max
 *    - Track A: As CV increases, range expands upward
 *    - Track B: As CV increases, range contracts
 *    - Result: Smooth transition between modulation characters
 *
 * 3. ENVELOPE FOLLOWER CONTROL:
 *    - Convert audio to CV (external envelope follower)
 *    - Route to curve track max
 *    - Curve track generates rhythmic modulation
 *    - Result: Modulation depth follows audio dynamics
 *
 * 4. PERFORMANCE CONTROL:
 *    - Route MIDI CC (via routing system) to curve max
 *    - Keyboard mod wheel controls modulation intensity
 *    - Hands-on control during live performance
 *
 * 5. MULTI-CURVE RECORDING (v0.2.0):
 *    - Record CV input movements as curve sequences
 *    - Captured min/max movements become automation
 *    - Playback with or without continued CV modulation
 *
 * DIFFERENCES FROM ORIGINAL FIRMWARE:
 *
 * Original (westlicht/PEW|FORMER):
 * - Min/max are sequence parameters (set per sequence)
 * - Static throughout playback
 * - Offset provides only global shift
 * - No real-time range modulation
 *
 * Mebitek Enhancement:
 * - Min/max are track parameters (applies to all sequences)
 * - Can be modulated by CV in real-time
 * - Enables dynamic range control
 * - More expressive performance capabilities
 * - Better integration with modular setups
 *
 * IMPLEMENTATION CONSIDERATIONS:
 *
 * 1. PARAMETER CONSTRAINTS:
 *    - Must always maintain max >= min
 *    - Automatic adjustment when constraint violated
 *    - Prevents invalid curve ranges
 *
 * 2. ROUTING SYSTEM:
 *    - Uses same routing infrastructure as other parameters
 *    - Can route from CV inputs, other tracks, MIDI, etc.
 *    - Routing takes precedence over local value
 *
 * 3. CPU IMPACT:
 *    - CV routing evaluated per sample or per step
 *    - Minimal overhead for simple routing
 *    - Consider when using many routed parameters
 *
 * 4. MEMORY:
 *    - Routable<T> template adds routing state
 *    - Small per-parameter overhead
 *    - Acceptable for enhanced functionality
 *
 * 5. USER INTERFACE:
 *    - UI must indicate when parameter is routed
 *    - Show both local and routed values
 *    - Allow temporary override of routing
 */
