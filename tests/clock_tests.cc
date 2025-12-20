//
// Tests for clock class
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <clock.h>

class ClockTests : public testing::Test {
public:
	virtual void SetUp() { }
	
	virtual void TearDown()	{}
};

TEST_F(ClockTests, BelowThresholdDoesNotDelay) {
    uint64_t count = 100;
    BusClock_t clock(1);
    bool didDelay = false;

    clock.enableTimingEmulation();

    while(count--) {
        didDelay |= clock.delay(1);
    }

    EXPECT_FALSE(didDelay);
}

TEST_F(ClockTests, AboveThresholdDelays) {
    BusClock_t clock(1);
    uint64_t count = clock.getCyclesInDelayTime();
    bool didDelay = false;

    clock.enableTimingEmulation();

    while(count--) {
        didDelay |= clock.delay(1);
    }

    EXPECT_TRUE(didDelay);
}

TEST_F(ClockTests, BelowThresholdDoesNotDelayAt4MHz) {
    uint64_t count = 100;
    BusClock_t clock(4);
    bool didDelay = false;

    clock.enableTimingEmulation();

    while(count--) {
        didDelay |= clock.delay(1);
    }

    EXPECT_FALSE(didDelay);
}

TEST_F(ClockTests, AboveThresholdDelaysAt4MHz) {
    BusClock_t clock(4);
    uint64_t count = clock.getCyclesInDelayTime();
    bool didDelay = false;

    clock.enableTimingEmulation();

    while(count--) {
        didDelay |= clock.delay(1);
    }

    EXPECT_TRUE(didDelay);
}

TEST_F(ClockTests, CanGetClockFrequency) {
    static constexpr uint16_t _MHz = 4;
    BusClock_t clock(_MHz);

    EXPECT_EQ(clock.getFrequencyMHz(), _MHz);
}

TEST_F(ClockTests, CanGetAccumulatedClockCycles) {
    static constexpr uint16_t _MHz = 4;
    BusClock_t clock(_MHz);

    clock.delay(10000);

    EXPECT_EQ(clock.getAccumulatedCycles(), 10000);
}

TEST_F(ClockTests, DelayConsumesAccumulatedCycles) {
    static constexpr uint16_t _MHz = 4;
    BusClock_t clock(_MHz);
    uint64_t cycles = clock.getCyclesInDelayTime();
    static constexpr uint64_t constant = 1500;

    clock.delay(cycles + constant);

    EXPECT_EQ(clock.getAccumulatedCycles(), constant);
}

TEST_F(ClockTests, CantSetLowMHz) {
    BusClock_t clock(0);
    EXPECT_EQ(clock.getFrequencyMHz(), 1);
}

TEST_F(ClockTests, CantSetHighMHz) {
    BusClock_t clock(1001);
    EXPECT_EQ(clock.getFrequencyMHz(), 1000);
}

// Additional tests for more coverage
TEST_F(ClockTests, TimingEmulationOffDisablesDelay) {
    BusClock_t clock(1);
    uint64_t count = clock.getCyclesInDelayTime();
    bool didDelay = false;

    clock.disableTimingEmulation();

    while(count--) {
        didDelay |= clock.delay(1);
    }
    EXPECT_FALSE(didDelay);
}

TEST_F(ClockTests, CustomDelayChangesThreshold) {
    constexpr std::chrono::milliseconds customDelay(50);
    BusClock_t clock(2, customDelay);

    EXPECT_EQ(clock.minimumDelayTime(), customDelay);
    EXPECT_EQ(clock.getCyclesInDelayTime(), static_cast<uint64_t>(customDelay.count()) * 2 * 1000);
}

TEST_F(ClockTests, BurstCyclesTriggersDelay) {
    BusClock_t clock(4);
    uint64_t burstCycles = clock.getCyclesInDelayTime() * 2 + 123;
    clock.enableTimingEmulation();

    bool didDelay = clock.delay(burstCycles);
    EXPECT_TRUE(didDelay);
    EXPECT_EQ(clock.getAccumulatedCycles(), burstCycles % clock.getCyclesInDelayTime());
}

TEST_F(ClockTests, ZeroCyclesNoDelay) {
    BusClock_t clock(1);
    clock.enableTimingEmulation();

    bool didDelay = clock.delay(0);
    EXPECT_FALSE(didDelay);
    EXPECT_EQ(clock.getAccumulatedCycles(), 0);
}

TEST_F(ClockTests, ToggleEmulationState) {
    BusClock_t clock(4);
    clock.enableTimingEmulation();
    clock.delay(100);
    clock.disableTimingEmulation();
    EXPECT_EQ(clock.getAccumulatedCycles(), 0);
    bool didDelay = clock.delay(clock.getCyclesInDelayTime()*2);
    EXPECT_FALSE(didDelay);
    clock.enableTimingEmulation();
    EXPECT_EQ(clock.getAccumulatedCycles(), 0);
}
