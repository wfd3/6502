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

class MOS65C02BBS_BBRTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}

	void testBranchIfBitSet(Byte ins, uint8_t bit, bool set, bool expectBranch, Byte zpAddress, Word startAddress, Byte offset) {
        

		// Given:
        cpu.TestReset(startAddress);
        mem[startAddress]   = ins;
        mem[startAddress+1] = zpAddress;
        mem[startAddress+2] = offset;

        if (set) 
            mem[zpAddress] = mem[zpAddress] | static_cast<Byte>(1 << bit);
        else 
            mem[zpAddress] = mem[zpAddress] & static_cast<Byte>(~(1 << bit));

        Word result = startAddress + 3;
        if (expectBranch) 
            result += SByte(offset);

        //When:
        cpu.execute();
       
        // Then:
        EXPECT_EQ(cpu.getPC(), result);
        EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles()); 
    }

};

#define setBit  true
#define dontSetBit  false
#define expectingBranch  true
#define notExpectingBranch  false

// BBS
TEST_F(MOS65C02BBS_BBRTests, BBS0BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS0, 0, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS1BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS1, 1, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS2BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS2, 2, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS3BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS3, 3, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS4BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS4, 4, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS5BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS5, 5, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS6BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS6, 6, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS7BranchesIfBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS7, 7, setBit, expectingBranch, 0x10, 0x1000, 0x20);
}

// BBS - don't expect to branch
TEST_F(MOS65C02BBS_BBRTests, BBS0DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS0, 1, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS1DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS1, 2, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS2DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS2, 3, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS3DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS3, 3, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS4DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS4, 4, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS5DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS5, 5, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS6DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS6, 6, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBS7DoesNotBranchIfBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBS7, 7, dontSetBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

//BBR - expect to branch
TEST_F(MOS65C02BBS_BBRTests, BBR0DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR0, 0, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR1DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR1, 1, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR2DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR2, 2, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR3DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR3, 3, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR4DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR4, 4, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR5DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR5, 5, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR6DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR6, 6, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR7DoesBranchesWhenBitNotSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR7, 7, dontSetBit, expectingBranch, 0x10, 0x1000, 0x20);
}

//BBR - don't expect to branch
TEST_F(MOS65C02BBS_BBRTests, BBR0DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR0, 0, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR1DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR1, 1, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR2DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR2, 2, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR3DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR3, 3, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR4DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR4, 4, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR5DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR5, 5, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR6DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR6, 6, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}

TEST_F(MOS65C02BBS_BBRTests, BBR7DoesDoesNotBranchWhenBitSet) {
    testBranchIfBitSet(cpu.Opcodes.BBR7, 7, setBit, notExpectingBranch, 0x10, 0x1000, 0x20);
}
