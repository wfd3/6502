//
// Tests for push and pop instructions (pha, php, pla, plp)
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

class MOS6502PushPopTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502PushPopTests, PhaImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_PHA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFC] = ins;
	cpu.setA(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(mem[0X01FF], 0x52);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PlaImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_PLA_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, CPU::INITIAL_SP - 1);
	
	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setA(0xff);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getA(), 0x52);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PhpImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_PHP_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0x52;
	cpu.setPS(0b01010101);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(mem[0x01FF], 0b01110101);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 1);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

TEST_F(MOS6502PushPopTests, PlpImmediate) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_PLP_IMP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR, CPU::INITIAL_SP - 1);

	mem[0xFFFC] = ins;
	mem[0x01FF] = 0b01010101;;
	cpu.setPS(0x52);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPS(), 0b01000101);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
