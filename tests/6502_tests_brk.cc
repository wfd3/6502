#include <gtest/gtest.h>
#include <6502.h>

class MOS6502BRKTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502BRKTests, BRKImplied) {
	uint64_t UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_BRK_IMP;
	Word pushed_PC = CPU::RESET_VECTOR + 2;
	constexpr Word STACK_FRAME = 0x0100 | CPU::INITIAL_SP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	mem[0xFFFC] = ins;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x60;

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, 0x6000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_EQ(mem[STACK_FRAME-1], pushed_PC & 0xff);
	EXPECT_EQ(mem[STACK_FRAME], pushed_PC >> 8);
	EXPECT_TRUE(cpu.Flags.B);
	EXPECT_TRUE(cpu.Flags.I);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
