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
	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
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
	uint64_t UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.Reset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

TEST_F(MOS6502LoadProgramTests, TestLoadAProgramFromAFileAndRun)
{
	uint64_t UsedCycles, ExpectedCycles;

	// Given:

	// When:

	mem.loadDataFromFile(testProgramFile, startAddress);
	cpu.Reset(startAddress);

	//Then:
	for (Word cycles = 0; cycles < 1000; cycles++) 	{
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.executeOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}


TEST_F(MOS6502LoadProgramTests, TestLoadAProgramAndTrace)
{
	uint64_t UsedCycles, ExpectedCycles;
	// Given:
	constexpr unsigned long loops = 2;

	// When:

	mem.loadData(testProgram, startAddress);
	cpu.Reset(startAddress);

	//Then:
	for (unsigned long l = 0; l < sizeof(testProgram) * loops; l++) {
		std::tie(UsedCycles, ExpectedCycles) =
			cpu.traceOneInstruction();
		EXPECT_EQ(UsedCycles, ExpectedCycles);
	}
}

