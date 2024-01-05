//
// Tests for cmp instruction 
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

class MOS6502CMPTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502CMPTests, CmpImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setA(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setA(0x0f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setA(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setA(0x10);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setA(0x20);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ZPX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	cpu.setX(0x01);
	mem[0x0001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x01);
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteXCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ABX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.setX(0xff);
	mem[0x210f] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x01);
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpAbsoluteYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_ABY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0xFFFE] = 0x20;
	cpu.setY(0xff);
	mem[0x210f] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IDX;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(0x01);
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2001] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.setY(0x01);
	mem[0x0002] = 0x01;
	mem[0x0000] = 0x20;
	mem[0x2002] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CmpIndirectYCrossesPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CMP_IDY;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x02;
	cpu.setY(0xff);
	mem[0x0002] = 0x10;
	mem[0x0000] = 0x20;
	mem[0x210F] = 0x0F;
	cpu.setA(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPX
TEST_F(MOS6502CMPTests, CpxImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setX(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf;
	cpu.setX(0x0e);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setX(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x2F;
	cpu.setX(0x1f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setX(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpxAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPX_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setX(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// CPY
TEST_F(MOS6502CMPTests, CpyImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	cpu.setY(0x30);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsCFlagFalse) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xf;
	cpu.setY(0x0e);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsZFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	cpu.setY(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyImmediateSetsNFlagTrue) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_IMM;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x3F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_ZP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x0F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502CMPTests, CpyAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_CPY_ABS;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);	

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.setY(0x2f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
