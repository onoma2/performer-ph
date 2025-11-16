#include "UnitTest.h"

#include "core/midi/MidiMessage.h"

#include <cstdint>

UNIT_TEST("MidiMessage") {

    CASE("NoteOff message construction and parsing") {
        auto msg = MidiMessage::makeNoteOff(0, 60, 64);
        expectTrue(msg.isNoteOff(), "is note off message");
        expectFalse(msg.isNoteOn(), "is not note on message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(0), "channel");
        expectEqual(msg.note(), uint8_t(60), "note");
        expectEqual(msg.velocity(), uint8_t(64), "velocity");
        expectEqual(msg.status(), uint8_t(0x80), "status byte");
        expectEqual(msg.length(), uint8_t(3), "message length");
    }

    CASE("NoteOn message construction and parsing") {
        auto msg = MidiMessage::makeNoteOn(5, 72, 100);
        expectTrue(msg.isNoteOn(), "is note on message");
        expectFalse(msg.isNoteOff(), "is not note off message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(5), "channel");
        expectEqual(msg.note(), uint8_t(72), "note");
        expectEqual(msg.velocity(), uint8_t(100), "velocity");
        expectEqual(msg.status(), uint8_t(0x95), "status byte");
    }

    CASE("NoteOn with velocity 0 converts to NoteOff") {
        auto msg = MidiMessage::makeNoteOn(3, 48, 0);
        msg.fixFakeNoteOff();
        expectTrue(msg.isNoteOff(), "is note off after fixFakeNoteOff");
        expectFalse(msg.isNoteOn(), "is not note on after fixFakeNoteOff");
        expectEqual(msg.channel(), uint8_t(3), "channel preserved");
        expectEqual(msg.note(), uint8_t(48), "note preserved");
        expectEqual(msg.velocity(), uint8_t(0), "velocity preserved");
    }

    CASE("ControlChange message construction and parsing") {
        auto msg = MidiMessage::makeControlChange(7, 74, 127);
        expectTrue(msg.isControlChange(), "is control change message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(7), "channel");
        expectEqual(msg.controlNumber(), uint8_t(74), "control number");
        expectEqual(msg.controlValue(), uint8_t(127), "control value");
        expectEqual(msg.status(), uint8_t(0xb7), "status byte");
    }

    CASE("ProgramChange message construction and parsing") {
        auto msg = MidiMessage::makeProgramChange(2, 42);
        expectTrue(msg.isProgramChange(), "is program change message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(2), "channel");
        expectEqual(msg.programNumber(), uint8_t(42), "program number");
        expectEqual(msg.length(), uint8_t(2), "message length");
    }

    CASE("PitchBend message construction and parsing") {
        auto msg = MidiMessage::makePitchBend(1, 0);
        expectTrue(msg.isPitchBend(), "is pitch bend message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(1), "channel");
        expectEqual(msg.pitchBend(), int(0), "pitch bend centered at 0");

        // Test positive pitch bend
        auto msgPos = MidiMessage::makePitchBend(1, 4096);
        expectEqual(msgPos.pitchBend(), int(4096), "positive pitch bend");

        // Test negative pitch bend
        auto msgNeg = MidiMessage::makePitchBend(1, -4096);
        expectEqual(msgNeg.pitchBend(), int(-4096), "negative pitch bend");
    }

    CASE("ChannelPressure message construction and parsing") {
        auto msg = MidiMessage::makeChannelPressure(8, 85);
        expectTrue(msg.isChannelPressure(), "is channel pressure message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(8), "channel");
        expectEqual(msg.channelPressure(), uint8_t(85), "pressure value");
        expectEqual(msg.length(), uint8_t(2), "message length");
    }

    CASE("KeyPressure message construction and parsing") {
        auto msg = MidiMessage::makeKeyPressure(4, 60, 50);
        expectTrue(msg.isKeyPressure(), "is key pressure message");
        expectTrue(msg.isChannelMessage(), "is channel message");
        expectEqual(msg.channel(), uint8_t(4), "channel");
        expectEqual(msg.note(), uint8_t(60), "note");
        expectEqual(msg.keyPressure(), uint8_t(50), "pressure value");
    }

    CASE("RealTime messages - Tick") {
        MidiMessage msg(MidiMessage::Tick);
        expectTrue(msg.isTick(), "is tick message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectTrue(msg.isClockMessage(), "is clock message");
        expectEqual(msg.status(), uint8_t(0xf8), "status byte");
        expectEqual(msg.length(), uint8_t(1), "message length");
    }

    CASE("RealTime messages - Start") {
        MidiMessage msg(MidiMessage::Start);
        expectTrue(msg.isStart(), "is start message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectTrue(msg.isClockMessage(), "is clock message");
        expectEqual(msg.status(), uint8_t(0xfa), "status byte");
    }

    CASE("RealTime messages - Stop") {
        MidiMessage msg(MidiMessage::Stop);
        expectTrue(msg.isStop(), "is stop message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectTrue(msg.isClockMessage(), "is clock message");
        expectEqual(msg.status(), uint8_t(0xfc), "status byte");
    }

    CASE("RealTime messages - Continue") {
        MidiMessage msg(MidiMessage::Continue);
        expectTrue(msg.isContinue(), "is continue message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectTrue(msg.isClockMessage(), "is clock message");
        expectEqual(msg.status(), uint8_t(0xfb), "status byte");
    }

    CASE("RealTime messages - Reset") {
        MidiMessage msg(MidiMessage::Reset);
        expectTrue(msg.isReset(), "is reset message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectFalse(msg.isClockMessage(), "is not clock message");
        expectEqual(msg.status(), uint8_t(0xff), "status byte");
    }

    CASE("RealTime messages - ActiveSensing") {
        MidiMessage msg(MidiMessage::ActiveSensing);
        expectTrue(msg.isActiveSensing(), "is active sensing message");
        expectTrue(msg.isRealTimeMessage(), "is real-time message");
        expectFalse(msg.isClockMessage(), "is not clock message");
        expectEqual(msg.status(), uint8_t(0xfe), "status byte");
    }

    CASE("System messages - TuneRequest") {
        MidiMessage msg(MidiMessage::TuneRequest);
        expectTrue(msg.isTuneRequest(), "is tune request message");
        expectTrue(msg.isSystemMessage(), "is system message");
        expectEqual(msg.status(), uint8_t(0xf6), "status byte");
    }

    CASE("System messages - SongPosition") {
        MidiMessage msg(MidiMessage::SongPosition, 0x00, 0x10);
        expectTrue(msg.isSongPosition(), "is song position message");
        expectTrue(msg.isSystemMessage(), "is system message");
        expectEqual(msg.songPosition(), int(0x800), "song position value");
        expectEqual(msg.length(), uint8_t(3), "message length");
    }

    CASE("System messages - SongSelect") {
        MidiMessage msg(MidiMessage::SongSelect, 5);
        expectTrue(msg.isSongSelect(), "is song select message");
        expectTrue(msg.isSystemMessage(), "is system message");
        expectEqual(msg.songNumber(), uint8_t(5), "song number");
        expectEqual(msg.length(), uint8_t(2), "message length");
    }

    CASE("System messages - TimeCode") {
        MidiMessage msg(MidiMessage::TimeCode, 0x42);
        expectTrue(msg.isTimeCode(), "is time code message");
        expectTrue(msg.isSystemMessage(), "is system message");
        expectEqual(msg.data0(), uint8_t(0x42), "time code data");
        expectEqual(msg.length(), uint8_t(2), "message length");
    }

    CASE("Channel message all channels") {
        for (int ch = 0; ch < 16; ++ch) {
            auto msg = MidiMessage::makeNoteOn(ch, 60, 100);
            expectEqual(msg.channel(), uint8_t(ch), "channel matches");
        }
    }

    CASE("Note range boundaries") {
        // Test min note
        auto msgMin = MidiMessage::makeNoteOn(0, 0, 100);
        expectEqual(msgMin.note(), uint8_t(0), "minimum note");

        // Test max note
        auto msgMax = MidiMessage::makeNoteOn(0, 127, 100);
        expectEqual(msgMax.note(), uint8_t(127), "maximum note");
    }

    CASE("Velocity range boundaries") {
        // Test min velocity
        auto msgMin = MidiMessage::makeNoteOn(0, 60, 0);
        expectEqual(msgMin.velocity(), uint8_t(0), "minimum velocity");

        // Test max velocity
        auto msgMax = MidiMessage::makeNoteOn(0, 60, 127);
        expectEqual(msgMax.velocity(), uint8_t(127), "maximum velocity");
    }

    CASE("Message copy constructor") {
        auto msg1 = MidiMessage::makeNoteOn(3, 60, 100);
        MidiMessage msg2(msg1);

        expectTrue(msg2.isNoteOn(), "copied message is note on");
        expectEqual(msg2.channel(), msg1.channel(), "channel matches");
        expectEqual(msg2.note(), msg1.note(), "note matches");
        expectEqual(msg2.velocity(), msg1.velocity(), "velocity matches");
    }

    CASE("Message assignment operator") {
        auto msg1 = MidiMessage::makeControlChange(5, 20, 64);
        MidiMessage msg2;
        msg2 = msg1;

        expectTrue(msg2.isControlChange(), "assigned message is control change");
        expectEqual(msg2.channel(), msg1.channel(), "channel matches");
        expectEqual(msg2.controlNumber(), msg1.controlNumber(), "control number matches");
        expectEqual(msg2.controlValue(), msg1.controlValue(), "control value matches");
    }

    CASE("Raw message construction - 3 byte") {
        uint8_t raw[3] = { 0x90, 0x3c, 0x64 };
        MidiMessage msg(raw, 3);

        expectTrue(msg.isNoteOn(), "is note on from raw");
        expectEqual(msg.status(), uint8_t(0x90), "status byte");
        expectEqual(msg.data0(), uint8_t(0x3c), "data0");
        expectEqual(msg.data1(), uint8_t(0x64), "data1");
    }

    CASE("Raw message construction - 2 byte") {
        uint8_t raw[2] = { 0xc0, 0x2a };
        MidiMessage msg(raw, 2);

        expectTrue(msg.isProgramChange(), "is program change from raw");
        expectEqual(msg.status(), uint8_t(0xc0), "status byte");
        expectEqual(msg.data0(), uint8_t(0x2a), "data0");
    }

    CASE("Raw message construction - 1 byte") {
        uint8_t raw[1] = { 0xfa };
        MidiMessage msg(raw, 1);

        expectTrue(msg.isStart(), "is start from raw");
        expectEqual(msg.status(), uint8_t(0xfa), "status byte");
    }

}
