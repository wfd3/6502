//
// Tests for the rti instruction for 6502 only
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

constexpr Byte START_SP_ADDRESS = 0xff;

TEST_F(testClass, RtiImplied) {
	
	Byte ins = cpu.Opcodes.RTI_IMP;

	//Given:
	cpu.TestReset(0x2000, START_SP_ADDRESS - 3);
	mem[0x2000] = ins;
	mem[0x01FF] = 0xAA;
	mem[0x01FE] = 0xFF;
	mem[0x01FD] = 0x00;
	cpu.setPS(0xff);
	cpu.setFlagB(true);
	cpu.setFlagC(true);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPC(), 0xAAFF);
	EXPECT_EQ(cpu.getSP(), START_SP_ADDRESS);
	EXPECT_FALSE(cpu.getFlagB());
	EXPECT_FALSE(cpu.getFlagC());
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}
