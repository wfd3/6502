#include <gtest/gtest.h>
#include "../6502.h"

class MOS6502INCTests : public testing::Test {
public:

	Memory mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502INCTests, IncZeroPage) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ZPX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.X = 0x20;
	mem[0x0040] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0040], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsolute) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ABS;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2000], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsoluteX) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ABX;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x02;
	mem[0x2002] = 0x02;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x2002], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageSetsZeroFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0xff;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


TEST_F(MOS6502INCTests, IncZeroPageSetsNegativeFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INC_ZP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x7f;

	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(mem[0x0020], 0x80);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}


// INX
TEST_F(MOS6502INCTests, InxImplied) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 0x09;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsZeroFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 0xff;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsNegativeFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INX_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.X = 127;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.X, 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// INY
TEST_F(MOS6502INCTests, InyImplied) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 0x09;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.C);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsZeroFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 0xff;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsNegativeFlag) {
	Byte UsedCycles, ExpectedCycles;
	Byte ins = CPU::INS_INY_IMP;

	//Given:
	cpu.Reset(CPU::RESET_VECTOR);
	

	mem[0xFFFC] = ins;
	cpu.Y = 127;
	
	//When:
	std::tie(UsedCycles, ExpectedCycles) =
		cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_EQ(cpu.Y, 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
