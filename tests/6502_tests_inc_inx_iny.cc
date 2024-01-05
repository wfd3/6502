//
// Tests for inc, inx and iny instructions
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

class MOS6502INCTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

// DEC 
TEST_F(MOS6502INCTests, IncZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x02;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ZPX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setX(0x20);
	mem[0x0040] = 0x02;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0040], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ABS;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x02;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ABX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x02);
	mem[0x2002] = 0x02;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2002], 0x03);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0xff;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, IncZeroPageSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x7f;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x80);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// INX
TEST_F(MOS6502INCTests, InxImplied) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(0x09);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(0xff);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InxImpliedSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setX(127);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// INY
TEST_F(MOS6502INCTests, InyImplied) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(0x09);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0x0a);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0xff);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502INCTests, InyImpliedSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_INY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(127);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 128);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
