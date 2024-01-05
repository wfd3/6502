//
// Tests for dec, dex and dey instructions
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

class MOS6502DECTests : public testing::Test {
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
TEST_F(MOS6502DECTests, DecZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ZP;

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
	EXPECT_EQ(mem[0x0020], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ZPX;

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
	EXPECT_EQ(mem[0x0040], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ABS;

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
	EXPECT_EQ(mem[0x2000], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ABX;

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
	EXPECT_EQ(mem[0x2002], 0x01);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x01;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0x00);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DecZeroPageSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEC_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x00;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// DEX
TEST_F(MOS6502DECTests, DexImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(10);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(1);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DexImmediateSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEX_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// DEY
TEST_F(MOS6502DECTests, DeyImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(10);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 9);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(1);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502DECTests, DeyImmediateSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_DEY_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setY(0);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), 0xff);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
