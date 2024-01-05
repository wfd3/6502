//
// Tests for loading data and programs into memory 
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
#include <6502.h>

std::vector<Byte> testProgram = {
	0xA9, 0xFF, 0x85, 0x90, 0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x00, 0x40
};

///
// Test Program
//      LDA #$00
//      STA $C000
//      LDX #$00
//      LDY #$00
//loop: INX
//      BNE loop
//      INY     
//      BNE loop
//      LDA $C000
//      CLC
//      ADC #$01
//      STA $C000
//      JMP loop

static const unsigned long programLen = 12;
constexpr Address_t startAddress = 0x4000;
static const char *testProgramFile = BINFILE_PATH "/sampleprg.bin";

class MOS6502LoadProgramTests : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}

	virtual void TearDown() {
	}
};

TEST_F(MOS6502LoadProgramTests, TestLoadProgram) {
	// Given:

	// When:
	mem.loadData(testProgram, startAddress);

	//then:
	for (Address_t i = 0; i < programLen; i++) 
		EXPECT_EQ(mem[startAddress + i ], testProgram[i]);
}


TEST_F(MOS6502LoadProgramTests, TestLoadAProgramAndRun)
{
	Cycles_t UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

TEST_F(MOS6502LoadProgramTests, TestLoadAProgramFromAFileAndRun)
{
	Cycles_t UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.loadDataFromFile(testProgramFile, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}


TEST_F(MOS6502LoadProgramTests, TestLoadAProgramAndTrace)
{
	Cycles_t UsedCycles, ExpectedCycles;
	// Given:
	constexpr unsigned long loops = 2;

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (unsigned long l = 0; l < sizeof(testProgram) * loops; l++) {
			cpu.traceOneInstruction(UsedCycles, ExpectedCycles);
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

