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

void testClass::BranchesWhenFlagSet(Word saddr, Byte rel, Byte ins) {
	

	//Given:
	mem[saddr]   = ins;
	mem[saddr+1] = rel;

	Word result = (saddr + 2) + SByte(rel);

	//When:
	cpu.execute();

	// Then:
	EXPECT_EQ(cpu.getPC(), result);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
}

// BCC
TEST_F(testClass, BCCBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BCC_REL);
}
	
TEST_F(testClass, BCCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BCC_REL);
}
	
TEST_F(testClass, BCCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BCC_REL);
}

TEST_F(testClass, BCCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BCC_REL);
}

// BCS
TEST_F(testClass, BCSBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BCS_REL);
}
	
TEST_F(testClass, BCSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BCS_REL);
}
	
TEST_F(testClass, BCSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BCS_REL);
}

TEST_F(testClass, BCSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BCS_REL);
}

// BEQ
TEST_F(testClass, BEQBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BEQ_REL);
}
	
TEST_F(testClass, BEQBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BEQ_REL);
}
	
TEST_F(testClass, BEQBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BEQ_REL);
}

TEST_F(testClass, BEQDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BEQ_REL);
}

// BMI
TEST_F(testClass, BMIBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BMI_REL);
}
	
TEST_F(testClass, BMIBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BMI_REL);
}
	
TEST_F(testClass, BMIBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BMI_REL);
}

TEST_F(testClass, BMIDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BMI_REL);
}

// BNE
TEST_F(testClass, BNEBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BNE_REL);
}
	
TEST_F(testClass, BNEBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BNE_REL);
}
	
TEST_F(testClass, BNEBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BNE_REL);
}

TEST_F(testClass, BNEDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BNE_REL);
}

// BPL
TEST_F(testClass, BPLBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BPL_REL);
}
	
TEST_F(testClass, BPLBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BPL_REL);
}
	
TEST_F(testClass, BPLBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BPL_REL);
}

TEST_F(testClass, BPLDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BPL_REL);
}

// BVC
TEST_F(testClass, BVCBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BVC_REL);
}
	
TEST_F(testClass, BVCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BVC_REL);
}
	
TEST_F(testClass, BVCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BVC_REL);
}

TEST_F(testClass, BVCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BVC_REL);
}

// BVS
TEST_F(testClass, BVSBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, cpu.Opcodes.BVS_REL);
}
	
TEST_F(testClass, BVSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(0x2000, 0xF, cpu.Opcodes.BVS_REL);
}
	
TEST_F(testClass, BVSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(0x20F3, 0xf, cpu.Opcodes.BVS_REL);
}

TEST_F(testClass, BVSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, cpu.Opcodes.BVS_REL);
}
