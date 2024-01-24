//
// Tests invalid instruction handling for 6502 only
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


TEST_F(testClass, InvalidOpcodeThrowsException) {
    bool caughtRuntimeException = false;
    bool caughtOtherException = false;
    Cycles_t UsedCycles, ExpectedCycles;
    Byte ins = 0x33;  // TODO: Really need a way to pick an unused address programmatically. 

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
