//
// Tests LSR instruction
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

TEST_F(testClass, Nop) {
	
	Byte ins = cpu.Opcodes.NOP_IMP;

	//Given:
	cpu.TestReset(MOS6502::RESET_VECTOR);

	mem[0xFFFC] = ins;
	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}
