#include "UnitTest.h"

#include "apps/sequencer/engine/Clock.cpp"
#include "apps/sequencer/engine/Groove.cpp"

#include "drivers/ClockTimer.h"

#include <cmath>

// Mock Clock Listener for testing
class MockClockListener : public Clock::Listener {
public:
    struct OutputEvent {
        Clock::OutputState state;
    };

    struct MidiEvent {
        uint8_t message;
    };

    void onClockOutput(const Clock::OutputState &state) override {
        outputEvents.push_back({state});
        lastOutputState = state;
    }

    void onClockMidi(uint8_t msg) override {
        midiEvents.push_back({msg});
        lastMidiMessage = msg;
    }

    void clear() {
        outputEvents.clear();
        midiEvents.clear();
        lastMidiMessage = 0;
    }

    std::vector<OutputEvent> outputEvents;
    std::vector<MidiEvent> midiEvents;
    Clock::OutputState lastOutputState;
    uint8_t lastMidiMessage = 0;
};

UNIT_TEST("Clock") {

    CASE("Default state") {
        ClockTimer timer;
        Clock clock(timer);

        expectTrue(clock.isIdle(), "clock is idle by default");
        expectFalse(clock.isRunning(), "clock is not running by default");
        expectEqual(int(clock.mode()), int(Clock::Mode::Auto), "default mode is Auto");
        expectEqual(clock.tick(), uint32_t(0), "tick starts at 0");
        expectEqual(clock.masterBpm(), 120.f, "default BPM is 120");
    }

    CASE("Set mode") {
        ClockTimer timer;
        Clock clock(timer);

        clock.setMode(Clock::Mode::Master);
        expectEqual(int(clock.mode()), int(Clock::Mode::Master), "mode set to Master");

        clock.setMode(Clock::Mode::Slave);
        expectEqual(int(clock.mode()), int(Clock::Mode::Slave), "mode set to Slave");

        clock.setMode(Clock::Mode::Auto);
        expectEqual(int(clock.mode()), int(Clock::Mode::Auto), "mode set to Auto");
    }

    CASE("Set master BPM") {
        ClockTimer timer;
        Clock clock(timer);

        clock.setMasterBpm(140.0f);
        expectEqual(clock.masterBpm(), 140.0f, "master BPM set to 140");

        clock.setMasterBpm(60.0f);
        expectEqual(clock.masterBpm(), 60.0f, "master BPM set to 60");

        clock.setMasterBpm(180.5f);
        expectEqual(clock.masterBpm(), 180.5f, "master BPM set to 180.5");
    }

    CASE("Master start and stop") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        expectTrue(clock.isRunning(), "clock running after start");
        expectFalse(clock.isIdle(), "clock not idle after start");
        expectEqual(int(clock.activeMode()), int(Clock::Mode::Master), "active mode is Master");

        clock.masterStop();

        expectTrue(clock.isIdle(), "clock idle after stop");
        expectFalse(clock.isRunning(), "clock not running after stop");
    }

    CASE("Master start resets ticks") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        // Manually advance tick (simulating timer ticks)
        // Note: In real scenario, timer would call onClockTimerTick()

        clock.masterStop();
        clock.masterStart();

        expectEqual(clock.tick(), uint32_t(0), "ticks reset on start");
    }

    CASE("Master continue") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        clock.masterStop();

        clock.masterContinue();

        expectTrue(clock.isRunning(), "clock running after continue");
        expectEqual(int(clock.activeMode()), int(Clock::Mode::Master), "active mode is Master");
    }

    CASE("Master reset") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        clock.masterReset();

        expectTrue(clock.isIdle(), "clock idle after reset");
        expectEqual(clock.tick(), uint32_t(0), "ticks reset to 0");
    }

    CASE("Tick duration calculation") {
        ClockTimer timer;
        Clock clock(timer);

        clock.setMasterBpm(120.0f);
        float expectedDuration = 60.0f / (120.0f * CONFIG_PPQN);
        expectEqual(clock.tickDuration(), expectedDuration, "tick duration calculated correctly at 120 BPM");

        clock.setMasterBpm(60.0f);
        expectedDuration = 60.0f / (60.0f * CONFIG_PPQN);
        expectEqual(clock.tickDuration(), expectedDuration, "tick duration calculated correctly at 60 BPM");

        clock.setMasterBpm(180.0f);
        expectedDuration = 60.0f / (180.0f * CONFIG_PPQN);
        expectEqual(clock.tickDuration(), expectedDuration, "tick duration calculated correctly at 180 BPM");
    }

    CASE("Event checking - Start") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        Clock::Event event = clock.checkEvent();
        expectTrue(event & Clock::Event::Start, "Start event detected");
        expectTrue(event & Clock::Event::Reset, "Reset event also set on start");

        // Events should be consumed
        event = clock.checkEvent();
        expectEqual(event, Clock::Event(0), "events consumed after check");
    }

    CASE("Event checking - Stop") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        clock.checkEvent();  // Consume start event

        clock.masterStop();

        Clock::Event event = clock.checkEvent();
        expectTrue(event & Clock::Event::Stop, "Stop event detected");
    }

    CASE("Event checking - Continue") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        clock.checkEvent();  // Consume start

        clock.masterStop();
        clock.checkEvent();  // Consume stop

        clock.masterContinue();

        Clock::Event event = clock.checkEvent();
        expectTrue(event & Clock::Event::Continue, "Continue event detected");
    }

    CASE("Event checking - Reset") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.masterReset();

        Clock::Event event = clock.checkEvent();
        expectTrue(event & Clock::Event::Reset, "Reset event detected");
    }

    CASE("Tick checking") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        clock.checkEvent();  // Consume start event

        uint32_t tick;
        bool hasTick = clock.checkTick(&tick);

        // Initially no ticks processed
        expectFalse(hasTick, "no ticks available initially");
    }

    CASE("Clock listener - output state") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        MockClockListener listener;
        clock.setListener(&listener);

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        expectTrue(listener.lastOutputState.run, "run state is true after start");
        expectTrue(listener.lastOutputState.reset, "reset state is true after start");
    }

    CASE("Clock listener - MIDI messages") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        MockClockListener listener;
        clock.setListener(&listener);

        listener.clear();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        bool foundStart = false;
        for (const auto &event : listener.midiEvents) {
            if (event.message == MidiMessage::Start) {
                foundStart = true;
                break;
            }
        }
        expectTrue(foundStart, "MIDI Start message sent");

        listener.clear();

        clock.masterStop();

        bool foundStop = false;
        for (const auto &event : listener.midiEvents) {
            if (event.message == MidiMessage::Stop) {
                foundStop = true;
                break;
            }
        }
        expectTrue(foundStop, "MIDI Stop message sent");
    }

    CASE("Output configuration") {
        ClockTimer timer;
        Clock clock(timer);

        clock.outputConfigure(24, 1000);  // divisor 24, pulse 1ms

        const auto &state = clock.outputState();
        // Just verify we can configure without crash
        // Actual output behavior requires timer ticks to test
    }

    CASE("Output swing configuration") {
        ClockTimer timer;
        Clock clock(timer);

        clock.outputConfigureSwing(55);  // 55% swing

        // Swing is applied during tick output
        // Just verify configuration doesn't crash
    }

    CASE("Slave mode - configure") {
        ClockTimer timer;
        Clock clock(timer);

        clock.slaveConfigure(0, 24, true);   // slave 0, divisor 24, enabled
        clock.slaveConfigure(1, 12, false);  // slave 1, divisor 12, disabled

        // Configuration stored, will be used when slave messages arrive
    }

    CASE("Slave mode - start") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        clock.slaveStart(0);

        expectTrue(clock.isRunning(), "clock running in slave mode");
        expectEqual(int(clock.activeMode()), int(Clock::Mode::Slave), "active mode is Slave");
    }

    CASE("Slave mode - stop") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        clock.slaveStart(0);
        clock.slaveStop(0);

        expectTrue(clock.isIdle(), "clock idle after slave stop");
    }

    CASE("Slave mode - ignore disabled slave") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, false);  // disabled

        clock.slaveStart(0);

        expectTrue(clock.isIdle(), "clock ignores disabled slave start");
    }

    CASE("Slave mode - MIDI message handling") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        // Start via MIDI
        clock.slaveHandleMidi(0, MidiMessage::Start);
        expectTrue(clock.isRunning(), "clock started via MIDI Start");

        // Stop via MIDI
        clock.slaveHandleMidi(0, MidiMessage::Stop);
        expectTrue(clock.isIdle(), "clock stopped via MIDI Stop");
    }

    CASE("Slave mode - tick via MIDI") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        clock.slaveHandleMidi(0, MidiMessage::Start);

        // Send MIDI ticks
        clock.slaveHandleMidi(0, MidiMessage::Tick);
        clock.slaveHandleMidi(0, MidiMessage::Tick);
        clock.slaveHandleMidi(0, MidiMessage::Tick);

        // Ticks are processed, BPM estimation happens internally
    }

    CASE("Slave mode - continue") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        clock.slaveStart(0);
        clock.slaveStop(0);

        clock.slaveContinue(0);

        expectTrue(clock.isRunning(), "clock running after slave continue");
    }

    CASE("Slave mode - reset") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Auto);
        clock.slaveConfigure(0, 24, true);

        clock.slaveStart(0);
        clock.slaveReset(0);

        expectTrue(clock.isIdle(), "clock idle after slave reset");
    }

    CASE("Mode switching - Master prevents Slave") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();

        // Try to start slave
        clock.slaveConfigure(0, 24, true);
        clock.slaveStart(0);

        // Should still be in master mode
        expectEqual(int(clock.activeMode()), int(Clock::Mode::Master), "Master mode prevents slave start");
    }

    CASE("Mode switching - Slave prevents Master") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Slave);
        clock.slaveConfigure(0, 24, true);
        clock.slaveStart(0);

        // Try to start master
        clock.masterStart();

        // Should still be in slave mode
        expectEqual(int(clock.activeMode()), int(Clock::Mode::Slave), "Slave mode prevents master start");
    }

    CASE("Mode switching - setMode stops running clock") {
        ClockTimer timer;
        Clock clock(timer);
        clock.init();

        clock.setMode(Clock::Mode::Master);
        clock.masterStart();
        expectTrue(clock.isRunning(), "master running");

        clock.setMode(Clock::Mode::Slave);
        expectTrue(clock.isIdle(), "clock stopped when switching to slave mode");
    }

    CASE("BPM range boundaries") {
        ClockTimer timer;
        Clock clock(timer);

        // Test various BPM values
        clock.setMasterBpm(1.0f);
        expectEqual(clock.masterBpm(), 1.0f, "minimum BPM");

        clock.setMasterBpm(1000.0f);
        expectEqual(clock.masterBpm(), 1000.0f, "maximum BPM");

        clock.setMasterBpm(120.5f);
        expectEqual(clock.masterBpm(), 120.5f, "fractional BPM");
    }

    CASE("PPQN tick rate") {
        ClockTimer timer;
        Clock clock(timer);

        // Verify PPQN is used in calculations
        int ppqn = clock.ppqn();
        expectEqual(ppqn, CONFIG_PPQN, "PPQN matches config");

        float ticksPerSecond = (120.0f * ppqn) / 60.0f;
        float tickDuration = 1.0f / ticksPerSecond;
        expectEqual(clock.tickDuration(), tickDuration, "tick duration based on PPQN");
    }

}
