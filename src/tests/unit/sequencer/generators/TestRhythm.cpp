#include "UnitTest.h"

#include "apps/sequencer/engine/generators/Rhythm.cpp"

UNIT_TEST("Rhythm") {

    CASE("Euclidean E(4,16) - classic four-on-the-floor") {
        auto pattern = Rhythm::euclidean(4, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        // Count beats
        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 4, "4 beats in pattern");

        // Expected pattern: x---x---x---x---
        expectTrue(pattern[0], "beat at 0");
        expectFalse(pattern[1], "no beat at 1");
        expectFalse(pattern[2], "no beat at 2");
        expectFalse(pattern[3], "no beat at 3");
        expectTrue(pattern[4], "beat at 4");
        expectTrue(pattern[8], "beat at 8");
        expectTrue(pattern[12], "beat at 12");
    }

    CASE("Euclidean E(5,8) - Cuban cinquillo") {
        auto pattern = Rhythm::euclidean(5, 8);

        expectEqual(int(pattern.size()), 8, "pattern size is 8");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in pattern");
    }

    CASE("Euclidean E(3,8) - tresillo") {
        auto pattern = Rhythm::euclidean(3, 8);

        expectEqual(int(pattern.size()), 8, "pattern size is 8");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 3, "3 beats in pattern");

        // Expected pattern: x--x--x-
        expectTrue(pattern[0], "beat at 0");
        expectFalse(pattern[1], "no beat at 1");
        expectFalse(pattern[2], "no beat at 2");
        expectTrue(pattern[3], "beat at 3");
    }

    CASE("Euclidean E(7,12) - West African bell pattern") {
        auto pattern = Rhythm::euclidean(7, 12);

        expectEqual(int(pattern.size()), 12, "pattern size is 12");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 7, "7 beats in pattern");
    }

    CASE("Euclidean E(5,13) - thirteenth note pattern") {
        auto pattern = Rhythm::euclidean(5, 13);

        expectEqual(int(pattern.size()), 13, "pattern size is 13");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in pattern");
    }

    CASE("Euclidean E(9,16) - complex pattern") {
        auto pattern = Rhythm::euclidean(9, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 9, "9 beats in pattern");
    }

    CASE("Beats equals steps - all beats") {
        auto pattern = Rhythm::euclidean(8, 8);

        expectEqual(int(pattern.size()), 8, "pattern size is 8");

        // All should be beats
        for (size_t i = 0; i < pattern.size(); ++i) {
            expectTrue(pattern[i], "all steps are beats");
        }
    }

    CASE("Beats = 1 - single beat") {
        auto pattern = Rhythm::euclidean(1, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        int beatCount = 0;
        int beatPosition = -1;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
                if (beatPosition == -1) {
                    beatPosition = i;
                }
            }
        }

        expectEqual(beatCount, 1, "only 1 beat in pattern");
        expectEqual(beatPosition, 0, "beat is at first position");
    }

    CASE("Beats = 0 - no beats") {
        auto pattern = Rhythm::euclidean(0, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }

        expectEqual(beatCount, 0, "no beats in pattern");
    }

    CASE("Beats > steps - clamped to steps") {
        auto pattern = Rhythm::euclidean(20, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        // Should clamp beats to steps
        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }

        expectEqual(beatCount, 16, "beats clamped to steps");
    }

    CASE("Pattern shifting") {
        auto pattern = Rhythm::euclidean(4, 16);
        auto shifted = pattern.shifted(4);

        expectEqual(int(shifted.size()), 16, "shifted pattern size is 16");

        // Original pattern: x---x---x---x---
        // Shifted by 4:     ----x---x---x---x (rotated left)

        int beatCount = 0;
        for (size_t i = 0; i < shifted.size(); ++i) {
            if (shifted[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 4, "shifted pattern has 4 beats");
    }

    CASE("Pattern shifting by 0 - no change") {
        auto pattern = Rhythm::euclidean(5, 8);
        auto shifted = pattern.shifted(0);

        // Should be identical
        for (size_t i = 0; i < pattern.size(); ++i) {
            expectEqual(pattern[i], shifted[i], "pattern unchanged with 0 shift");
        }
    }

    CASE("Pattern shifting wraps around") {
        auto pattern = Rhythm::euclidean(4, 8);

        // Shift by pattern size should return to original
        auto shifted8 = pattern.shifted(8);

        for (size_t i = 0; i < pattern.size(); ++i) {
            expectEqual(pattern[i], shifted8[i], "shift by size returns to original");
        }
    }

    CASE("RhythmString set and access") {
        Rhythm::Pattern pattern;

        pattern.set(0, true);
        pattern.set(5, true);
        pattern.set(10, true);

        expectTrue(pattern[0], "bit 0 is set");
        expectFalse(pattern[1], "bit 1 is not set");
        expectTrue(pattern[5], "bit 5 is set");
        expectTrue(pattern[10], "bit 10 is set");
    }

    CASE("RhythmString clear") {
        Rhythm::Pattern pattern;

        pattern.set(0, true);
        pattern.set(5, true);
        pattern.clear();

        expectEqual(int(pattern.size()), 0, "size is 0 after clear");
    }

    CASE("RhythmString resize") {
        Rhythm::Pattern pattern;

        pattern.resize(16);
        expectEqual(int(pattern.size()), 16, "size is 16 after resize");

        pattern.resize(8);
        expectEqual(int(pattern.size()), 8, "size is 8 after resize");
    }

    CASE("RhythmString append") {
        Rhythm::Pattern pattern1, pattern2;

        pattern1.set(0, true);
        pattern1.set(1, false);

        pattern2.set(0, false);
        pattern2.set(1, true);

        pattern1.append(pattern2);

        expectEqual(int(pattern1.size()), 4, "appended size is 4");
        expectTrue(pattern1[0], "original pattern preserved");
        expectFalse(pattern1[1], "original pattern preserved");
        expectFalse(pattern1[2], "appended pattern added");
        expectTrue(pattern1[3], "appended pattern added");
    }

    CASE("Euclidean distribution is even") {
        // E(3, 8) should distribute beats evenly
        auto pattern = Rhythm::euclidean(3, 8);

        // Find positions of beats
        int positions[3];
        int idx = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                positions[idx++] = i;
            }
        }

        // Calculate gaps between beats
        int gap1 = positions[1] - positions[0];
        int gap2 = positions[2] - positions[1];
        int gap3 = (8 - positions[2]) + positions[0];

        // Gaps should be roughly equal (2 or 3 for E(3,8))
        expect(gap1 >= 2 && gap1 <= 3, "gap1 is even");
        expect(gap2 >= 2 && gap2 <= 3, "gap2 is even");
        expect(gap3 >= 2 && gap3 <= 3, "gap3 is even");
    }

    CASE("Large pattern E(16,64)") {
        auto pattern = Rhythm::euclidean(16, 64);

        expectEqual(int(pattern.size()), 64, "pattern size is 64");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 16, "16 beats in pattern");
    }

    CASE("E(2,5) - Aka rhythm") {
        auto pattern = Rhythm::euclidean(2, 5);

        expectEqual(int(pattern.size()), 5, "pattern size is 5");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 2, "2 beats in pattern");
    }

    CASE("E(3,4) - Waltz") {
        auto pattern = Rhythm::euclidean(3, 4);

        expectEqual(int(pattern.size()), 4, "pattern size is 4");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 3, "3 beats in pattern");
    }

    CASE("E(5,9) - Aksak rhythm") {
        auto pattern = Rhythm::euclidean(5, 9);

        expectEqual(int(pattern.size()), 9, "pattern size is 9");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in pattern");
    }

    CASE("E(5,16) - Bossa nova") {
        auto pattern = Rhythm::euclidean(5, 16);

        expectEqual(int(pattern.size()), 16, "pattern size is 16");

        int beatCount = 0;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i]) {
                beatCount++;
            }
        }
        expectEqual(beatCount, 5, "5 beats in pattern");
    }

    CASE("Capacity check") {
        Rhythm::Pattern pattern;

        expectEqual(int(pattern.capacity()), CONFIG_STEP_COUNT, "capacity matches CONFIG_STEP_COUNT");
    }

}
