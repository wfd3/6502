//
// Tests for stx, sty, tax,tay, stx, txa, txa and tya instructions
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

class MOS6502TXTYTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

// TAX

TEST_F(MOS6502TXTYTests, TAX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0xAB);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAXSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXA

TEST_F(MOS6502TXTYTests, TXA) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setX(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TXASetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TXA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xff);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TAY

TEST_F(MOS6502TXTYTests, TAY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0);
	cpu.setY(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TAYSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TAY_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0xff);
	cpu.setY(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TYA

TEST_F(MOS6502TXTYTests, TYA) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);
	cpu.setY(0x0b);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TYASetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TYA_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setY(0xff);
	cpu.setA(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getY(), cpu.getA());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TSX

TEST_F(MOS6502TXTYTests, TSX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsZeroFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502TXTYTests, TSXSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TSX_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0xff);
	
	mem[0xFFFC] = ins;
	cpu.setX(0xab);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// TXS

TEST_F(MOS6502TXTYTests, TXS) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_TXS_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR, 0x12);
	
	mem[0xFFFC] = ins;
	cpu.setX(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getX(), cpu.getSP());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
