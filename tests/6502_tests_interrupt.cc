//
// Tests for external interrupts
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
#include <thread>

#ifdef _WIN64
#include <Windows.h>

void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;

	// Convert to 100 nanosecond interval, negative value
	// indicates relative time
	ft.QuadPart = -(10 * usec); 

	timer = CreateWaitableTimer(nullptr, TRUE, nullptr);
	SetWaitableTimer(timer, &ft, 0, nullptr, nullptr, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
#endif

class MOS6502InterruptTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}

	bool executeOneInstruction() {
		Cycles_t used;
		bool halt, debug;
		cpu.execute(halt, debug, used);
		return halt;
	}

	void execute() {
		while(!executeOneInstruction())
			;
	}
};

std::vector<Byte> interruptTestProgram = {
	0xca,					// 1000: dex
	0xc8, 					//       iny
	0xca, 					//       dex
	0xc8, 					//       iny
	0xca, 					//       dex
	0xc8, 					//       iny
	0x4c, 0x00, 0x10		//		 jmp #$1000
};

TEST_F(MOS6502InterruptTests, InlineMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.getFlagI());

	// When
	executeOneInstruction();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502InterruptTests, InlineMaskableInterruptDoesNotInterruptWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.setFlagI(true);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_TRUE(cpu.getFlagI());

	// When
	executeOneInstruction();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1001);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP);
	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502InterruptTests, MaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());

	// When
	std::thread runProgram(&MOS6502InterruptTests_MaskableInterrupt_Test::execute, this);
	usleep(250);
	cpu.raiseIRQ();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502InterruptTests, NonMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);

	// When
	std::thread runProgram(&MOS6502InterruptTests_MaskableInterrupt_Test::execute, this);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502InterruptTests, NonMaskableInterruptWorksEvenWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.setFlagI(true);

	// When
	std::thread runProgram(&MOS6502InterruptTests_MaskableInterrupt_Test::execute, this);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(MOS6502InterruptTests, MaskableInterruptFollowedByRTSWorks) {

	// 1000 loop: dex
	// 1001       cpy #0
	// 1003       bne loop
	// 1005       dex
	std::vector<Byte> thisProgram = { 0xca, 0xc0, 0x00, 0xd0, 0xfb, 0xca };

	// 4000       ldy #0
	// 4002       rti
	std::vector<Byte> rtiProgram  = { 0xa0, 0x00, 0x40 };

	//Given:
	mem.loadData(thisProgram, 0x1000);
	cpu.TestReset(0x1000);
	cpu.setHaltAddress(0x1005);
	cpu.setInterruptVector(0x4000);
	mem.loadData(rtiProgram, 0x4000);

	// When
	cpu.raiseIRQ();
	std::thread runProgram(&MOS6502InterruptTests_MaskableInterrupt_Test::execute, this);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1005);
	EXPECT_EQ(cpu.getSP(), CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_FALSE(cpu.getFlagI());
}
