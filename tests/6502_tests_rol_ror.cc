//
// Tests for rotate instructions (ror and rol)
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

class MOS6502RORROLTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502RORROLTests, RolAccumulator) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAccumulatorSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], data << 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RolAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROL_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], Byte(data << 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// ROR
TEST_F(MOS6502RORROLTests, RorAccumulator) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ACC;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorSetsCarryFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ACC;
	Byte data = 0b10000001;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(data); 

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAccumulatorClearsCarryAndSetsNegativeFlag) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ACC;
	Byte data = 0b10000000;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	cpu.setA(data); 
	cpu.setFlagC(true);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_TRUE(cpu.getFlagN());
	EXPECT_EQ(cpu.getA(), Byte(data >> 1) | (1 << 7));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPage) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ZP;
	Byte data = 0b01010101;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x20;
	mem[0x0020] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorZeroPageX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ZPX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x0020], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAbsolute) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ABS;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2000], data >> 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502RORROLTests, RorAbsoluteX) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_ROR_ABX;
	Byte data = 0b01010101;
	
	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x05);
	mem[0x2005] = data;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_TRUE(cpu.getFlagC());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(mem[0x2005], Byte(data >> 1));
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
