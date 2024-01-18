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
#include <65C02.h>

class MOS65C02OpcodeTests : public testing::Test {
public:

        Memory<Address_t, Byte> mem{MOS65C02::MAX_MEM};
        MOS65C02 cpu{mem};

        virtual void SetUp() {
                mem.mapRAM(0, MOS65C02::MAX_MEM);
        }
        
        virtual void TearDown() {
        }
};

#define testClass MOS65C02OpcodeTests
#include "invalid_instruction_tests.cc"