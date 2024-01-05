//
// Tests for branch instructions (bcc, bcs, beq, bm1, bpl)
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

class MOS6502BranchTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
	MOS6502 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS6502::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}

	void BranchesWhenFlagSet(Word, Byte, Byte);
};

void MOS6502BranchTests::BranchesWhenFlagSet(Word saddr, Byte rel, Byte ins) {
	Cycles_t UsedCycles, ExpectedCycles;

	//Given:
	mem[saddr]   = ins;
	mem[saddr+1] = rel;

	Word result = (saddr + 2) + SByte(rel);

	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	EXPECT_EQ(cpu.getPC(), result);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCC
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BCC_REL);
}

TEST_F(MOS6502BranchTests, BCCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BCC_REL);
}

// BCS
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagC(true);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BCS_REL);
}

TEST_F(MOS6502BranchTests, BCSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagC(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BCS_REL);
}

// BEQ
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BEQ_REL);
}

TEST_F(MOS6502BranchTests, BEQDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BEQ_REL);
}

// BMI
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BMI_REL);
}

TEST_F(MOS6502BranchTests, BMIDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BMI_REL);
}

// BNE
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BNE_REL);
}
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BNE_REL);
}
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagZ(false);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BNE_REL);
}

TEST_F(MOS6502BranchTests, BNEDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagZ(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BNE_REL);
}

// BPL
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BPL_REL);
}
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BPL_REL);
}
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagN(false);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BPL_REL);
}

TEST_F(MOS6502BranchTests, BPLDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagN(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BPL_REL);
}

// BVC
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BVC_REL);
}

TEST_F(MOS6502BranchTests, BVCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BVC_REL);
}

// BVS
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0xF, Opcodes::INS_BVS_REL);
}
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BVS_REL);
}
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.setFlagV(true);
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BVS_REL);
}

TEST_F(MOS6502BranchTests, BVSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	cpu.setFlagV(false);
	BranchesWhenFlagSet(MOS6502::RESET_VECTOR, 0x0, Opcodes::INS_BVS_REL);
}
