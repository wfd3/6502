//
// Tests for lda, sta, ldx, stx, ldy and sty instructions
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

class MOS6502LDTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};
	
	enum class Registers {
		A,
		X,
		Y
	};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
	
	void TestLD(Byte, Registers);
	void TestST(Byte, Word, Registers);
	
	void setReg(Registers r, Byte val) {
		switch (r) {
		case Registers::A:
			cpu.setA(val);
			break;
		case Registers::X:
			cpu.setX(val);
			break;
		case Registers::Y:
			cpu.setY(val);
			break;
		}		
	}
	
	Byte getReg(Registers r) {
		switch (r) {
		case Registers::A:
			return cpu.getA();
		case Registers::X:
			return cpu.getX();
		case Registers::Y:
			return cpu.getY();
		}
		return 0;
	}
};

void MOS6502LDTests::TestLD(Byte ins, Registers r) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte reg;
	static constexpr Byte VAL = 0xff;

	//Given:
	mem[0xFFFC] = ins;
	setReg(r, VAL);
	
	//When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	// Then:
	reg = getReg(r);
	EXPECT_EQ(reg, 0x0F);
	EXPECT_FALSE(cpu.getFlagZ());
	EXPECT_FALSE(cpu.getFlagN());
	EXPECT_EQ(UsedCycles, ExpectedCycles); 
}

// Test STA, STX and STY
void MOS6502LDTests::TestST(Byte ins, Word addr, Registers r) {
	Cycles_t UsedCycles, ExpectedCycles;
	Byte reg = 0;

	//Given:
	mem[0xFFFC] = ins;

	// When:
	cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

	//Then:
	reg = getReg(r);
	EXPECT_EQ(mem[addr], reg);
}
	 
// LDA
TEST_F(MOS6502LDTests, LDAImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDA_IMM, Registers::A);
}

TEST_F(MOS6502LDTests, LDAZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDA_ZP, Registers::A);
}

TEST_F(MOS6502LDTests, LDAZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setX(0);
	TestLD(Opcodes::INS_LDA_ZPX, Registers::A);
}

TEST_F(MOS6502LDTests, LDAAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDA_ABS, Registers::A);
}

TEST_F(MOS6502LDTests, LDAAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0);
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDA_ABX, Registers::A);
}

// LDX
TEST_F(MOS6502LDTests, LDXImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDX_IMM, Registers::X);
}

TEST_F(MOS6502LDTests, LDXZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDX_ZP, Registers::X);
}


TEST_F(MOS6502LDTests, LDXZeroPageY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setY(0);
	TestLD(Opcodes::INS_LDX_ZPY, Registers::X);
}

TEST_F(MOS6502LDTests, LDXAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDX_ABS, Registers::X);
}

TEST_F(MOS6502LDTests, LDXAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0);
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABX, Registers::Y);
}

// LDY
TEST_F(MOS6502LDTests, LDYImmediate) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	TestLD(Opcodes::INS_LDY_IMM, Registers::Y);
}

TEST_F(MOS6502LDTests, LDYZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	TestLD(Opcodes::INS_LDY_ZP, Registers::Y);
}

TEST_F(MOS6502LDTests, LDYZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x0F;
	mem[0x000F] = 0x0F;
	cpu.setX(0);
	TestLD(Opcodes::INS_LDY_ZPX, Registers::Y);
}

TEST_F(MOS6502LDTests, LDYAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
		
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABS, Registers::Y);
}

TEST_F(MOS6502LDTests, LDYAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0);
	mem[0x2000] = 0x0F;
	TestLD(Opcodes::INS_LDY_ABX, Registers::Y);
}

// STA
TEST_F(MOS6502LDTests, STAAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_ABS, 0x2000, Registers::A);
}

TEST_F(MOS6502LDTests, STAZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_ZP, 0x0020, Registers::A);
}

TEST_F(MOS6502LDTests, STAZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x02);
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_ZPX, 0x0022, Registers::A);
}

TEST_F(MOS6502LDTests, STAAbsoluteX) {
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x02);
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_ABX, 0x2002, Registers::A);
}

TEST_F(MOS6502LDTests, STAAbsoluteY) {
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x02);
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_ABY, 0x2002, Registers::A);
}

TEST_F(MOS6502LDTests, STAIndirectX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x10;
	cpu.setX(0x10);
	mem[0x0020] = 0x00;
	mem[0x0021] = 0x20;
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_IDX, 0x2000, Registers::A);
}

TEST_F(MOS6502LDTests, STAIndirectY) {
	cpu.TestReset(CPU::RESET_VECTOR);

	mem[0xFFFD] = 0x00;
	mem[0x0000] = 0x00;
	mem[0x0001] = 0x20;
	cpu.setY(0x02);
	cpu.setA(0x52);
	TestST(Opcodes::INS_STA_IDY, 0x2002, Registers::A);
}

// STX
TEST_F(MOS6502LDTests, STXZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x52);
	TestST(Opcodes::INS_STX_ZP, 0x0020, Registers::X);
}

TEST_F(MOS6502LDTests, STXZeroPageY) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setY(0x02);
	cpu.setX(0x52);
	TestST(Opcodes::INS_STX_ZPY, 0x0022, Registers::X);
}

TEST_F(MOS6502LDTests, STXAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setX(0x52);
	TestST(Opcodes::INS_STX_ABS, 0x2000, Registers::X);
}

// STY
TEST_F(MOS6502LDTests, STYZeroPage) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setY(0x52);
	TestST(Opcodes::INS_STY_ZP, 0x0020, Registers::Y);
}

TEST_F(MOS6502LDTests, STYZeroPageX) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	cpu.setX(0x02);
	cpu.setY(0x52);
	TestST(Opcodes::INS_STY_ZPX, 0x0022, Registers::Y);
}

TEST_F(MOS6502LDTests, STYAbsolute) {
	cpu.TestReset(CPU::RESET_VECTOR);
	
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.setY(0x52);
	TestST(Opcodes::INS_STY_ABS, 0x2000, Registers::Y);
}
