//
// Tests for WAI (Wait for Interrupt) instruction
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

class MOS65C02WAITests : public testing::Test {
public:
	Memory<Word, Byte> mem{MOS65C02::LAST_ADDRESS};
	MOS65C02 cpu{mem};

	virtual void SetUp() {
		mem.mapRAM(0, MOS65C02::LAST_ADDRESS);
	}

	virtual void TearDown()	{
	}

	void executeProgram() {
		while(!cpu.isPCAtHaltAddress())
			cpu.execute();
	}
};

// Basic execution test
TEST_F(MOS65C02WAITests, WAI_SetsWAIFlag) {
	Byte ins = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = ins;

	//When:
	cpu.execute();

	//Then:
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_EQ(cpu.getPC(), 0x1001);
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());
}

// Test that WAI blocks subsequent instruction execution
TEST_F(MOS65C02WAITests, WAI_BlocksSubsequentExecution) {
	Byte wai = cpu.Opcodes.WAI_IMP;
	Byte nop = cpu.Opcodes.NOP_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	mem[0x1001] = nop;  // Should not execute
	mem[0x1002] = nop;  // Should not execute

	//When:
	cpu.execute();  // Execute WAI
	Word pcAfterWAI = cpu.getPC();
	EXPECT_EQ(cpu.usedCycles(), cpu.expectedCycles());  // No cycles consumed after WAI

	cpu.execute();  // Try to execute NOP (should be blocked)
	cpu.execute();  // Try again (should still be blocked)

	//Then:
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_EQ(cpu.getPC(), pcAfterWAI);  // PC hasn't moved
}

// Test that IRQ wakes CPU from WAI
TEST_F(MOS65C02WAITests, IRQ_WakesFromWAI) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setInterruptVector(0x2000);
	cpu.setHaltAddress(0x2000);
	Word initialSP = cpu.getSP();

	//When:
	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());

	cpu.raiseIRQ();  // Raise interrupt

	//Then:
	EXPECT_FALSE(cpu.isInWAI());  // WAI cleared
	EXPECT_TRUE(cpu.pendingIRQ());  // Interrupt is pending

	cpu.execute();  // Process interrupt

	EXPECT_EQ(cpu.getPC(), 0x2000);  // Jumped to IRQ vector
	EXPECT_FALSE(cpu.pendingIRQ());  // Interrupt processed
	EXPECT_TRUE(cpu.getFlagI());  // I flag set
	EXPECT_EQ(cpu.getSP(), initialSP - 3);  // Stack pushed
}

// Test that NMI wakes CPU from WAI
TEST_F(MOS65C02WAITests, NMI_WakesFromWAI) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setNMIVector(0x3000);
	cpu.setHaltAddress(0x3000);
	Word initialSP = cpu.getSP();

	//When:
	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());

	cpu.raiseNMI();  // Raise NMI

	//Then:
	EXPECT_FALSE(cpu.isInWAI());  // WAI cleared
	EXPECT_TRUE(cpu.pendingNMI());  // NMI is pending

	cpu.execute();  // Process NMI

	EXPECT_EQ(cpu.getPC(), 0x3000);  // Jumped to NMI vector
	EXPECT_FALSE(cpu.pendingNMI());  // NMI processed
	EXPECT_TRUE(cpu.getFlagI());  // I flag set
	EXPECT_EQ(cpu.getSP(), initialSP - 3);  // Stack pushed
}

// Test that IRQ with I flag set clears WAI but doesn't service interrupt
// Per 65C02 spec: WAI wakes on any interrupt, but IRQ only services if I=0
TEST_F(MOS65C02WAITests, IRQ_WithIFlagSet_WakesButDoesNotService) {
	Byte wai = cpu.Opcodes.WAI_IMP;
	Byte nop = cpu.Opcodes.NOP_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	mem[0x1001] = nop;
	cpu.setFlagI(true);  // Mask IRQs
	cpu.setInterruptVector(0x2000);
	Word initialSP = cpu.getSP();

	//When:
	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_TRUE(cpu.getFlagI());

	cpu.raiseIRQ();  // Try to raise IRQ (should wake but not service)

	//Then:
	EXPECT_FALSE(cpu.isInWAI());  // raiseIRQ() clears _inWAI
	EXPECT_TRUE(cpu.pendingIRQ());  // IRQ is pending but won't service

	cpu.execute();  // Try to process (should execute NOP, not IRQ)

	EXPECT_EQ(cpu.getPC(), 0x1002);  // Executed NOP, not interrupt
	EXPECT_TRUE(cpu.pendingIRQ());  // IRQ still pending
	EXPECT_TRUE(cpu.getFlagI());  // I flag still set
	EXPECT_EQ(cpu.getSP(), initialSP);  // Stack not modified
}

// Test that NMI bypasses I flag
TEST_F(MOS65C02WAITests, NMI_BypassesIFlag) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setFlagI(true);  // IRQs masked
	cpu.setNMIVector(0x3000);
	cpu.setHaltAddress(0x3000);
	Word initialSP = cpu.getSP();

	//When:
	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_TRUE(cpu.getFlagI());

	cpu.raiseNMI();  // NMI should wake and service regardless of I flag

	//Then:
	EXPECT_FALSE(cpu.isInWAI());
	EXPECT_TRUE(cpu.pendingNMI());

	cpu.execute();  // Process NMI

	EXPECT_EQ(cpu.getPC(), 0x3000);  // NMI serviced despite I flag
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
}

// Test that reset clears WAI flag
TEST_F(MOS65C02WAITests, Reset_ClearsWAIFlag) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setResetVector(0x2000);

	//When:
	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());

	cpu.Reset();  // Reset CPU

	//Then:
	EXPECT_FALSE(cpu.isInWAI());  // WAI cleared by reset
}

// Test asynchronous IRQ wake-up (simulates external interrupt)
TEST_F(MOS65C02WAITests, ThreadedIRQ_WakesFromWAI) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setInterruptVector(0x2000);
	cpu.setHaltAddress(0x2000);
	Word initialSP = cpu.getSP();

	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());

	//When: Raise IRQ from another thread (simulates external interrupt)
	std::thread irqThread([this]() {
		usleep(100);  // Small delay
		cpu.raiseIRQ();
	});

	executeProgram();  // Wait for halt
	irqThread.join();

	//Then:
	EXPECT_EQ(cpu.getPC(), 0x2000);  // Interrupt serviced
	EXPECT_FALSE(cpu.isInWAI());
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_TRUE(cpu.getFlagI());
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
}

// Test asynchronous NMI wake-up
TEST_F(MOS65C02WAITests, ThreadedNMI_WakesFromWAI) {
	Byte wai = cpu.Opcodes.WAI_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	cpu.setNMIVector(0x3000);
	cpu.setHaltAddress(0x3000);
	Word initialSP = cpu.getSP();

	cpu.execute();  // Execute WAI
	EXPECT_TRUE(cpu.isInWAI());

	//When: Raise NMI from another thread
	std::thread nmiThread([this]() {
		usleep(100);  // Small delay
		cpu.raiseNMI();
	});

	executeProgram();  // Wait for halt
	nmiThread.join();

	//Then:
	EXPECT_EQ(cpu.getPC(), 0x3000);  // NMI serviced
	EXPECT_FALSE(cpu.isInWAI());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.getFlagI());
	EXPECT_EQ(cpu.getSP(), initialSP - 3);
}

// Test multiple WAI executions
TEST_F(MOS65C02WAITests, MultipleWAI_ExecutionsWork) {
	Byte wai = cpu.Opcodes.WAI_IMP;
	Byte nop = cpu.Opcodes.NOP_IMP;

	//Given:
	cpu.TestReset(0x1000);
	mem[0x1000] = wai;
	mem[0x1001] = nop;
	mem[0x1002] = wai;
	mem[0x1003] = nop;
	cpu.setInterruptVector(0x2000);

	//When: First WAI
	cpu.execute();
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_EQ(cpu.getPC(), 0x1001);

	cpu.raiseIRQ();
	EXPECT_FALSE(cpu.isInWAI());

	cpu.execute();  // Service interrupt - jumps to 0x2000
	EXPECT_EQ(cpu.getPC(), 0x2000);

	// Simulate RTI by manually setting PC back
	cpu.setPC(0x1001);

	cpu.execute();  // Execute NOP
	EXPECT_EQ(cpu.getPC(), 0x1002);

	cpu.execute();  // Second WAI
	EXPECT_TRUE(cpu.isInWAI());
	EXPECT_EQ(cpu.getPC(), 0x1003);

	cpu.raiseNMI();
	EXPECT_FALSE(cpu.isInWAI());

	//Then: WAI can be used multiple times
	EXPECT_FALSE(cpu.isInWAI());
}
