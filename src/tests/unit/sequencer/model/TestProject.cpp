#include "UnitTest.h"

#include "tests/unit/core/io/MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#include "apps/sequencer/model/Types.cpp"
#include "apps/sequencer/model/Scale.cpp"
#include "apps/sequencer/model/Calibration.cpp"
#include "apps/sequencer/model/TimeSignature.cpp"
#include "apps/sequencer/model/Curve.cpp"
#include "apps/sequencer/model/UserScale.cpp"
#include "apps/sequencer/model/Routing.cpp"
#include "apps/sequencer/model/MidiOutput.cpp"
#include "apps/sequencer/model/ClockSetup.cpp"
#include "apps/sequencer/model/CurveSequence.cpp"
#include "apps/sequencer/model/NoteSequence.cpp"
#include "apps/sequencer/model/NoteTrack.cpp"
#include "apps/sequencer/model/CurveTrack.cpp"
#include "apps/sequencer/model/MidiCvTrack.cpp"
#include "apps/sequencer/model/Track.cpp"
#include "apps/sequencer/model/Song.cpp"
#include "apps/sequencer/model/PlayState.cpp"
#include "apps/sequencer/model/Project.cpp"

#include <cstring>

UNIT_TEST("Project") {

    CASE("Default project values") {
        Project project;

        expectEqual(project.name(), "INIT", "default name is INIT");
        expectEqual(project.tempo(), 120.f, "default tempo is 120");
        expectEqual(project.swing(), 50, "default swing is 50%");
        expectFalse(project.slotAssigned(), "no slot assigned by default");
    }

    CASE("Set project name") {
        Project project;

        project.setName("Test Project");
        expectEqual(project.name(), "Test Project", "name set correctly");

        project.setName("Another Name");
        expectEqual(project.name(), "Another Name", "name updated");
    }

    CASE("Set tempo") {
        Project project;

        project.setTempo(140.f);
        expectEqual(project.tempo(), 140.f, "tempo set to 140");

        project.setTempo(60.f);
        expectEqual(project.tempo(), 60.f, "tempo set to 60");

        project.setTempo(180.5f);
        expectEqual(project.tempo(), 180.5f, "tempo set to 180.5");
    }

    CASE("Tempo clamping") {
        Project project;

        project.setTempo(0.5f);
        expectEqual(project.tempo(), 1.f, "tempo clamped to minimum 1");

        project.setTempo(1500.f);
        expectEqual(project.tempo(), 1000.f, "tempo clamped to maximum 1000");
    }

    CASE("Set swing") {
        Project project;

        project.setSwing(60);
        expectEqual(project.swing(), 60, "swing set to 60%");

        project.setSwing(55);
        expectEqual(project.swing(), 55, "swing set to 55%");
    }

    CASE("Swing clamping") {
        Project project;

        project.setSwing(40);
        expectEqual(project.swing(), 50, "swing clamped to minimum 50%");

        project.setSwing(80);
        expectEqual(project.swing(), 75, "swing clamped to maximum 75%");
    }

    CASE("Set slot") {
        Project project;

        project.setSlot(5);
        expectEqual(project.slot(), 5, "slot set to 5");
        expectTrue(project.slotAssigned(), "slot is assigned");

        project.setSlot(0);
        expectEqual(project.slot(), 0, "slot set to 0");
        expectTrue(project.slotAssigned(), "slot is assigned");
    }

    CASE("Auto loaded flag") {
        Project project;

        expectFalse(project.autoLoaded(), "not auto loaded by default");

        project.setAutoLoaded(true);
        expectTrue(project.autoLoaded(), "auto loaded set to true");

        project.setAutoLoaded(false);
        expectFalse(project.autoLoaded(), "auto loaded set to false");
    }

    CASE("Track access") {
        Project project;

        // Project should have CONFIG_TRACK_COUNT tracks
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            Track &track = project.track(i);
            // Just verify we can access tracks without crash
            expectEqual(track.trackIndex(), i, "track index matches");
        }
    }

    CASE("User scale access") {
        Project project;

        // Project should have user scales
        for (int i = 0; i < CONFIG_USER_SCALE_COUNT; ++i) {
            UserScale &scale = project.userScale(i);
            // Verify we can access user scales
            expectEqual(scale.size(), 12, "default user scale size is 12");
        }
    }

    CASE("Song access") {
        Project project;

        Song &song = project.song();
        // Verify song is accessible
        expectEqual(song.slotCount(), CONFIG_SONG_SLOT_COUNT, "song has correct slot count");
    }

    CASE("Play state access") {
        Project project;

        PlayState &playState = project.playState();
        // Verify play state is accessible
        expectFalse(playState.running(), "play state not running by default");
    }

    CASE("Project clear") {
        Project project;

        // Modify project
        project.setName("Modified");
        project.setTempo(150.f);
        project.setSwing(65);

        // Clear
        project.clear();

        // Verify reset to defaults
        expectEqual(project.name(), "INIT", "name cleared");
        expectEqual(project.tempo(), 120.f, "tempo cleared");
        expectEqual(project.swing(), 50, "swing cleared");
    }

    CASE("Basic serialization - write and read") {
        uint8_t buffer[65536];  // Large buffer for project data
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;
        originalProject.setName("Test Serialize");
        originalProject.setTempo(135.f);
        originalProject.setSwing(60);

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify
        expectEqual(loadedProject.name(), originalProject.name(), "name preserved");
        expectEqual(loadedProject.tempo(), originalProject.tempo(), "tempo preserved");
        expectEqual(loadedProject.swing(), originalProject.swing(), "swing preserved");
    }

    CASE("Track data preservation in serialization") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Modify first track
        originalProject.track(0).setName("Track1");
        originalProject.track(0).noteTrack().setScale(3);

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify track data
        expectEqual(loadedProject.track(0).name(), originalProject.track(0).name(), "track name preserved");
        expectEqual(loadedProject.track(0).noteTrack().scale(), originalProject.track(0).noteTrack().scale(), "track scale preserved");
    }

    CASE("Multiple tracks serialization") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Modify multiple tracks
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            char name[8];
            snprintf(name, sizeof(name), "T%d", i);
            originalProject.track(i).setName(name);
        }

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify all tracks
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            expectEqual(loadedProject.track(i).name(), originalProject.track(i).name(), "all track names preserved");
        }
    }

    CASE("Song data preservation") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Modify song
        originalProject.song().setName("MySong");

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify song
        expectEqual(loadedProject.song().name(), originalProject.song().name(), "song name preserved");
    }

    CASE("User scale preservation") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Modify a user scale
        originalProject.userScale(0).setName("MyScale");
        originalProject.userScale(0).setSize(7);

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify user scale
        expectEqual(loadedProject.userScale(0).name(), originalProject.userScale(0).name(), "user scale name preserved");
        expectEqual(loadedProject.userScale(0).size(), originalProject.userScale(0).size(), "user scale size preserved");
    }

    CASE("Routing configuration preservation") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Modify routing
        originalProject.routing().setChannel(0, Types::MidiPort::Midi, 1);

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Verify routing
        expectEqual(int(loadedProject.routing().channel(0).port()),
                   int(originalProject.routing().channel(0).port()), "routing port preserved");
    }

    CASE("Complete project round-trip") {
        uint8_t buffer[65536];
        std::memset(buffer, 0, sizeof(buffer));

        Project originalProject;

        // Create a complex project state
        originalProject.setName("Complex Project");
        originalProject.setTempo(142.5f);
        originalProject.setSwing(58);

        // Modify various tracks
        originalProject.track(0).setName("Kick");
        originalProject.track(1).setName("Snare");
        originalProject.track(2).setName("HiHat");

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalProject.write(writer);
        }

        // Read
        Project loadedProject;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            loadedProject.read(reader);
        }

        // Comprehensive verification
        expectEqual(loadedProject.name(), "Complex Project", "project name");
        expectEqual(loadedProject.tempo(), 142.5f, "project tempo");
        expectEqual(loadedProject.swing(), 58, "project swing");
        expectEqual(loadedProject.track(0).name(), "Kick", "track 0 name");
        expectEqual(loadedProject.track(1).name(), "Snare", "track 1 name");
        expectEqual(loadedProject.track(2).name(), "HiHat", "track 2 name");
    }

}
