#include <gtest/gtest.h>
#include "../6502.h"

static Byte testProgram[] = {
	0xA9, 0xFF, 0x85, 0x90, 0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x00, 0x40 };
///
// Test Program
//
// $4000: lda #$FF
// $4001: sta $90
// $4002: sta $8000
// $4003: eor #$CC
// $4004: jmp #$4000
//

static const unsigned long programLen = 12;
constexpr Address_t startAddress = 0x4000;
static char *testProgramFile = "./tests/program.bin";

class MOS6502LoadProgramTests : public testing::Test {
public:	
	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.Reset(CPU::INITIAL_PC);
		mem.Init();
	}

	virtual void TearDown() {
	}
};

TEST_F(MOS6502LoadProgramTests, TestLoadProgram) {
	// Given:

	// When:
	mem.LoadProgram(testProgram, startAddress, programLen);

	//then:
	for (Address_t i = 0; i < programLen; i++) 
		EXPECT_EQ(mem[startAddress + i ], testProgram[i]);
}


TEST_F(MOS6502LoadProgramTests, TestLoadAProgramAndRun)
{
	Byte UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.LoadProgram(testProgram, startAddress, programLen);
	cpu.PC = startAddress;

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.ExecuteOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

TEST_F(MOS6502LoadProgramTests, TestLoadAProgramFromAFileAndRun)
{
	Byte UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.LoadProgramFromFile(testProgramFile, startAddress);
	cpu.PC = startAddress;

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.ExecuteOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}
