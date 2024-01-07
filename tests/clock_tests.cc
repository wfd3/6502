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
	virtual void SetUp() {}
	
	virtual void TearDown()	{}
};

static constexpr bool VERBOSE          = false;
static constexpr int variance          = 200;
static constexpr int minimumResolution = 200;

void runClockTest(uint64_t MHz, int cycles = 1) {
    uint64_t nsPerCycle = 1000 / MHz;

    // This delay method bottoms out at about 220ns, so to keep the tests passing as the 
    // clock frequency increases we just floor the lowerBound at minimumResolution.
    int64_t lb = (nsPerCycle - variance) * cycles;
    if (lb < minimumResolution) 
        lb = minimumResolution;

    std::chrono::duration<uint64_t, std::nano> lowerBound(lb);
    std::chrono::duration<uint64_t, std::nano> upperBound((nsPerCycle + variance) * cycles);

    BusClock_t clock(MHz);
    clock.enableTimingEmulation();
    
    auto start = std::chrono::high_resolution_clock::now();
    clock.delay(cycles);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = (stop - start); 

    EXPECT_GE(duration, lowerBound);
    EXPECT_LE(duration, upperBound);

    if (VERBOSE || 
        ::testing::UnitTest::GetInstance()->current_test_info()->result()->Failed()) {
        std::cout << "clock.delay(" << cycles << ") at " << MHz << "MHz expected a " <<
            nsPerCycle * cycles << "ns delay, actually delayed " << duration << std::endl;
    }
}

TEST_F(ClockTests, CanGetClockFreqency) {
    static constexpr uint16_t _MHz = 4;

    BusClock_t clock(_MHz);
    EXPECT_EQ(clock.getFrequencyMHz(), _MHz);
}

TEST_F(ClockTests, TestDelayAt1MHz) {
    runClockTest(1);    
}

TEST_F(ClockTests, TestDelayFor50CyclesAt1MHz) {
    runClockTest(1, 50);    
}

TEST_F(ClockTests, TestDelayFor50000CyclesAt1MHz) {
    runClockTest(1, 50000);    
}

TEST_F(ClockTests, TestDelayFor4CyclesAt1MHz) {
    runClockTest(1, 4);    
}

TEST_F(ClockTests, TestDelayAt2MHz) {
    runClockTest(2);
}

TEST_F(ClockTests, TestDelayAt3MHz) {
    runClockTest(3);
}

TEST_F(ClockTests, TestDelayAt4MHz) {
    runClockTest(4);
}

TEST_F(ClockTests, TestDelayAt5MHz) {
    runClockTest(5);
}

TEST_F(ClockTests, TestDelayAt6MHz) {
    runClockTest(6);
}
