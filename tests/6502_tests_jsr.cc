#include <gtest/gtest.h>
#include <6502.h>

class MOS6502JSRTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502JSRTests, JsrAbsolute) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_JSR_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x21;
	mem[0xFFFE] = 0x43;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x4321);
	EXPECT_EQ(mem[0x01FE], 0xFC+2);
	EXPECT_EQ(mem[0x01FF], 0xFF);	
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
