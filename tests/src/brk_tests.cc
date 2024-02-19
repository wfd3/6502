//
// Tests for asl instruction for 6502 only
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

#if !defined(testClass) 
# error "Macro 'testClass' not defined"
#endif

TEST_F(testClass, BRKImplied) {
	
	Byte ins = cpu.Opcodes.BRK_IMP;
	Word pushed_PC = MOS6502::RESET_VECTOR + 2;
	constexpr Word STACK_FRAME = 0x0100;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	Word initialSP = cpu.getSP();
	Word initialStackAddr = STACK_FRAME | initialSP;

	mem[0xFFFC] = ins;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x60;

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPC(), 0x6000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_EQ(mem[initialStackAddr-1], pushed_PC & 0xff);
	EXPECT_EQ(mem[initialStackAddr], pushed_PC >> 8);
	EXPECT_TRUE(cpu.getFlagB());
	EXPECT_TRUE(cpu.getFlagI());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
