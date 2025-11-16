#include "UnitTest.h"

#include "core/midi/MidiParser.h"
#include "core/midi/MidiMessage.h"

#include <cstdint>

UNIT_TEST("MidiParser") {

    CASE("Parse NoteOn message") {
        MidiParser parser;
        bool result;

        result = parser.feed(0x90);  // NoteOn, channel 0
        expectFalse(result, "status byte doesn't emit message");

        result = parser.feed(0x3c);  // note 60
        expectFalse(result, "first data byte doesn't emit message");

        result = parser.feed(0x64);  // velocity 100
        expectTrue(result, "complete message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isNoteOn(), "parsed message is note on");
        expectEqual(msg.channel(), uint8_t(0), "channel");
        expectEqual(msg.note(), uint8_t(60), "note");
        expectEqual(msg.velocity(), uint8_t(100), "velocity");
    }

    CASE("Parse NoteOff message") {
        MidiParser parser;
        bool result;

        result = parser.feed(0x82);  // NoteOff, channel 2
        expectFalse(result, "status byte doesn't emit");

        result = parser.feed(0x40);  // note 64
        expectFalse(result, "first data byte doesn't emit");

        result = parser.feed(0x00);  // velocity 0
        expectTrue(result, "complete message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isNoteOff(), "parsed message is note off");
        expectEqual(msg.channel(), uint8_t(2), "channel");
        expectEqual(msg.note(), uint8_t(64), "note");
    }

    CASE("Parse ControlChange message") {
        MidiParser parser;

        parser.feed(0xb5);  // CC, channel 5
        parser.feed(0x4a);  // controller 74
        bool result = parser.feed(0x7f);  // value 127
        expectTrue(result, "complete message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isControlChange(), "parsed message is CC");
        expectEqual(msg.channel(), uint8_t(5), "channel");
        expectEqual(msg.controlNumber(), uint8_t(74), "controller number");
        expectEqual(msg.controlValue(), uint8_t(127), "controller value");
    }

    CASE("Parse ProgramChange message - 2 bytes") {
        MidiParser parser;

        parser.feed(0xc3);  // PC, channel 3
        bool result = parser.feed(0x2a);  // program 42
        expectTrue(result, "2-byte message complete");

        const auto &msg = parser.message();
        expectTrue(msg.isProgramChange(), "parsed message is PC");
        expectEqual(msg.channel(), uint8_t(3), "channel");
        expectEqual(msg.programNumber(), uint8_t(42), "program number");
    }

    CASE("Parse ChannelPressure message - 2 bytes") {
        MidiParser parser;

        parser.feed(0xd7);  // Channel Pressure, channel 7
        bool result = parser.feed(0x55);  // pressure 85
        expectTrue(result, "2-byte message complete");

        const auto &msg = parser.message();
        expectTrue(msg.isChannelPressure(), "parsed message is channel pressure");
        expectEqual(msg.channel(), uint8_t(7), "channel");
        expectEqual(msg.channelPressure(), uint8_t(85), "pressure value");
    }

    CASE("Parse PitchBend message") {
        MidiParser parser;

        parser.feed(0xe1);  // Pitch bend, channel 1
        parser.feed(0x00);  // LSB
        bool result = parser.feed(0x40);  // MSB
        expectTrue(result, "complete message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isPitchBend(), "parsed message is pitch bend");
        expectEqual(msg.channel(), uint8_t(1), "channel");
        expectEqual(msg.pitchBend(), int(0), "pitch bend centered");
    }

    CASE("Parse RealTime message - Tick") {
        MidiParser parser;

        bool result = parser.feed(0xf8);  // Tick
        expectTrue(result, "real-time message emits immediately");

        const auto &msg = parser.message();
        expectTrue(msg.isTick(), "parsed message is tick");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
    }

    CASE("Parse RealTime message - Start") {
        MidiParser parser;

        bool result = parser.feed(0xfa);  // Start
        expectTrue(result, "start message emits immediately");

        const auto &msg = parser.message();
        expectTrue(msg.isStart(), "parsed message is start");
    }

    CASE("Parse RealTime message - Stop") {
        MidiParser parser;

        bool result = parser.feed(0xfc);  // Stop
        expectTrue(result, "stop message emits immediately");

        const auto &msg = parser.message();
        expectTrue(msg.isStop(), "parsed message is stop");
    }

    CASE("Parse RealTime message - Continue") {
        MidiParser parser;

        bool result = parser.feed(0xfb);  // Continue
        expectTrue(result, "continue message emits immediately");

        const auto &msg = parser.message();
        expectTrue(msg.isContinue(), "parsed message is continue");
    }

    CASE("Running status - same message type") {
        MidiParser parser;

        // First complete message
        parser.feed(0x90);  // NoteOn, channel 0
        parser.feed(0x3c);  // note 60
        parser.feed(0x64);  // velocity 100

        // Second message using running status (no status byte)
        parser.feed(0x40);  // note 64
        bool result = parser.feed(0x50);  // velocity 80
        expectTrue(result, "running status message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isNoteOn(), "parsed message is note on");
        expectEqual(msg.note(), uint8_t(64), "note from running status");
        expectEqual(msg.velocity(), uint8_t(80), "velocity from running status");
    }

    CASE("Running status interrupted by real-time") {
        MidiParser parser;

        parser.feed(0x90);  // NoteOn, channel 0
        parser.feed(0x3c);  // note 60

        // Real-time message interrupts but doesn't cancel running status
        bool realTimeResult = parser.feed(0xf8);  // Tick
        expectTrue(realTimeResult, "real-time message emits");
        expectTrue(parser.message().isTick(), "tick message received");

        // Continue with data byte
        bool noteResult = parser.feed(0x64);  // velocity 100
        expectTrue(noteResult, "note message completes");

        const auto &msg = parser.message();
        expectTrue(msg.isNoteOn(), "parsed message is note on");
        expectEqual(msg.note(), uint8_t(60), "note");
        expectEqual(msg.velocity(), uint8_t(100), "velocity");
    }

    CASE("Running status - multiple messages") {
        MidiParser parser;

        // First message with status
        parser.feed(0x90);
        parser.feed(0x3c);
        parser.feed(0x64);

        // Multiple messages with running status
        for (int i = 0; i < 5; ++i) {
            parser.feed(40 + i);  // varying notes
            bool result = parser.feed(50 + i);  // varying velocities
            expectTrue(result, "running status message emits");
            expectEqual(parser.message().note(), uint8_t(40 + i), "note matches");
        }
    }

    CASE("System message - TuneRequest") {
        MidiParser parser;

        bool result = parser.feed(0xf6);  // TuneRequest
        expectTrue(result, "tune request emits immediately");

        const auto &msg = parser.message();
        expectTrue(msg.isTuneRequest(), "parsed message is tune request");
    }

    CASE("System message - SongPosition") {
        MidiParser parser;

        parser.feed(0xf2);  // Song Position
        parser.feed(0x00);  // LSB
        bool result = parser.feed(0x10);  // MSB
        expectTrue(result, "song position message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isSongPosition(), "parsed message is song position");
        expectEqual(msg.songPosition(), int(0x800), "song position value");
    }

    CASE("System message - SongSelect") {
        MidiParser parser;

        parser.feed(0xf3);  // Song Select
        bool result = parser.feed(0x05);  // song number
        expectTrue(result, "song select message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isSongSelect(), "parsed message is song select");
        expectEqual(msg.songNumber(), uint8_t(5), "song number");
    }

    CASE("System message - TimeCode") {
        MidiParser parser;

        parser.feed(0xf1);  // Time Code
        bool result = parser.feed(0x42);  // time code data
        expectTrue(result, "time code message emits");

        const auto &msg = parser.message();
        expectTrue(msg.isTimeCode(), "parsed message is time code");
        expectEqual(msg.data0(), uint8_t(0x42), "time code data");
    }

    CASE("System Exclusive start") {
        MidiParser parser;

        bool result = parser.feed(0xf0);  // SysEx start
        expectFalse(result, "sysex start doesn't emit");

        // Feed some sysex data
        result = parser.feed(0x41);  // manufacturer ID
        expectFalse(result, "sysex data doesn't emit");

        result = parser.feed(0x10);  // device ID
        expectFalse(result, "sysex data doesn't emit");
    }

    CASE("System Exclusive end") {
        MidiParser parser;

        parser.feed(0xf0);  // SysEx start
        parser.feed(0x41);  // data
        bool result = parser.feed(0xf7);  // SysEx end
        expectFalse(result, "sysex end doesn't emit yet (not implemented)");
    }

    CASE("System message cancels running status") {
        MidiParser parser;

        // Establish running status
        parser.feed(0x90);  // NoteOn
        parser.feed(0x3c);
        parser.feed(0x64);

        // System message cancels running status
        parser.feed(0xf6);  // TuneRequest

        // Try to use running status - should not work
        parser.feed(0x40);
        bool result = parser.feed(0x50);
        expectFalse(result, "running status cancelled by system message");
    }

    CASE("Multiple sequential channel messages") {
        MidiParser parser;

        // Note On
        parser.feed(0x91);
        parser.feed(0x40);
        bool result1 = parser.feed(0x64);
        expectTrue(result1 && parser.message().isNoteOn(), "note on parsed");

        // Control Change
        parser.feed(0xb2);
        parser.feed(0x07);
        bool result2 = parser.feed(0x7f);
        expectTrue(result2 && parser.message().isControlChange(), "CC parsed");

        // Program Change
        parser.feed(0xc3);
        bool result3 = parser.feed(0x10);
        expectTrue(result3 && parser.message().isProgramChange(), "PC parsed");

        // Note Off
        parser.feed(0x84);
        parser.feed(0x3c);
        bool result4 = parser.feed(0x00);
        expectTrue(result4 && parser.message().isNoteOff(), "note off parsed");
    }

    CASE("Data bytes ignored when no running status") {
        MidiParser parser;

        // Feed data bytes without status
        bool result1 = parser.feed(0x40);  // data byte
        expectFalse(result1, "data byte without status ignored");

        bool result2 = parser.feed(0x64);  // another data byte
        expectFalse(result2, "data byte without status ignored");

        // Now send proper message
        parser.feed(0x90);
        parser.feed(0x3c);
        bool result3 = parser.feed(0x64);
        expectTrue(result3, "proper message parsed");
        expectTrue(parser.message().isNoteOn(), "note on message");
    }

    CASE("All 16 MIDI channels") {
        MidiParser parser;

        for (int ch = 0; ch < 16; ++ch) {
            parser.feed(0x90 | ch);  // NoteOn on channel ch
            parser.feed(0x3c);
            bool result = parser.feed(0x64);
            expectTrue(result, "message parsed");
            expectEqual(parser.message().channel(), uint8_t(ch), "channel matches");
        }
    }

    CASE("Boundary values for note and velocity") {
        MidiParser parser;

        // Min note, min velocity
        parser.feed(0x90);
        parser.feed(0x00);  // note 0
        parser.feed(0x00);  // velocity 0
        expectEqual(parser.message().note(), uint8_t(0), "min note");
        expectEqual(parser.message().velocity(), uint8_t(0), "min velocity");

        // Max note, max velocity
        parser.feed(0x90);
        parser.feed(0x7f);  // note 127
        parser.feed(0x7f);  // velocity 127
        expectEqual(parser.message().note(), uint8_t(127), "max note");
        expectEqual(parser.message().velocity(), uint8_t(127), "max velocity");
    }

    CASE("Parser state persistence across messages") {
        MidiParser parser;

        // Parse first message
        parser.feed(0x90);
        parser.feed(0x3c);
        parser.feed(0x64);

        // Parser should be ready for next message
        parser.feed(0xb0);
        parser.feed(0x07);
        bool result = parser.feed(0x7f);
        expectTrue(result, "second message parsed correctly");
        expectTrue(parser.message().isControlChange(), "correct message type");
    }

}
