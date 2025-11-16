/**
 * MEBITEK FORK - Launchpad Performance Mode
 *
 * Introduced in v0.2.0 (February 2024)
 *
 * Performance Mode provides an enhanced Launchpad interface optimized
 * for live performance, with quick access to mute, solo, pattern switching,
 * and fill controls.
 *
 * STANDARD LAUNCHPAD MODES:
 * - Pattern Mode: Select and switch patterns
 * - Sequence Edit Mode: Edit sequence steps directly on grid
 * - Note Edit Mode: Piano-roll style note entry
 *
 * MEBITEK ADDITIONS:
 * - Performance Mode: Live performance-oriented layout (v0.2.0)
 * - Follow Mode: Launchpad follows pattern/page changes (v0.3.1)
 * - Persistent Follow: Follow state saved in projects (v0.3.1)
 * - Enhanced Responsiveness: Better LaunchPad X support (v0.3.2)
 */

#pragma once

#include "Config.h"
#include "LaunchpadController.h"

/**
 * PERFORMANCE MODE LAYOUT CONCEPT
 *
 * Performance Mode reorganizes the Launchpad grid for live performance:
 *
 * ┌──────────────────────────────────────┐
 * │  PATTERN SELECTION    │  FUNCTIONS   │
 * │  [1][2][3][4][5][6][7][8]  │ [Func]   │
 * │  [9][10][11][12][13][14][15][16] │   │
 * │───────────────────────────────────│
 * │  TRACK CONTROLS                      │
 * │  [M1][M2][M3][M4][M5][M6][M7][M8]    │  M = Mute
 * │  [S1][S2][S3][S4][S5][S6][S7][S8]    │  S = Solo
 * │  [F1][F2][F3][F4][F5][F6][F7][F8]    │  F = Fill
 * │───────────────────────────────────│
 * │  TRANSPORT & GLOBAL                  │
 * │  [Play][Stop][Rec][<<][>>][All]...   │
 * └──────────────────────────────────────┘
 *
 * KEY FEATURES:
 * 1. Direct pattern selection (16 patterns on main grid)
 * 2. Per-track mute/solo/fill in dedicated rows
 * 3. Transport controls easily accessible
 * 4. Global mute/unmute functions
 * 5. Visual feedback with bi-color LEDs
 */

class LaunchpadPerformanceMode {
public:
    //----------------------------------------
    // PERFORMANCE MODE GRID LAYOUT
    //----------------------------------------

    /**
     * PATTERN SELECTION AREA
     *
     * Top two rows (16 pads) = 16 patterns
     * - Dim color: Pattern exists but not selected
     * - Bright color: Currently selected pattern
     * - Different colors for note vs curve patterns
     * - Flashing: Queued/scheduled pattern change
     *
     * Press pad: Immediate pattern change
     * Shift + Press: Schedule pattern change (sync to bar)
     */
    enum class PatternSelectMode : uint8_t {
        Immediate,    // Change pattern immediately
        Scheduled,    // Schedule change for next bar/measure
        Latched,      // Queue multiple patterns
    };

    /**
     * TRACK CONTROL ROWS
     *
     * Three dedicated rows for track control:
     *
     * Row 1 - MUTE:
     * [Track1][Track2]...[Track8]
     * - Off (dim): Track playing
     * - On (bright red): Track muted
     * - Press: Toggle mute immediately
     * - Shift+Press: Schedule mute for next bar
     *
     * Row 2 - SOLO:
     * [Track1][Track2]...[Track8]
     * - Off (dim): Normal playback
     * - On (bright yellow): Track soloed (all others muted)
     * - Press: Toggle solo
     * - Multiple tracks can be soloed simultaneously
     *
     * Row 3 - FILL:
     * [Track1][Track2]...[Track8]
     * - Off: Normal sequence
     * - On (bright green): Fill active
     * - Hold: Momentary fill
     * - Shift+Hold: Latched fill
     */

    struct TrackControl {
        bool mute : 1;           // Track muted
        bool solo : 1;           // Track soloed
        bool fill : 1;           // Fill active
        bool mutePending : 1;    // Scheduled mute change
        bool soloPending : 1;    // Scheduled solo change
        bool fillLatch : 1;      // Fill latched (held)
    };

    /**
     * TRANSPORT & GLOBAL CONTROLS
     *
     * Bottom row and side buttons:
     * - Play/Stop: Start/stop sequencer
     * - Record: Enable step/live recording
     * - << / >>: Navigate pages or patterns
     * - All Mute: Mute all tracks
     * - All Unmute: Unmute all tracks
     * - Cancel: Cancel scheduled actions
     */

    //----------------------------------------
    // VISUAL FEEDBACK
    //----------------------------------------

    /**
     * LED COLOR CODING
     *
     * Performance Mode uses colors for quick identification:
     *
     * PATTERNS:
     * - Yellow (dim): Note pattern available
     * - Red (dim): Curve pattern available
     * - Bright: Currently active pattern
     * - Flashing green: Scheduled/queued pattern
     *
     * TRACK CONTROLS:
     * - Red: Mute
     * - Yellow: Solo
     * - Green: Fill
     * - Orange: Multiple states active
     *
     * TRANSPORT:
     * - Green: Playing
     * - Red: Stopped
     * - Flashing red: Recording
     */
    enum class LEDColor : uint8_t {
        Off = 0,
        Red_Dim,
        Red_Bright,
        Yellow_Dim,
        Yellow_Bright,
        Green_Dim,
        Green_Bright,
        Orange_Dim,
        Orange_Bright,
    };

    //----------------------------------------
    // PERFORMANCE FEATURES
    //----------------------------------------

    /**
     * SCHEDULED ACTIONS (v0.2.0+)
     *
     * Shift+button schedules action for next measure:
     * - Pattern changes sync to bar
     * - Mute/unmute happens on downbeat
     * - Prevents timing glitches
     * - Maintains musical phrasing
     *
     * Press F5 (or cancel button) to cancel scheduled actions
     */
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

    /**
     * FILL AMOUNT CONTROL (v0.1.22+)
     *
     * New parameter controls "how much fill":
     * - 0%: No fill effect
     * - 50%: Partial fill
     * - 100%: Full fill variation
     *
     * Can be modulated via routing system
     * Allows subtle to extreme fill variations
     */
    int fillAmount() const { return _fillAmount; }
    void setFillAmount(int amount) {
        _fillAmount = clamp(amount, 0, 100);
    }

private:
    TrackControl _tracks[8];
    ScheduledAction _scheduledActions[16];
    int _fillAmount;
    PatternSelectMode _patternSelectMode;
};

/**
 * WORKFLOW EXAMPLES:
 *
 * 1. LIVE PATTERN SWITCHING:
 *    - Patterns laid out on top 2 rows
 *    - Quick visual reference of available patterns
 *    - Tap to switch immediately
 *    - Hold Shift + tap to schedule for next bar
 *    - No interruption in timing
 *
 * 2. TRACK MUTING FOR ARRANGEMENT:
 *    - Start with all 8 tracks playing
 *    - Mute tracks 5-8 for breakdown section
 *    - Solo track 3 for fill
 *    - Unmute all for drop
 *    - All changes can be scheduled to sync
 *
 * 3. FILL PERFORMANCE:
 *    - Hold fill button for momentary variation
 *    - Release to return to normal
 *    - Shift+press to latch fill
 *    - Adjust fill amount parameter for intensity
 *
 * 4. MULTI-TRACK SOLO:
 *    - Solo track 1 (drums)
 *    - Add solo track 4 (bass)
 *    - All other tracks muted
 *    - Quick isolation of elements
 *
 * ADVANTAGES OVER STANDARD MODE:
 *
 * Standard Launchpad Mode:
 * - Focus on step editing
 * - Pattern switching via menu
 * - Mute/solo through main unit
 * - Less immediate performance control
 *
 * Performance Mode:
 * - Optimized for live playing
 * - Direct access to all patterns
 * - All track controls on grid
 * - Visual feedback at a glance
 * - Faster workflow for performance
 * - Scheduled actions prevent timing issues
 *
 * INTEGRATION WITH OTHER FEATURES:
 *
 * 1. FOLLOW MODE (v0.3.1):
 *    - Launchpad display follows pattern changes
 *    - Follows page navigation on main unit
 *    - Always shows current context
 *    - Reduces confusion in performance
 *
 * 2. PERSISTENT FOLLOW (v0.3.1):
 *    - Follow mode state saved in project
 *    - Restores your preferred layout
 *    - Per-project customization
 *
 * 3. ENHANCED RESPONSIVENESS (v0.3.2):
 *    - Improved LaunchPad X support
 *    - Faster button response
 *    - Better LED updates
 *    - Smoother performance experience
 *
 * USE CASES:
 *
 * 1. ELECTRONIC MUSIC LIVE SETS:
 *    - Quick pattern switching
 *    - Track muting for build/breakdown
 *    - Fill variations for interest
 *    - All without looking at main screen
 *
 * 2. IMPROVISATION:
 *    - Experiment with track combinations
 *    - Try pattern variations quickly
 *    - Visual feedback helps decision-making
 *    - Undo via immediate re-press
 *
 * 3. JAMMING:
 *    - Multiple performers can control
 *    - Each person controls different tracks
 *    - Collaborative live sequencing
 *    - Visual coordination between players
 *
 * 4. SOUND DESIGN:
 *    - Quick A/B comparison of patterns
 *    - Isolate tracks for listening
 *    - Test fills and variations
 *    - Rapid iteration workflow
 */
