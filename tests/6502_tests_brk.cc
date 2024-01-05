//
// Tests for bkr instruction
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
	Cycles_t UsedCycles, ExpectedCycles;
	Byte ins = Opcodes::INS_BRK_IMP;
	Word pushed_PC = CPU::RESET_VECTOR + 2;
	constexpr Word STACK_FRAME = 0x0100 | CPU::INITIAL_SP;

	//Given:
	cpu.TestReset(CPU::RESET_VECTOR);
	mem[0xFFFC] = ins;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x60;

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x6000);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 3);
	EXPECT_EQ(mem[STACK_FRAME-1], pushed_PC & 0xff);
	EXPECT_EQ(mem[STACK_FRAME], pushed_PC >> 8);
	EXPECT_TRUE(cpu.getFlagB());
	EXPECT_TRUE(cpu.getFlagI());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}
