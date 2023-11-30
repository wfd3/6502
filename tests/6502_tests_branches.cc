#include <gtest/gtest.h>
#include <6502.h>

class MOS6502BranchTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
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
	EXPECT_EQ(cpu.PC, result);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCC
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BCC_REL);
}

TEST_F(MOS6502BranchTests, BCCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BCC_REL);
}

// BCS
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BCS_REL);
}

TEST_F(MOS6502BranchTests, BCSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BCS_REL);
}

// BEQ
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BEQ_REL);
}


TEST_F(MOS6502BranchTests, BEQDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BEQ_REL);
}

// BMI
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BMI_REL);
}

TEST_F(MOS6502BranchTests, BMIDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BMI_REL);
}

// BNE
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BNE_REL);
}
	
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BNE_REL);
}
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BNE_REL);
}

TEST_F(MOS6502BranchTests, BNEDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BNE_REL);
}

// BPL
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BPL_REL);
}
	
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BPL_REL);
}
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BPL_REL);
}

TEST_F(MOS6502BranchTests, BPLDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BPL_REL);
}

// BVC
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BVC_REL);
}


TEST_F(MOS6502BranchTests, BVCDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BVC_REL);
}

// BVS
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, Opcodes::INS_BVS_REL);
}
	
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetForward) {
	cpu.TestReset(0x2000);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(0x2000, 0xF, Opcodes::INS_BVS_REL);
}
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetAcrossPage) {
	cpu.TestReset(0x20F3);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, Opcodes::INS_BVS_REL);
}


TEST_F(MOS6502BranchTests, BVSDoesNotBranchesWhenVIsNotSet) {
	cpu.TestReset(CPU::RESET_VECTOR);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, Opcodes::INS_BVS_REL);
}
