/**
 * MEBITEK FORK - Launchpad Follow Mode
 *
 * Enhanced in v0.3.1 (May 2024)
 *
 * Follow Mode makes the Launchpad display follow the main unit's current
 * pattern and page context, reducing confusion during performance.
 *
 * FEATURES:
 * - Launchpad follows pattern changes on main unit
 * - Follows page navigation (sequence edit, performer, etc.)
 * - Follow state persists in project files (v0.3.1)
 * - Prevents display desynchronization
 * - Improves workflow continuity
 */

#pragma once

#include "Config.h"

class LaunchpadFollowMode {
public:
    //----------------------------------------
    // FOLLOW MODE BEHAVIOR
    //----------------------------------------

    /**
     * PATTERN FOLLOW
     *
     * When pattern is changed on main unit (via encoder/buttons):
     * - Launchpad automatically updates to show new pattern
     * - Grid display reflects current pattern context
     * - LED colors update to show active pattern
     *
     * Without Follow Mode:
     * - Launchpad may show different pattern than main unit
     * - User must manually sync Launchpad view
     * - Confusion during performance
     *
     * With Follow Mode:
     * - Launchpad always shows current pattern
     * - Single source of truth
     * - Reduced cognitive load
     */
    bool patternFollow() const { return _patternFollow; }
    void setPatternFollow(bool enabled) {
        _patternFollow = enabled;
    }

    /**
     * PAGE FOLLOW
     *
     * When page is changed on main unit:
     * - Launchpad mode switches to match
     * - Example: Main unit shows Performer page → Launchpad shows Performance mode
     * - Example: Main unit shows Sequence Edit → Launchpad shows Edit grid
     *
     * Pages that follow:
     * - Sequence Edit → Launchpad Edit Grid
     * - Performer → Launchpad Performance Mode
     * - Pattern Select → Launchpad Pattern Mode
     * - Track Setup → Launchpad shows track indicators
     */
    bool pageFollow() const { return _pageFollow; }
    void setPageFollow(bool enabled) {
        _pageFollow = enabled;
    }

    /**
     * TRACK FOLLOW
     *
     * When track is selected on main unit:
     * - Launchpad highlights corresponding track
     * - Track controls (mute/solo/fill) update
     * - Sequence edit shows selected track's sequence
     */
    bool trackFollow() const { return _trackFollow; }
    void setTrackFollow(bool enabled) {
        _trackFollow = enabled;
    }

    //----------------------------------------
    // PERSISTENT FOLLOW (v0.3.1)
    //----------------------------------------

    /**
     * FOLLOW MODE PERSISTENCE
     *
     * Prior to v0.3.1:
     * - Follow mode settings reset on project load
     * - Had to re-enable follow mode each session
     * - Inconsistent user experience
     *
     * From v0.3.1 onwards:
     * - Follow mode state saved in project file
     * - Each project remembers its follow settings
     * - Restores on project load
     * - Per-project customization
     */

    // Save follow state to project
    void write(VersionedSerializedWriter &writer) const {
        writer.write(_patternFollow);
        writer.write(_pageFollow);
        writer.write(_trackFollow);
    }

    // Load follow state from project
    void read(VersionedSerializedReader &reader) {
        reader.read(_patternFollow);
        reader.read(_pageFollow);
        reader.read(_trackFollow);
    }

    //----------------------------------------
    // FOLLOW MODE INDICATORS
    //----------------------------------------

    /**
     * VISUAL FEEDBACK
     *
     * Launchpad provides visual indication of follow mode:
     * - Dedicated LED shows follow mode status
     * - Flashing when actively following
     * - Different color when follow is disabled
     *
     * Helps user understand current mode state
     */
    enum class FollowIndicator : uint8_t {
        Off,              // Follow mode disabled
        PatternFollowing, // Following pattern changes
        PageFollowing,    // Following page changes
        BothFollowing,    // Following both
    };

private:
    bool _patternFollow;
    bool _pageFollow;
    bool _trackFollow;
};

/**
 * USAGE SCENARIOS:
 *
 * 1. PERFORMANCE WITH MAIN UNIT CONTROL:
 *    - Main performer uses encoder to switch patterns
 *    - Launchpad player sees pattern changes automatically
 *    - Both stay synchronized
 *    - No verbal communication needed
 *
 * 2. RECORDING WORKFLOW:
 *    - Navigate pages on main unit
 *    - Launchpad follows to relevant mode
 *    - Edit sequences on Launchpad
 *    - Switch to perform mode seamlessly
 *
 * 3. COLLABORATIVE PERFORMANCE:
 *    - One performer uses main unit
 *    - Another uses Launchpad
 *    - Follow mode keeps them synchronized
 *    - Reduces mistakes and confusion
 *
 * 4. TEACHING/DEMONSTRATION:
 *    - Instructor uses main unit
 *    - Students see changes on Launchpad
 *    - Visual learning aid
 *    - Everyone sees same context
 *
 * BENEFITS:
 *
 * 1. REDUCED COGNITIVE LOAD:
 *    - Don't need to remember which pattern is active
 *    - Don't need to manually sync Launchpad
 *    - Focus on performance, not housekeeping
 *
 * 2. FEWER MISTAKES:
 *    - Prevents editing wrong pattern
 *    - Prevents muting wrong track
 *    - Always see current context
 *
 * 3. WORKFLOW CONTINUITY:
 *    - Seamless transition between pages
 *    - Natural integration of Launchpad
 *    - Less interruption to creative flow
 *
 * 4. PROJECT-SPECIFIC SETTINGS:
 *    - Some projects need follow mode
 *    - Others work better without
 *    - Per-project persistence allows both
 *
 * TECHNICAL IMPLEMENTATION:
 *
 * 1. EVENT HANDLING:
 *    - Main unit generates change events
 *    - Launchpad controller listens for events
 *    - Updates display in response
 *    - Low latency for immediate feedback
 *
 * 2. STATE SYNCHRONIZATION:
 *    - Main unit is source of truth
 *    - Launchpad mirrors state
 *    - Unidirectional data flow
 *    - Prevents conflicts
 *
 * 3. PERSISTENCE:
 *    - Follow settings stored in project data
 *    - Serialized with other project settings
 *    - Loaded on project open
 *    - Version-compatible serialization
 *
 * DIFFERENCES FROM ORIGINAL:
 *
 * Original Firmware:
 * - Launchpad operates independently
 * - Manual synchronization required
 * - No follow mode concept
 * - Settings don't persist
 *
 * Mebitek Enhancement (v0.3.1):
 * - Automatic synchronization
 * - Multiple follow options (pattern/page/track)
 * - Persistent settings per project
 * - Visual feedback of follow state
 * - Fixed follow mode bugs
 *
 * CONFIGURATION:
 *
 * Typical Settings:
 *
 * - Performance Use:
 *   Pattern Follow: ON
 *   Page Follow: ON
 *   Track Follow: ON
 *   → Complete synchronization
 *
 * - Studio Use:
 *   Pattern Follow: OFF (manual control)
 *   Page Follow: ON (convenience)
 *   Track Follow: OFF (independent editing)
 *   → Selective synchronization
 *
 * - Teaching/Demo:
 *   Pattern Follow: ON
 *   Page Follow: ON
 *   Track Follow: ON
 *   → Maximum visibility
 *
 * BUG FIXES IN v0.3.1:
 *
 * - Follow mode now works correctly
 * - Follow pattern persists in projects
 * - Fixed desynchronization issues
 * - Improved responsiveness
 */
