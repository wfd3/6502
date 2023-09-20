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
	uint64_t UsedCycles, ExpectedCycles;

	//Given:
	mem[saddr]   = ins;
	mem[saddr+1] = rel;

	Word result = (saddr + 2) + SByte(rel);

	//When:
	std::tie(UsedCycles, ExpectedCycles) = cpu.executeOneInstruction();

	// Then:
	EXPECT_EQ(cpu.PC, result);
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// BCC
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BCC_REL);
}
	
TEST_F(MOS6502BranchTests, BCCBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BCC_REL);
}

TEST_F(MOS6502BranchTests, BCCDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BCC_REL);
}

// BCS
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BCS_REL);
}
	
TEST_F(MOS6502BranchTests, BCSBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.C = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BCS_REL);
}

TEST_F(MOS6502BranchTests, BCSDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.C = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BCS_REL);
}

// BEQ
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BEQ_REL);
}
	
TEST_F(MOS6502BranchTests, BEQBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BEQ_REL);
}


TEST_F(MOS6502BranchTests, BEQDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BEQ_REL);
}

// BMI
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BMI_REL);
}
	
TEST_F(MOS6502BranchTests, BMIBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BMI_REL);
}

TEST_F(MOS6502BranchTests, BMIDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BMI_REL);
}

// BNE
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BNE_REL);
}
	
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BNE_REL);
}
	
TEST_F(MOS6502BranchTests, BNEBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.Z = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BNE_REL);
}

TEST_F(MOS6502BranchTests, BNEDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.Z = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BNE_REL);
}

// BPL
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BPL_REL);
}
	
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BPL_REL);
}
	
TEST_F(MOS6502BranchTests, BPLBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.N = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BPL_REL);
}

TEST_F(MOS6502BranchTests, BPLDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.N = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BPL_REL);
}

// BVC
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BVC_REL);
}
	
TEST_F(MOS6502BranchTests, BVCBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BVC_REL);
}


TEST_F(MOS6502BranchTests, BVCDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BVC_REL);
}

// BVS
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0xF, CPU::INS_BVS_REL);
}
	
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetForward) {
	cpu.Reset(0x2000);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(0x2000, 0xF, CPU::INS_BVS_REL);
}
	
TEST_F(MOS6502BranchTests, BVSBranchesWhenVSetAcrossPage) {
	cpu.Reset(0x20F3);
	cpu.Flags.V = 1;
	BranchesWhenFlagSet(0x20F3, 0xf, CPU::INS_BVS_REL);
}


TEST_F(MOS6502BranchTests, BVSDoesNotBranchesWhenVIsNotSet) {
	cpu.Reset(CPU::RESET_VECTOR);
	cpu.Flags.V = 0;
	BranchesWhenFlagSet(CPU::RESET_VECTOR, 0x0, CPU::INS_BVS_REL);
}
