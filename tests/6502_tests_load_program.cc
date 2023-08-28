#include <gtest/gtest.h>
#include "../6502.h"

std::vector<Byte> testProgram = {
	0xA9, 0xFF, 0x85, 0x90, 0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x00, 0x40
};

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
static const char *testProgramFile = "./binfiles/simpleprg.bin";

class MOS6502LoadProgramTests : public testing::Test {
public:	
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
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
	Byte UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.loadData(testProgram, startAddress);
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

	mem.loadDataFromFile(testProgramFile, startAddress);
	cpu.PC = startAddress;

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.ExecuteOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}


TEST_F(MOS6502LoadProgramTests, TestLoadAProgramAndTrace)
{
	Byte UsedCycles, ExpectedCycles;
	// Given:
	constexpr unsigned long loops = 2;

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.PC = startAddress;

	//Then:
	for (unsigned long l = 0; l < sizeof(testProgram) * loops; l++) {
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.TraceOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

