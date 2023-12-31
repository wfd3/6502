//
// Tests for invalid instruction exception
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

class MOS6502OpcodeTests : public testing::Test {
public:

        Memory<Address_t, Byte> mem{MOS6502::MAX_MEM};
        MOS6502 cpu{mem};

        virtual void SetUp() {
                mem.mapRAM(0, MOS6502::MAX_MEM);
        }
        
        virtual void TearDown() {
        }
};

TEST_F(MOS6502OpcodeTests, InvalidOpcodeThrowsException) {
    bool caughtRuntimeException = false;
    bool caughtOtherException = false;
    Cycles_t UsedCycles, ExpectedCycles;
    Byte ins = 0xff;  

    //Given:
    cpu.TestReset(MOS6502::RESET_VECTOR);
    mem[0xFFFC] = ins;

    try {
        //When:
        cpu.executeOneInstructionWithCycleCount(UsedCycles, ExpectedCycles);
    }

    catch ([[maybe_unused]] const std::runtime_error& e) {
        caughtRuntimeException = true;
    }

    catch (...) {
        caughtOtherException = true;
    }

    EXPECT_TRUE(caughtRuntimeException);
    EXPECT_FALSE(caughtOtherException);
}
