//
// Tests that we can load and run programs 
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

#if !defined(testClass) 
# error "Macro 'testClass' not defined"
#endif

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
constexpr Word startAddress = 0x4000;
static const char *testProgramFile = BINFILE_PATH "/sampleprg.bin";

TEST_F(testClass, TestLoadProgram) {
	// Given:

	// When:
	mem.loadData(testProgram, startAddress);

	//then:
	for (Word i = 0; i < programLen; i++) 
		EXPECT_EQ(mem[startAddress + i ], testProgram[i]);
}


TEST_F(testClass, TestLoadAProgramAndRun)
{
	

	// Given:

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		cpu.execute();
		EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
	}
}

TEST_F(testClass, TestLoadAProgramFromAFileAndRun)
{
	

	// Given:

	// When:

	mem.loadDataFromFile(testProgramFile, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		cpu.execute();
		EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
	}
}


TEST_F(testClass, TestLoadAProgramAndTrace)
{
	
	// Given:
	constexpr unsigned long loops = 2;

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.TestReset(startAddress);

	//Then:
	for (unsigned long l = 0; l < sizeof(testProgram) * loops; l++) {
			cpu.traceOneInstruction();
		EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
	}
}

#ifdef TEST_LOOP_DETECTION

std::vector<Byte> testLoopDetectionProgram = {
	0x4c, 0x00, 0x40  // JMP #4000
};

TEST_F(testClass, TestLoopDetection)
{
	mem.loadData(testLoopDetectionProgram, startAddress);
	cpu.TestReset(startAddress);
	cpu.loopDetection(true);

	Word cycles = 1000;
	while (--cycles && !cpu.loopDetected()) {
		cpu.execute();
		EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
		EXPECT_EQ(cpu.getPC(), startAddress);
	}
	EXPECT_TRUE(cpu.loopDetected());
	EXPECT_EQ(cpu.getPC(), startAddress);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}
#endif