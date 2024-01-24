//
// Tests for Rockwell 65C02 variant instructions RMB and SMB
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

class MOS65C02RMB_SMBTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}

	bool testRMB(Byte ins, uint8_t bit) {
		Cycles_t UsedCycles, ExpectedCycles;
		Byte mask = ~(1 << bit);
		const Byte value = 0xff;
		Byte result = value & mask;

		//Given:
		cpu.TestReset(MOS6502::RESET_VECTOR);

		mem[0xfffc] = ins;
		mem[0xfffd] = 0x10;
		mem[0x0010] = value;

		//When:
		cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

		// Then:
		EXPECT_EQ(mem[0x0010], result);

		EXPECT_FALSE(cpu.getFlagZ());
		EXPECT_FALSE(cpu.getFlagV());
		EXPECT_FALSE(cpu.getFlagN());
		EXPECT_FALSE(cpu.getFlagC());
		EXPECT_EQ(UsedCycles, ExpectedCycles);  

		return !::testing::UnitTest::GetInstance()->current_test_info()->result()->Failed();
	}

	bool testSMB(Byte ins, uint8_t bit) {
		Cycles_t UsedCycles, ExpectedCycles;
		Byte mask = 1 << bit;
		const Byte value = 0xff;
		Byte result = 0xff | mask;

		//Given:
		cpu.TestReset(MOS6502::RESET_VECTOR);

		mem[0xfffc] = ins;
		mem[0xfffd] = 0x10;
		mem[0x0010] = value;

		//When:
		cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);

		// Then:
		EXPECT_EQ(mem[0x0010], result);

		EXPECT_FALSE(cpu.getFlagZ());
		EXPECT_FALSE(cpu.getFlagV());
		EXPECT_FALSE(cpu.getFlagN());
		EXPECT_FALSE(cpu.getFlagC());
		EXPECT_EQ(UsedCycles, ExpectedCycles);  

		return !::testing::UnitTest::GetInstance()->current_test_info()->result()->Failed();
	}

	
};

// RMB
TEST_F(MOS65C02RMB_SMBTests, RMB0Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB0, 0));
}

TEST_F(MOS65C02RMB_SMBTests, RMB1Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB1, 1));
}

TEST_F(MOS65C02RMB_SMBTests, RMB2Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB2, 2));
}

TEST_F(MOS65C02RMB_SMBTests, RMB3Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB3, 3));
}

TEST_F(MOS65C02RMB_SMBTests, RMB4Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB4, 4));
}

TEST_F(MOS65C02RMB_SMBTests, RMB5Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB5, 5));
}

TEST_F(MOS65C02RMB_SMBTests, RMB6Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB6, 6));
}

TEST_F(MOS65C02RMB_SMBTests, RMB7Works) {
    EXPECT_TRUE(testRMB(cpu.Opcodes.RMB7, 7));
}

// SMB
TEST_F(MOS65C02RMB_SMBTests, SMB0Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB0, 0));
}

TEST_F(MOS65C02RMB_SMBTests, SMB1Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB1, 1));
}

TEST_F(MOS65C02RMB_SMBTests, SMB2Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB2, 2));
}

TEST_F(MOS65C02RMB_SMBTests, SMB3Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB3, 3));
}

TEST_F(MOS65C02RMB_SMBTests, SMB4Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB4, 4));
}

TEST_F(MOS65C02RMB_SMBTests, SMB5Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB5, 5));
}

TEST_F(MOS65C02RMB_SMBTests, SMB6Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB6, 6));
}

TEST_F(MOS65C02RMB_SMBTests, SMB7Works) {
    EXPECT_TRUE(testSMB(cpu.Opcodes.SMB7, 7));
}
