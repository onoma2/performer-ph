#include "UnitTest.h"

#include "apps/sequencer/engine/generators/RandomGenerator.cpp"
#include "apps/sequencer/engine/generators/Generator.cpp"
#include "apps/sequencer/engine/generators/SequenceBuilder.h"

#include "core/utils/Random.h"

#include <cmath>

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

UNIT_TEST("RandomGenerator") {

    CASE("Default parameters") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        expectEqual(gen.seed(), 0, "default seed is 0");
        expectEqual(gen.smooth(), 0, "default smooth is 0");
        expectEqual(gen.bias(), 0, "default bias is 0");
        expectEqual(gen.scale(), 10, "default scale is 10");
    }

    CASE("Set seed") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setSeed(100);
        expectEqual(gen.seed(), 100, "seed set to 100");

        gen.setSeed(500);
        expectEqual(gen.seed(), 500, "seed set to 500");
    }

    CASE("Set seed clamping") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setSeed(-1);
        expectEqual(gen.seed(), 0, "seed clamped to minimum 0");

        gen.setSeed(1001);
        expectEqual(gen.seed(), 1000, "seed clamped to maximum 1000");
    }

    CASE("Set smooth") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setSmooth(5);
        expectEqual(gen.smooth(), 5, "smooth set to 5");

        gen.setSmooth(10);
        expectEqual(gen.smooth(), 10, "smooth set to 10");
    }

    CASE("Set smooth clamping") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setSmooth(-1);
        expectEqual(gen.smooth(), 0, "smooth clamped to minimum 0");

        gen.setSmooth(11);
        expectEqual(gen.smooth(), 10, "smooth clamped to maximum 10");
    }

    CASE("Set bias") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setBias(5);
        expectEqual(gen.bias(), 5, "bias set to 5");

        gen.setBias(-5);
        expectEqual(gen.bias(), -5, "bias set to -5");
    }

    CASE("Set bias clamping") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setBias(-11);
        expectEqual(gen.bias(), -10, "bias clamped to minimum -10");

        gen.setBias(11);
        expectEqual(gen.bias(), 10, "bias clamped to maximum 10");
    }

    CASE("Set scale") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setScale(50);
        expectEqual(gen.scale(), 50, "scale set to 50");

        gen.setScale(0);
        expectEqual(gen.scale(), 0, "scale set to 0");
    }

    CASE("Set scale clamping") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.setScale(-1);
        expectEqual(gen.scale(), 0, "scale clamped to minimum 0");

        gen.setScale(101);
        expectEqual(gen.scale(), 100, "scale clamped to maximum 100");
    }

    CASE("Same seed produces same pattern") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params2.seed = 42;

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Same seed should produce identical patterns
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            expectEqual(builder1.values[i], builder2.values[i], "same seed produces same pattern");
        }
    }

    CASE("Different seed produces different pattern") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params2.seed = 100;

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Different seeds should produce different patterns
        bool different = false;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            if (builder1.values[i] != builder2.values[i]) {
                different = true;
                break;
            }
        }
        expectTrue(different, "different seed produces different pattern");
    }

    CASE("Values are in valid range [0, 1]") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        params.seed = 123;
        RandomGenerator gen(builder, params);

        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            expect(builder.values[i] >= 0.0f && builder.values[i] <= 1.0f, "value in range [0, 1]");
        }
    }

    CASE("Smooth parameter reduces variation") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params1.smooth = 0;
        params2.seed = 42;
        params2.smooth = 5;

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Calculate variation (sum of absolute differences between adjacent values)
        float variation1 = 0;
        float variation2 = 0;

        for (int i = 0; i < CONFIG_STEP_COUNT - 1; ++i) {
            variation1 += std::abs(builder1.values[i + 1] - builder1.values[i]);
            variation2 += std::abs(builder2.values[i + 1] - builder2.values[i]);
        }

        expect(variation2 < variation1, "smoothing reduces variation");
    }

    CASE("Bias shifts values up") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params1.bias = 0;
        params2.seed = 42;
        params2.bias = 5;  // Positive bias

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Calculate averages
        float avg1 = 0, avg2 = 0;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            avg1 += builder1.values[i];
            avg2 += builder2.values[i];
        }
        avg1 /= CONFIG_STEP_COUNT;
        avg2 /= CONFIG_STEP_COUNT;

        expect(avg2 > avg1, "positive bias increases average");
    }

    CASE("Bias shifts values down") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params1.bias = 0;
        params2.seed = 42;
        params2.bias = -5;  // Negative bias

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Calculate averages
        float avg1 = 0, avg2 = 0;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            avg1 += builder1.values[i];
            avg2 += builder2.values[i];
        }
        avg1 /= CONFIG_STEP_COUNT;
        avg2 /= CONFIG_STEP_COUNT;

        expect(avg2 < avg1, "negative bias decreases average");
    }

    CASE("Scale affects variation") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params1.scale = 10;  // Default scale
        params2.seed = 42;
        params2.scale = 5;   // Reduced scale

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Calculate range (max - min)
        float min1 = 1.0f, max1 = 0.0f;
        float min2 = 1.0f, max2 = 0.0f;

        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            min1 = std::min(min1, builder1.values[i]);
            max1 = std::max(max1, builder1.values[i]);
            min2 = std::min(min2, builder2.values[i]);
            max2 = std::max(max2, builder2.values[i]);
        }

        float range1 = max1 - min1;
        float range2 = max2 - min2;

        expect(range2 < range1, "reduced scale reduces range");
    }

    CASE("Scale = 0 produces constant middle value") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        params.seed = 42;
        params.scale = 0;
        params.bias = 0;
        RandomGenerator gen(builder, params);

        // With scale=0, all values should be close to 0.5 (middle)
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            expect(std::abs(builder.values[i] - 0.5f) < 0.1f, "scale=0 produces near-constant values");
        }
    }

    CASE("Pattern pattern() accessor") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        params.seed = 42;
        RandomGenerator gen(builder, params);

        const auto &pattern = gen.pattern();

        // Pattern should have CONFIG_STEP_COUNT elements
        expectEqual(int(pattern.size()), CONFIG_STEP_COUNT, "pattern size matches CONFIG_STEP_COUNT");

        // Values should be valid
        for (size_t i = 0; i < pattern.size(); ++i) {
            expect(pattern[i] <= 255, "pattern value in valid range");
        }
    }

    CASE("Update regenerates pattern") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        params.seed = 42;
        RandomGenerator gen(builder, params);

        float original[CONFIG_STEP_COUNT];
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            original[i] = builder.values[i];
        }

        // Change seed and update
        gen.setSeed(100);
        gen.update();

        // Pattern should be different
        bool different = false;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            if (builder.values[i] != original[i]) {
                different = true;
                break;
            }
        }
        expectTrue(different, "update regenerates pattern");
    }

    CASE("Parameter editing") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        gen.editParam(int(RandomGenerator::Param::Seed), 10, false);
        expectEqual(gen.seed(), 10, "seed incremented");

        gen.editParam(int(RandomGenerator::Param::Smooth), 3, false);
        expectEqual(gen.smooth(), 3, "smooth incremented");

        gen.editParam(int(RandomGenerator::Param::Bias), 2, false);
        expectEqual(gen.bias(), 2, "bias incremented");

        gen.editParam(int(RandomGenerator::Param::Scale), 5, false);
        expectEqual(gen.scale(), 15, "scale incremented");
    }

    CASE("Parameter names") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        expectEqual(gen.paramName(int(RandomGenerator::Param::Seed)), "Seed", "seed param name");
        expectEqual(gen.paramName(int(RandomGenerator::Param::Smooth)), "Smooth", "smooth param name");
        expectEqual(gen.paramName(int(RandomGenerator::Param::Bias)), "Bias", "bias param name");
        expectEqual(gen.paramName(int(RandomGenerator::Param::Scale)), "Scale", "scale param name");
    }

    CASE("Init resets to defaults") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        params.seed = 100;
        params.smooth = 5;
        params.bias = 3;
        params.scale = 50;
        RandomGenerator gen(builder, params);

        gen.init();

        expectEqual(gen.seed(), 0, "seed reset to default");
        expectEqual(gen.smooth(), 0, "smooth reset to default");
        expectEqual(gen.bias(), 0, "bias reset to default");
        expectEqual(gen.scale(), 10, "scale reset to default");
    }

    CASE("Mode is Random") {
        MockSequenceBuilder builder;
        RandomGenerator::Params params;
        RandomGenerator gen(builder, params);

        expectEqual(int(gen.mode()), int(Generator::Mode::Random), "mode is Random");
    }

    CASE("Smoothing multiple iterations") {
        MockSequenceBuilder builder1, builder2, builder3;
        RandomGenerator::Params params1, params2, params3;
        params1.seed = 42;
        params1.smooth = 1;
        params2.seed = 42;
        params2.smooth = 3;
        params3.seed = 42;
        params3.smooth = 5;

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);
        RandomGenerator gen3(builder3, params3);

        // More smoothing should produce smoother curves
        float variation1 = 0, variation2 = 0, variation3 = 0;
        for (int i = 0; i < CONFIG_STEP_COUNT - 1; ++i) {
            variation1 += std::abs(builder1.values[i + 1] - builder1.values[i]);
            variation2 += std::abs(builder2.values[i + 1] - builder2.values[i]);
            variation3 += std::abs(builder3.values[i + 1] - builder3.values[i]);
        }

        expect(variation2 < variation1, "more smoothing reduces variation (3 vs 1)");
        expect(variation3 < variation2, "more smoothing reduces variation (5 vs 3)");
    }

    CASE("Extreme bias values") {
        MockSequenceBuilder builder1, builder2;
        RandomGenerator::Params params1, params2;
        params1.seed = 42;
        params1.bias = 10;  // Maximum positive bias
        params2.seed = 42;
        params2.bias = -10; // Maximum negative bias

        RandomGenerator gen1(builder1, params1);
        RandomGenerator gen2(builder2, params2);

        // Max positive bias should push values toward 1.0
        float avg1 = 0;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            avg1 += builder1.values[i];
        }
        avg1 /= CONFIG_STEP_COUNT;

        // Max negative bias should push values toward 0.0
        float avg2 = 0;
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            avg2 += builder2.values[i];
        }
        avg2 /= CONFIG_STEP_COUNT;

        expect(avg1 > 0.5f, "max positive bias pushes average above 0.5");
        expect(avg2 < 0.5f, "max negative bias pushes average below 0.5");
    }

}
