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
#include <65C02.h>

class MOS65C02LDTests : public testing::Test {
public:

	Memory<Word, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};
	
	enum class Registers {
		A,
		X,
		Y
	};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
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

#define testClass MOS65C02LDTests
#include "lda_ldx_ldy_tests.cc"

TEST_F(testClass, STAZeroPageIndirect) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	mem[0x20] = 0x10;
	mem[0x21] = 0x10;
	cpu.setA(0x52);
	TestST(cpu.Opcodes.STA_ZPI, 0x1010, Registers::A);
}

TEST_F(testClass, LDAZeroPageIndirect) {
	cpu.TestReset(MOS6502::RESET_VECTOR);
	
	mem[0xFFFD] = 0x20;
	mem[0x0020] = 0x10;
	mem[0x0021] = 0x10;
	mem[0x1010] = 0x0f;
	TestLD(cpu.Opcodes.LDA_ZPI, Registers::A);
}