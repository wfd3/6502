#include <gtest/gtest.h>
#include "6502.h"

MEMORY mem;
CPU cpu;

class MOS6502Tests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

TEST_F(MOS6502Tests, and_immediate) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_IMM;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();

	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;

	//When:
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then:
	EXPECT_EQ(cpu.A, 0x0F);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_zp) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ZP;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_zpx) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ZPX;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x01;
	cpu.X = 0x80;
	mem[0x81] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_abs) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ABS;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	mem[0x2000] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_abx) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_abx_crosses_page) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ABX;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.X = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles + 1); // This is a hack.
	// If the instruction can take a variable number of cycles, we
	// need to know that the extra cycles are OK in any case.
	// Probably ExecuteOneInstruction() should return the expected
	// and actual cycles?  TODO
}

TEST_F(MOS6502Tests, and_aby) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x00;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x2010] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles); 
}

TEST_F(MOS6502Tests, and_aby_crosses_page) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_ABY;
	
	// Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0xFF;
	mem[0xFFFE] = 0x20;
	cpu.Y = 0x10;
	mem[0x20FF + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles + 1); // This is a hack.
	// If the instruction can take a variable number of cycles, we
	// need to know that the extra cycles are OK in any case.
	// Probably ExecuteOneInstruction() should return the expected
	// and actual cycles?  TODO
}

TEST_F(MOS6502Tests, and_idx) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_IDX;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	cpu.X = 0x10;
	mem[0x00 + 0x10 + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles);
}

TEST_F(MOS6502Tests, and_idy) {
	Cycles_t UsedCycles;
	Byte ins = CPU::INS_AND_IDY;

	//Given:
	cpu.Reset(CPU::INITIAL_PC);
	mem.Init();
	
	mem[0xFFFC] = ins;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x00;
	mem[0x0011] = 0x20;
	cpu.Y = 0x10;
	mem[0x2000 + 0x10] = 0x0F;
	cpu.A = 0xFF;

	// When: 
	UsedCycles = cpu.ExecuteOneInstruction();

	// Then
	EXPECT_EQ(cpu.A, 0x0f);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	EXPECT_EQ(UsedCycles, cpu.instructions[ins].cycles);
}
