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
};

// label: dex
//        iny
//        dex
//        iny
//        dex
//        iny
//        jmp label
std::vector<Byte> interruptTestProgram = {
	0xca, 0xc8, 0xca, 0xc8, 0xca, 0xc8, 0x4c, 0x00, 0x10
};

TEST_F(MOS6502InterruptTests, InlineMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.Flags.I);

	// When
	cpu.executeOneInstruction();

	// Expect
	EXPECT_EQ(cpu.PC, 0x4000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.Flags.I);
}

TEST_F(MOS6502InterruptTests, InlineMaskableInterruptDoesNotInterruptWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.Flags.I = 1;
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_TRUE(cpu.Flags.I);

	// When
	cpu.executeOneInstruction();

	// Expect
	EXPECT_EQ(cpu.PC, 0x1001);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.Flags.I);
}

TEST_F(MOS6502InterruptTests, MaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());

	// When
	std::thread runProgram(&CPU::execute, &cpu);
	usleep(250);
	cpu.raiseIRQ();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.PC, 0x4000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.Flags.I);
}

TEST_F(MOS6502InterruptTests, NonMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);

	// When
	std::thread runProgram(&CPU::execute, &cpu);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.PC, 0x4000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.Flags.I);
}

TEST_F(MOS6502InterruptTests, NonMaskableInterruptWorksEvenWhenIFlagSet) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.Flags.I = 1;

	// When
	std::thread runProgram(&CPU::execute, &cpu);
	usleep(250);
	cpu.raiseNMI();
	usleep(250);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.PC, 0x4000);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP - 3);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_TRUE(cpu.Flags.I);
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
	cpu.Reset(0x1000);
	cpu.setExitAddress(0x1005);
	cpu.setInterruptVector(0x4000);
	mem.loadData(rtiProgram, 0x4000);

	// When
	cpu.raiseIRQ();
	std::thread runProgram(&CPU::execute, &cpu);
	runProgram.join();

	// Expect
	EXPECT_EQ(cpu.PC, 0x1005);
	EXPECT_EQ(cpu.SP, CPU::INITIAL_SP);
	EXPECT_FALSE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.pendingNMI());
	EXPECT_FALSE(cpu.Flags.I);
}
