#include "UnitTest.h"

#include "apps/sequencer/engine/generators/Rhythm.cpp"
#include "apps/sequencer/engine/generators/EuclideanGenerator.cpp"
#include "apps/sequencer/engine/generators/Generator.cpp"
#include "apps/sequencer/engine/generators/SequenceBuilder.h"

// Mock SequenceBuilder for testing
class MockSequenceBuilder : public SequenceBuilder {
public:
    void revert() override {}
    void setValue(int index, float value) override {
        if (index >= 0 && index < CONFIG_STEP_COUNT) {
            values[index] = value;
        }
    }
    void setLength(int length) override {
        this->length = length;
    }

    float values[CONFIG_STEP_COUNT] = {};
    int length = 0;
};

UNIT_TEST("EuclideanGenerator") {

    CASE("Default parameters") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        expectEqual(gen.steps(), 16, "default steps is 16");
        expectEqual(gen.beats(), 4, "default beats is 4");
        expectEqual(gen.offset(), 0, "default offset is 0");
    }

    CASE("Set steps") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setSteps(8);
        expectEqual(gen.steps(), 8, "steps set to 8");

        gen.setSteps(16);
        expectEqual(gen.steps(), 16, "steps set to 16");

        gen.setSteps(32);
        expectEqual(gen.steps(), 32, "steps set to 32");
    }

    CASE("Set steps clamping") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setSteps(0);
        expectEqual(gen.steps(), 1, "steps clamped to minimum 1");

        gen.setSteps(CONFIG_STEP_COUNT + 1);
        expectEqual(gen.steps(), CONFIG_STEP_COUNT, "steps clamped to CONFIG_STEP_COUNT");
    }

    CASE("Set beats") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setBeats(8);
        expectEqual(gen.beats(), 8, "beats set to 8");

        gen.setBeats(3);
        expectEqual(gen.beats(), 3, "beats set to 3");
    }

    CASE("Set beats clamping") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setBeats(0);
        expectEqual(gen.beats(), 1, "beats clamped to minimum 1");

        gen.setBeats(CONFIG_STEP_COUNT + 1);
        expectEqual(gen.beats(), CONFIG_STEP_COUNT, "beats clamped to CONFIG_STEP_COUNT");
    }

    CASE("Set offset") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setOffset(4);
        expectEqual(gen.offset(), 4, "offset set to 4");

        gen.setOffset(0);
        expectEqual(gen.offset(), 0, "offset set to 0");
    }

    CASE("Set offset clamping") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        gen.setOffset(-1);
        expectEqual(gen.offset(), 0, "offset clamped to minimum 0");

        gen.setOffset(CONFIG_STEP_COUNT);
        expectEqual(gen.offset(), CONFIG_STEP_COUNT - 1, "offset clamped to CONFIG_STEP_COUNT - 1");
    }

    CASE("Pattern generation - E(4,16) - classic four-on-the-floor") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 4;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Expected pattern: x---x---x---x--- (beats on 0, 4, 8, 12)
        expectEqual(builder.values[0], 1.0f, "beat at step 0");
        expectEqual(builder.values[1], 0.0f, "no beat at step 1");
        expectEqual(builder.values[2], 0.0f, "no beat at step 2");
        expectEqual(builder.values[3], 0.0f, "no beat at step 3");
        expectEqual(builder.values[4], 1.0f, "beat at step 4");
        expectEqual(builder.values[8], 1.0f, "beat at step 8");
        expectEqual(builder.values[12], 1.0f, "beat at step 12");
    }

    CASE("Pattern generation - E(5,8) - classic Cuban cinquillo") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 5;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Expected pattern: x-xx-xx- (Bjorklund's algorithm)
        int beatCount = 0;
        for (int i = 0; i < 8; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in 8 steps");
    }

    CASE("Pattern generation - E(3,8)") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 3;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Expected pattern: x--x--x-
        int beatCount = 0;
        for (int i = 0; i < 8; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 3, "3 beats in 8 steps");
    }

    CASE("Pattern offset - rotate pattern") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 4;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Get original pattern
        float original[8];
        for (int i = 0; i < 8; ++i) {
            original[i] = builder.values[i];
        }

        // Apply offset
        gen.setOffset(2);
        gen.update();

        // Pattern should be rotated by 2 steps
        for (int i = 0; i < 8; ++i) {
            expectEqual(builder.values[i], original[(i - 2 + 8) % 8], "pattern rotated by offset");
        }
    }

    CASE("Pattern offset - full rotation") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 4;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Get original pattern
        float original[16];
        for (int i = 0; i < 16; ++i) {
            original[i] = builder.values[i];
        }

        // Offset by full steps should return to original
        gen.setOffset(16);
        gen.update();

        for (int i = 0; i < 16; ++i) {
            expectEqual(builder.values[i], original[i], "full rotation returns to original");
        }
    }

    CASE("Beats equals steps - all on") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 8;
        EuclideanGenerator gen(builder, params);

        // All steps should be on
        for (int i = 0; i < 8; ++i) {
            expectEqual(builder.values[i], 1.0f, "all steps have beats");
        }
    }

    CASE("Beats = 1 - single beat") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 1;
        EuclideanGenerator gen(builder, params);

        // Only first step should be on
        int beatCount = 0;
        for (int i = 0; i < 16; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 1, "only one beat");
        expectEqual(builder.values[0], 1.0f, "beat at first step");
    }

    CASE("Builder length is set") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 12;
        EuclideanGenerator gen(builder, params);

        expectEqual(builder.length, 12, "builder length set to steps");

        gen.setSteps(24);
        gen.update();

        expectEqual(builder.length, 24, "builder length updated with steps");
    }

    CASE("Pattern pattern() accessor") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 3;
        EuclideanGenerator gen(builder, params);

        const auto &pattern = gen.pattern();

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 3, "pattern has 3 beats");
    }

    CASE("Parameter editing") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 4;
        params.offset = 0;
        EuclideanGenerator gen(builder, params);

        // Edit steps
        gen.editParam(int(EuclideanGenerator::Param::Steps), 4, false);
        expectEqual(gen.steps(), 20, "steps incremented by 4");

        // Edit beats
        gen.editParam(int(EuclideanGenerator::Param::Beats), 2, false);
        expectEqual(gen.beats(), 6, "beats incremented by 2");

        // Edit offset
        gen.editParam(int(EuclideanGenerator::Param::Offset), 3, false);
        expectEqual(gen.offset(), 3, "offset incremented by 3");
    }

    CASE("Parameter names") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        expectEqual(gen.paramName(int(EuclideanGenerator::Param::Steps)), "Steps", "steps param name");
        expectEqual(gen.paramName(int(EuclideanGenerator::Param::Beats)), "Beats", "beats param name");
        expectEqual(gen.paramName(int(EuclideanGenerator::Param::Offset)), "Offset", "offset param name");
    }

    CASE("Init resets to defaults") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 8;
        params.beats = 3;
        params.offset = 4;
        EuclideanGenerator gen(builder, params);

        gen.init();

        expectEqual(gen.steps(), 16, "steps reset to default");
        expectEqual(gen.beats(), 4, "beats reset to default");
        expectEqual(gen.offset(), 0, "offset reset to default");
    }

    CASE("Mode is Euclidean") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        EuclideanGenerator gen(builder, params);

        expectEqual(int(gen.mode()), int(Generator::Mode::Euclidean), "mode is Euclidean");
    }

    CASE("Known Euclidean rhythms - E(5,13) - thirteenth note pattern") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 13;
        params.beats = 5;
        EuclideanGenerator gen(builder, params);

        int beatCount = 0;
        for (int i = 0; i < 13; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in 13 steps");
    }

    CASE("Known Euclidean rhythms - E(7,12) - West African bell pattern") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 12;
        params.beats = 7;
        EuclideanGenerator gen(builder, params);

        int beatCount = 0;
        for (int i = 0; i < 12; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 7, "7 beats in 12 steps");
    }

    CASE("Edge case - 0 beats (clamped to 1)") {
        MockSequenceBuilder builder;
        EuclideanGenerator::Params params;
        params.steps = 16;
        params.beats = 0;
        EuclideanGenerator gen(builder, params);

        // Should be clamped to 1 beat
        int beatCount = 0;
        for (int i = 0; i < 16; ++i) {
            if (builder.values[i] == 1.0f) {
                beatCount++;
            }
        }
        expect(beatCount >= 1, "at least 1 beat after clamping");
    }

}
