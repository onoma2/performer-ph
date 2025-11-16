#include "UnitTest.h"

#include "tests/unit/core/io/MemoryReaderWriter.h"

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#include "apps/sequencer/model/NoteSequence.cpp"

#include <cstring>

UNIT_TEST("NoteSequence") {

    CASE("Step default values") {
        NoteSequence::Step step;
        expectEqual(step.gate(), false, "default gate is false");
        expectEqual(step.gateProbability(), NoteSequence::GateProbability::Max, "default gate probability");
        expectEqual(step.gateOffset(), 0, "default gate offset");
        expectEqual(step.slide(), false, "default slide is false");
        expectEqual(step.retrigger(), 0, "default retrigger");
        expectEqual(step.retriggerProbability(), NoteSequence::RetriggerProbability::Max, "default retrigger probability");
        expectEqual(step.length(), NoteSequence::Length::Max / 2, "default length");
        expectEqual(step.lengthVariationRange(), 0, "default length variation range");
        expectEqual(step.lengthVariationProbability(), NoteSequence::LengthVariationProbability::Max, "default length variation probability");
        expectEqual(step.note(), 0, "default note");
        expectEqual(step.noteVariationRange(), 0, "default note variation range");
        expectEqual(step.noteVariationProbability(), NoteSequence::NoteVariationProbability::Max, "default note variation probability");
        expectEqual(int(step.condition()), int(Types::Condition::Off), "default condition");
    }

    CASE("Step gate property") {
        NoteSequence::Step step;
        step.setGate(true);
        expectEqual(step.gate(), true, "gate set to true");

        step.setGate(false);
        expectEqual(step.gate(), false, "gate set to false");

        step.toggleGate();
        expectEqual(step.gate(), true, "gate toggled to true");

        step.toggleGate();
        expectEqual(step.gate(), false, "gate toggled to false");
    }

    CASE("Step gateProbability property") {
        NoteSequence::Step step;
        step.setGateProbability(5);
        expectEqual(step.gateProbability(), 5, "gate probability set");

        // Test clamping
        step.setGateProbability(NoteSequence::GateProbability::Max + 1);
        expectEqual(step.gateProbability(), NoteSequence::GateProbability::Max, "gate probability clamped to max");

        step.setGateProbability(-1);
        expectEqual(step.gateProbability(), 0, "gate probability clamped to min");
    }

    CASE("Step gateOffset property") {
        NoteSequence::Step step;
        step.setGateOffset(10);
        expectEqual(step.gateOffset(), 10, "gate offset set");

        // Test boundaries
        step.setGateOffset(0);
        expectEqual(step.gateOffset(), 0, "gate offset at min");

        step.setGateOffset(NoteSequence::GateOffset::Max);
        expect(step.gateOffset() <= NoteSequence::GateOffset::Max, "gate offset within max");
    }

    CASE("Step slide property") {
        NoteSequence::Step step;
        step.setSlide(true);
        expectEqual(step.slide(), true, "slide set to true");

        step.toggleSlide();
        expectEqual(step.slide(), false, "slide toggled to false");

        step.toggleSlide();
        expectEqual(step.slide(), true, "slide toggled to true");
    }

    CASE("Step retrigger property") {
        NoteSequence::Step step;
        step.setRetrigger(3);
        expectEqual(step.retrigger(), 3, "retrigger set");

        // Test clamping
        step.setRetrigger(NoteSequence::Retrigger::Max + 1);
        expectEqual(step.retrigger(), NoteSequence::Retrigger::Max, "retrigger clamped to max");
    }

    CASE("Step length property") {
        NoteSequence::Step step;
        step.setLength(5);
        expectEqual(step.length(), 5, "length set");

        step.setLength(NoteSequence::Length::Max);
        expectEqual(step.length(), NoteSequence::Length::Max, "length at max");

        step.setLength(0);
        expectEqual(step.length(), 0, "length at min");
    }

    CASE("Step note property") {
        NoteSequence::Step step;
        step.setNote(60);
        expectEqual(step.note(), 60, "note set to middle C");

        step.setNote(0);
        expectEqual(step.note(), 0, "note at zero");

        step.setNote(-24);
        expectEqual(step.note(), -24, "negative note");

        // Test range boundaries
        step.setNote(NoteSequence::Note::Min);
        expectEqual(step.note(), NoteSequence::Note::Min, "note at min");

        step.setNote(NoteSequence::Note::Max);
        expectEqual(step.note(), NoteSequence::Note::Max, "note at max");
    }

    CASE("Step noteVariationRange property") {
        NoteSequence::Step step;
        step.setNoteVariationRange(12);
        expectEqual(step.noteVariationRange(), 12, "note variation range set");

        step.setNoteVariationRange(-12);
        expectEqual(step.noteVariationRange(), -12, "negative note variation range");

        step.setNoteVariationRange(0);
        expectEqual(step.noteVariationRange(), 0, "zero note variation range");
    }

    CASE("Step condition property") {
        NoteSequence::Step step;
        step.setCondition(Types::Condition::Fill);
        expectEqual(int(step.condition()), int(Types::Condition::Fill), "condition set to Fill");

        step.setCondition(Types::Condition::Off);
        expectEqual(int(step.condition()), int(Types::Condition::Off), "condition set to Off");
    }

    CASE("Step layerValue/setLayerValue") {
        NoteSequence::Step step;

        // Test gate layer
        step.setLayerValue(NoteSequence::Layer::Gate, 1);
        expectEqual(step.layerValue(NoteSequence::Layer::Gate), 1, "gate via layer API");

        // Test note layer
        step.setLayerValue(NoteSequence::Layer::Note, 48);
        expectEqual(step.layerValue(NoteSequence::Layer::Note), 48, "note via layer API");

        // Test length layer
        step.setLayerValue(NoteSequence::Layer::Length, 3);
        expectEqual(step.layerValue(NoteSequence::Layer::Length), 3, "length via layer API");
    }

    CASE("Step clear") {
        NoteSequence::Step step;

        // Modify all properties
        step.setGate(true);
        step.setNote(60);
        step.setLength(7);
        step.setSlide(true);

        // Clear and verify defaults
        step.clear();
        expectEqual(step.gate(), false, "gate cleared");
        expectEqual(step.note(), 0, "note cleared");
        expectEqual(step.slide(), false, "slide cleared");
        expectEqual(step.length(), NoteSequence::Length::Max / 2, "length reset to default");
    }

    CASE("Step equality") {
        NoteSequence::Step step1, step2;

        expectTrue(step1 == step2, "default steps are equal");

        step1.setGate(true);
        expectFalse(step1 == step2, "modified steps are not equal");
        expectTrue(step1 != step2, "inequality operator works");

        step2.setGate(true);
        expectTrue(step1 == step2, "modified steps are equal again");
    }

    CASE("Step serialization - write and read") {
        uint8_t buffer[256];
        std::memset(buffer, 0, sizeof(buffer));

        NoteSequence::Step originalStep;
        originalStep.setGate(true);
        originalStep.setNote(60);
        originalStep.setVelocity(100);
        originalStep.setLength(5);
        originalStep.setSlide(true);
        originalStep.setRetrigger(2);
        originalStep.setGateProbability(6);

        // Write
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalStep.write(writer);
        }

        // Read
        NoteSequence::Step readStep;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            readStep.read(reader);
        }

        // Verify
        expectTrue(readStep == originalStep, "deserialized step matches original");
        expectEqual(readStep.gate(), true, "gate preserved");
        expectEqual(readStep.note(), 60, "note preserved");
        expectEqual(readStep.velocity(), 100, "velocity preserved");
        expectEqual(readStep.length(), 5, "length preserved");
        expectEqual(readStep.slide(), true, "slide preserved");
        expectEqual(readStep.retrigger(), 2, "retrigger preserved");
        expectEqual(readStep.gateProbability(), 6, "gate probability preserved");
    }

    CASE("Step serialization - all layers") {
        uint8_t buffer[256];

        NoteSequence::Step originalStep;
        originalStep.setGate(true);
        originalStep.setGateProbability(4);
        originalStep.setGateOffset(5);
        originalStep.setSlide(true);
        originalStep.setRetrigger(1);
        originalStep.setRetriggerProbability(5);
        originalStep.setLength(6);
        originalStep.setLengthVariationRange(-3);
        originalStep.setLengthVariationProbability(4);
        originalStep.setNote(48);
        originalStep.setNoteVariationRange(12);
        originalStep.setNoteVariationProbability(3);
        originalStep.setCondition(Types::Condition::Fill);

        // Write and read
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalStep.write(writer);
        }

        NoteSequence::Step readStep;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            readStep.read(reader);
        }

        // Verify all layers
        expectEqual(readStep.gate(), originalStep.gate(), "gate layer");
        expectEqual(readStep.gateProbability(), originalStep.gateProbability(), "gate probability layer");
        expectEqual(readStep.gateOffset(), originalStep.gateOffset(), "gate offset layer");
        expectEqual(readStep.slide(), originalStep.slide(), "slide layer");
        expectEqual(readStep.retrigger(), originalStep.retrigger(), "retrigger layer");
        expectEqual(readStep.retriggerProbability(), originalStep.retriggerProbability(), "retrigger probability layer");
        expectEqual(readStep.length(), originalStep.length(), "length layer");
        expectEqual(readStep.lengthVariationRange(), originalStep.lengthVariationRange(), "length variation range layer");
        expectEqual(readStep.lengthVariationProbability(), originalStep.lengthVariationProbability(), "length variation probability layer");
        expectEqual(readStep.note(), originalStep.note(), "note layer");
        expectEqual(readStep.noteVariationRange(), originalStep.noteVariationRange(), "note variation range layer");
        expectEqual(readStep.noteVariationProbability(), originalStep.noteVariationProbability(), "note variation probability layer");
        expectEqual(int(readStep.condition()), int(originalStep.condition()), "condition layer");
    }

    CASE("Step serialization - boundary values") {
        uint8_t buffer[256];

        NoteSequence::Step originalStep;
        originalStep.setNote(NoteSequence::Note::Max);
        originalStep.setNoteVariationRange(NoteSequence::NoteVariationRange::Min);
        originalStep.setLength(NoteSequence::Length::Max);
        originalStep.setGateProbability(NoteSequence::GateProbability::Max);

        // Write and read
        {
            MemoryWriter memoryWriter(buffer, sizeof(buffer));
            VersionedSerializedWriter writer([&memoryWriter] (const void *data, size_t len) {
                memoryWriter.write(data, len);
            }, ProjectVersion::Version27);
            originalStep.write(writer);
        }

        NoteSequence::Step readStep;
        {
            MemoryReader memoryReader(buffer, sizeof(buffer));
            VersionedSerializedReader reader([&memoryReader] (void *data, size_t len) {
                memoryReader.read(data, len);
            }, ProjectVersion::Version27);
            readStep.read(reader);
        }

        // Verify boundary values preserved
        expectEqual(readStep.note(), NoteSequence::Note::Max, "max note preserved");
        expectEqual(readStep.noteVariationRange(), NoteSequence::NoteVariationRange::Min, "min note variation preserved");
        expectEqual(readStep.length(), NoteSequence::Length::Max, "max length preserved");
        expectEqual(readStep.gateProbability(), NoteSequence::GateProbability::Max, "max gate probability preserved");
    }

    CASE("NoteSequence default values") {
        NoteSequence sequence;
        expectEqual(sequence.steps(), CONFIG_STEP_COUNT, "default step count");
        expectEqual(sequence.firstStep(), 0, "default first step");
        expectEqual(sequence.lastStep(), CONFIG_STEP_COUNT - 1, "default last step");
        expectEqual(sequence.divisor(), 12, "default divisor");
        expectEqual(sequence.scale(), 0, "default scale");
        expectEqual(sequence.rootNote(), 0, "default root note");
    }

    CASE("NoteSequence step access") {
        NoteSequence sequence;

        // Modify a step
        sequence.step(5).setGate(true);
        sequence.step(5).setNote(72);

        expectEqual(sequence.step(5).gate(), true, "step 5 gate set");
        expectEqual(sequence.step(5).note(), 72, "step 5 note set");

        // Verify other steps unchanged
        expectEqual(sequence.step(4).gate(), false, "step 4 gate unchanged");
        expectEqual(sequence.step(6).gate(), false, "step 6 gate unchanged");
    }

    CASE("NoteSequence divisor") {
        NoteSequence sequence;
        sequence.setDivisor(6);
        expectEqual(sequence.divisor(), 6, "divisor set");

        sequence.setDivisor(192);
        expectEqual(sequence.divisor(), 192, "divisor set to max");

        sequence.setDivisor(1);
        expectEqual(sequence.divisor(), 1, "divisor set to min");
    }

    CASE("NoteSequence scale and root note") {
        NoteSequence sequence;
        sequence.setScale(5);
        expectEqual(sequence.scale(), 5, "scale set");

        sequence.setRootNote(3);
        expectEqual(sequence.rootNote(), 3, "root note set");
    }

    CASE("NoteSequence first and last step") {
        NoteSequence sequence;
        sequence.setFirstStep(4);
        sequence.setLastStep(12);

        expectEqual(sequence.firstStep(), 4, "first step set");
        expectEqual(sequence.lastStep(), 12, "last step set");
    }

    CASE("NoteSequence clear sequence") {
        NoteSequence sequence;

        // Set some values
        sequence.step(0).setGate(true);
        sequence.step(1).setNote(60);
        sequence.setDivisor(24);

        // Clear
        sequence.clear();

        // Verify cleared
        expectEqual(sequence.step(0).gate(), false, "step 0 gate cleared");
        expectEqual(sequence.step(1).note(), 0, "step 1 note cleared");
        // Note: divisor is not cleared by clear(), it's a sequence property
    }

}
