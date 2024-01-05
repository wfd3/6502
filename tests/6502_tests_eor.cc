//
// Tests for eor instruction
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

class MOS6502EORTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502EORTests, EorImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_ZP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	mem[0x0001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_ZPX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.setX(1);
	mem[0x0002] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_ABS;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_ABX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorAbsoluteY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_ABY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorIndirectX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_IDX;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	mem[0x2000] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorIndirectY) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_IDY;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;

	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	cpu.setY(1);
	mem[0x2001] = 0x01;
	cpu.setA(0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x01);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorImmediateWhenZeroFlagShouldBeSet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0x0f);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x00);
	EXPECT_TRUE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502EORTests, EorImmediateWhenNegativeFlagShouldBeSet) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_EOR_IMM;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.setA(0xf0);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0xFF);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_TRUE(cpu.getFlagN()) ;
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

