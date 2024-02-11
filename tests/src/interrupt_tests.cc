//
// Tests interrupts for 6502 only
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

#define CONCATENATE(a, b) a ## b
#define CONCATENATE_INDIRECT(a, b) CONCATENATE(a, b)
#define CONCATENATE3(a, b, c) CONCATENATE_INDIRECT(CONCATENATE_INDIRECT(a, b), c)
#define CONCATENATE4(a, b, c, d) CONCATENATE3(CONCATENATE_INDIRECT(a, b), c, d)
#define STRINGIZE(x) #x
#define EXPAND_AND_STRINGIZE(x) STRINGIZE(x)
#define executeFunction(f) &CONCATENATE4(testClass, _, f, _Test::executeProgram)

std::vector<Byte> interruptTestProgram = {
	0xca,					// 1000: dex
	0xc8, 					//       iny
	0xca, 					//       dex
	0xc8, 					//       iny
	0xca, 					//       dex
	0xc8, 					//       iny
	0x4c, 0x00, 0x10		//		 jmp #$1000
};

TEST_F(testClass, InlineMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.getFlagI());

	// When
	cpu.execute();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, InlineMaskableInterruptDoesNotInterruptWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.setFlagI(true);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_TRUE(cpu.getFlagI());

	// When
	cpu.execute();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1001);
	EXPECT_EQ(cpu.getSP(), initialSP);
	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, MaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.setNMIVector(0x3000);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());

	// When
	std::thread runProgram(executeFunction(MaskableInterrupt), this);
	usleep(250);
	cpu.raiseIRQ();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, NonMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x3000);
	cpu.setNMIVector(0x4000);

	// When
	std::thread runProgram(executeFunction(NonMaskableInterrupt), this);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, NonMaskableInterruptWorksEvenWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setNMIVector(0x4000);
	cpu.setFlagI(true);

	// When
	std::thread runProgram(executeFunction(NonMaskableInterruptWorksEvenWhenIFlagSet), this);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x4000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, MaskableInterruptFollowedByRTSWorks) {

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
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x1005);
	cpu.setInterruptVector(0x4000);
	mem.loadData(rtiProgram, 0x4000);

	// When
	cpu.raiseIRQ();
	std::thread runProgram(executeFunction(MaskableInterruptFollowedByRTSWorks), this);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1005);
	EXPECT_EQ(cpu.getSP(), initialSP);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_FALSE(cpu.getFlagI());
}

TEST_F(testClass, NonMaskableInterruptFollowedByRTSWorks) {

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
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x1005);
	cpu.setNMIVector(0x4000);
	mem.loadData(rtiProgram, 0x4000);

	// When
	cpu.raiseNMI();
	std::thread runProgram(executeFunction(NonMaskableInterruptFollowedByRTSWorks), this);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1005);
	EXPECT_EQ(cpu.getSP(), initialSP);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_FALSE(cpu.getFlagI());
}

TEST_F(testClass, SimultaneousNMIandIRQRunsNMIFirst) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.setNMIVector(0x3000);
	cpu.raiseIRQ();
	cpu.raiseNMI();

	mem[0x1000] = cpu.Opcodes.NOP_IMP;

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.getFlagI());

	// When
	cpu.execute();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x3000);
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
}

TEST_F(testClass, SimultaneousNMIandIRQRunsNMIFirstAndThenRunsIRQAfterNMICompletes) {
    
	// Loop until both X and Y are 0
	// 1000 loop: cpy #0
	// 1002       bne loop
	// 1004       cmp #0
	// 1006       bne loop
	// 1008       nop
	std::vector<Byte> thisProgram = { 0xc0, 0x00, 0xd0, 0xfc, 0xe0, 0x00, 0xd0, 0xf8, 0xea };

	// Set Y to 0
	// 4000       ldy #0
	// 4002       rti
	std::vector<Byte> nmiProgram  = { 0xa0, 0x00, 0x40 };

	// Set X to 0
	// 4000       ldx #0
	// 4002       rti
	std::vector<Byte> irqProgram  = { 0xa2, 0x00, 0x40 };

	//Given:
	mem.loadData(thisProgram, 0x1000);
	cpu.TestReset(0x1000);
	Word initialSP = cpu.getSP();
	cpu.setHaltAddress(0x1008);
	cpu.setInterruptVector(0x3000);
	mem.loadData(irqProgram, 0x3000);
	cpu.setNMIVector(0x4000);
	mem.loadData(nmiProgram, 0x4000);
	cpu.setY(0xff);
	cpu.setX(0xff);

	// When
	cpu.raiseNMI();
	cpu.raiseIRQ();
	std::thread runProgram(executeFunction(NonMaskableInterruptFollowedByRTSWorks), this);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.getPC(), 0x1008);
	EXPECT_EQ(cpu.getSP(), initialSP);
	EXPECT_EQ(cpu.getY(), 0); // NMI ran
	EXPECT_EQ(cpu.getX(), 0); // IRQ ran
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_FALSE(cpu.getFlagI());

}