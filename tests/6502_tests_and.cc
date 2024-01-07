//
// Tests for and instruction
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


class MOS6502ANDTests : public testing::Test {
public:
	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502ANDTests, AndImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x0F);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndImmediateSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.setA(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ZP;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.setA(0xff); 

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ZPX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(0x80);
	mem[0x81] = 0x0F;
	cpu.setA(0xFF);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ABS;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ABX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x10);
	mem[0x2010] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteXCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ABX;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x10);
	mem[0x20FF + 0x10] = 0x0F;
	cpu.setA(0xff); 

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndAbsoluteY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ABY;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x10);
	mem[0x2010] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502ANDTests, AndAbsoluteYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_ABY;
	
	// Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x10);
	mem[0x20FF + 0x10] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles);
}

TEST_F(MOS6502ANDTests, AndIndirectY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::AND_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.setY(0x10);
	mem[0x2000 + 0x10] = 0x0F;
	cpu.setA(0xff);

	// When: 
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);	

	// Then
	EXPECT_EQ(cpu.getA(), 0x0f);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles);

}
