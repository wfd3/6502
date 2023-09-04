#include <gtest/gtest.h>
#include <6502.h>
#include <thread>

class MOS6502InterruptTests : public testing::Test {
public:

	Memory<Address_t, Byte> mem{CPU::MAX_MEM};
	CPU cpu{&mem};

	virtual void SetUp() {
		cpu.exitReset();
		mem.mapRAM(0, CPU::MAX_MEM);
	}
	
	virtual void TearDown()	{
	}
};

// label:
//  dex
//  iny
//  dex
//  iny
//  dex
//  iny
//  jmp label
std::vector<Byte> interruptTestProgram = {
	0xca, 0xc8, 0xca, 0xc8, 0xca, 0xc8, 0xc4, 0x00, 0x10
};

TEST_F(MOS6502InterruptTests, InlineMaskableInterrupt) {

	//Given:
	mem.loadData(interruptTestProgram, 0x1000);
	cpu.PC = 0x1000;
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_FALSE(cpu.Flags.I);
	EXPECT_EQ(cpu.PC, 0x1000);

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
	cpu.PC = 0x1000;
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);
	cpu.Flags.I = 1;
	cpu.raiseIRQ();

	EXPECT_TRUE(cpu.pendingIRQ());
	EXPECT_TRUE(cpu.Flags.I);
	EXPECT_EQ(cpu.PC, 0x1000);

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
	cpu.PC = 0x1000;
	cpu.setExitAddress(0x4000);
	cpu.setInterruptVector(0x4000);

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
	cpu.PC = 0x1000;
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
	cpu.PC = 0x1000;
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
