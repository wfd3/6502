#include <string>
#include <cstring>
#include <memory>
#include <cstdarg>

#include "6502.h"

/////////////////////////////////////////////////////////////////////////
CPU::CPU(Memory *m) {
	mem = m;
}

// BCD addition and substraction functions.
// See:
// https://www.electrical4u.com/bcd-or-binary-coded-decimal-bcd-conversion-addition-subtraction/

void CPU::bcdADC(Byte operand) {
	Byte addend, carry, a_low;
	int answer;

	addend = A;
	carry = Flags.C;

	// Low nibble first
	a_low = (addend & 0x0f) + (operand & 0x0f) + carry;
	if (a_low >= 0x0a) 
		a_low = ((a_low + 0x06) & 0x0f) + 0x10;

	answer = (addend & 0xf0) + (operand & 0xf0) + a_low;
	if (answer >= 0xa0) 
		answer += 0x60;
	
	A = answer & 0xff;
	
	SetFlagN(answer);
	SetFlagZ(answer);
	Flags.C = (answer >= 0x100);
	Flags.V = (answer < -128) || (answer > 127);
}

void CPU::bcdSBC(Byte subtrahend) {
	SByte op_l;
	int operand;
	Byte carry;

	operand = A;
	carry = (Flags.C == 0);

	// Low nibble first
	op_l = (operand & 0x0f) - (subtrahend & 0x0f) - carry;
	if (op_l < 0) 
		op_l = ((op_l - 0x06) & 0x0f) - 0x10;
					
	operand = (operand & 0xf0) - (subtrahend & 0xf0);
	operand += op_l;
	if (operand < 0) 
		operand -= 0x60;

	A = (Byte) operand & 0xff;

	SetFlagZ(A);
	Flags.C = (operand >= 0);
}

// A = A + operand + Flags.C
void CPU::doADC(Byte operand) {
	Word result;
	bool same_sign;

	same_sign = (A & NegativeBit) == (operand & NegativeBit);
	result = A + operand + Flags.C;
	A = result & 0xff;
	SetFlagZ(A);
	SetFlagN(A);
	Flags.C = result > 0xff;
	Flags.V = same_sign &&
		((A & NegativeBit) !=  (operand & NegativeBit));
}

////
// CPU Instructions

void CPU::ins_adc(Byte opcode, Byte &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);
	
	if (Flags.D) {
		bcdADC(operand);
		return;
	}

	doADC(operand);
}

void CPU::ins_and(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A &= data;
	SetFlagsForRegister(A);
}

void CPU::ins_asl(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		data = A;
	} else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = ReadByte(address);
	}

	Flags.C = (data & NegativeBit) > 0;
	data = data << 1;
	SetFlagN(data);
	SetFlagZ(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		A = data;
	} else {
		WriteByte(address, data);
	}
	
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;	
}

// Set PC to @address if @condition is true
void CPU::doBranch(bool condition, Word address, Word _PC,
		   Byte &expectedCyclesToUse) {
	(void) _PC;		// Avoid compiler warning;
	
	if (condition) {
		Cycles++;	// Branch taken
		expectedCyclesToUse++;

		if ((PC >> 8) != (address >> 8)) { // Crossed page boundry
			Cycles += 2;
			expectedCyclesToUse += 2;
		}

		PC = address;
	}
}

void CPU::ins_bcc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.C, address, _pc, expectedCyclesToUse);
}

void CPU::ins_bcs(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.C, address, _pc, expectedCyclesToUse);
}

void CPU::ins_beq(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.Z, address, _pc, expectedCyclesToUse);
}

void CPU::ins_bit(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	SetFlagZ(A & data);
	SetFlagN(data);
	Flags.V = (data & (1 << 6)) != 0;
}

void CPU::ins_bmi(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.N, address, _pc, expectedCyclesToUse);
}

void CPU::ins_bne(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.Z, address, _pc, expectedCyclesToUse);
}

void CPU::ins_bpl(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.N, address, _pc, expectedCyclesToUse);
}

void CPU::ins_brk(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	// From the internets, BRK pushes PC + 1 to the stack. See:
	// https://retrocomputing.stackexchange.com/questions/12291/what-are-uses-of-the-byte-after-brk-instruction-on-6502

	PC++;
	PushWord(PC);

	addBacktrace(PC);

	PushPS();
	PC = ReadWord(INT_VECTOR);
	Flags.B = 1;
	Flags.I = 1;
	Cycles++;
}

void CPU::ins_bvc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(!Flags.V, address, _pc, expectedCyclesToUse);
}

void CPU::ins_bvs(Byte opcode, Byte &expectedCyclesToUse) {
	Word address, _pc;

	_pc = PC - 1;
	address = getAddress(opcode, expectedCyclesToUse);
	doBranch(Flags.V, address, _pc, expectedCyclesToUse);
}

void CPU::ins_clc(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.C = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cld(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.D = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cli(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.I = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_clv(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.V = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cmp(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = A >= data;
	Flags.Z = A == data;
	Flags.N = A  < data;
}

void CPU::ins_cpx(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = X >= data;
	Flags.Z = X == data;
	Flags.N = X  < data;
}

void CPU::ins_cpy(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	Flags.C = Y >= data;
	Flags.Z = Y == data;
	Flags.N = Y  < data;

}

void CPU::ins_dec(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = ReadByte(address);
	data--;
	WriteByte(address, data);
	SetFlagZ(data);
	SetFlagN(data);
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_dex(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X--;
	SetFlagN(X);
	SetFlagZ(X);
	Cycles++;
}

void CPU::ins_dey(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y--;
	SetFlagN(Y);
	SetFlagZ(Y);
	Cycles++;
}

void CPU::ins_eor(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A ^= data;
	SetFlagsForRegister(A);
}

void CPU::ins_inc(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	address = getAddress(opcode, expectedCyclesToUse);
	data = ReadByte(address);
	data++;
	WriteByte(address, data);
	SetFlagZ(data);
	SetFlagN(data);
	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_inx(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X++;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_iny(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y++;
	SetFlagZ(Y);
	SetFlagN(Y);
	Cycles++;
}

void CPU::ins_jmp(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = PC;

	addBacktrace(PC - 1);

	if (instructions[opcode].addrmode == ADDR_MODE_IND) {
		address = getAddress(opcode, expectedCyclesToUse);
	}

	PC = ReadWord(address);
}

void CPU::ins_jsr(Byte opcode, Byte &expectedCyclesToUse) {
	Word newPC;
	
	addBacktrace(PC - 1);

	newPC = ReadWord(PC);
	PushWord(PC + 1);
	PC = newPC;
	
	Cycles++;

	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
}

void CPU::ins_lda(Byte opcode, Byte &expectedCyclesToUse) {
	A = getData(opcode, expectedCyclesToUse);
	SetFlagsForRegister(A);
}

void CPU::ins_ldx(Byte opcode, Byte &expectedCyclesToUse) {
	X = getData(opcode, expectedCyclesToUse);
	SetFlagsForRegister(X);
}

void CPU::ins_ldy(Byte opcode, Byte &expectedCyclesToUse) {
	Y = getData(opcode, expectedCyclesToUse);
	SetFlagsForRegister(Y);
}

void CPU::ins_lsr(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) 
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = ReadByte(address);
	}

	Flags.C = (data & 1);
	data = data >> 1;
	SetFlagZ(data);
	SetFlagN(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		WriteByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;	
}

void CPU::ins_nop(Byte opcode, Byte &expectedCyclesToUse) {
	// NOP, like all single byte instructions, takes
	// two cycles.
	Cycles++;

	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
}

void CPU::ins_ora(Byte opcode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(opcode, expectedCyclesToUse);
	A |= data;
	SetFlagsForRegister(A);
}

void CPU::ins_pha(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Push(A);
	Cycles++;		// Single byte instruction
}

void CPU::ins_pla(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = Pop();
	SetFlagsForRegister(A);
	Cycles += 2;      
}

void CPU::PushPS() {
	// PHP silently sets the Unused flag (bit 5) and the Break flag (bit 4)
	Push(PS | UnusedBit | BreakBit);
}

void CPU::PopPS() {
	PS = Pop();
	Flags.B = false;
	Flags._unused = false;
}

void CPU::ins_php(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	PushPS();
	Cycles++;		// Single byte instruction
}
void CPU::ins_plp(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	PopPS();
	Cycles += 2;
}
void CPU::ins_rol(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data, carry;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) {
		data = A;
	} else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = ReadByte(address);
	}

	carry = Flags.C;
	Flags.C = (data & NegativeBit) > 0;

	data = (data << 1) | carry;

	SetFlagZ(data);
	SetFlagN(data);

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		WriteByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_ror(Byte opcode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data, zero;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC) 
		data = A;
	else {
		address = getAddress(opcode, expectedCyclesToUse);
		data = ReadByte(address);
	}

	zero = data & 1;
	data = data >> 1;
	if (Flags.C)
		data |= NegativeBit;
	SetFlagN(data);
	SetFlagZ(data);
	Flags.C = zero;

	if (instructions[opcode].addrmode == ADDR_MODE_ACC)
		A = data;
	else 
		WriteByte(address, data);

	Cycles++;
	if (instructions[opcode].addrmode == ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_rti(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	removeBacktrace();
	PopPS();
	PC = PopWord();
	Cycles += 2;
}

void CPU::ins_rts(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;

	removeBacktrace();
	
	PC = PopWord() + 1;
	Cycles += 3;	       
}

void CPU::ins_sbc(Byte opcode, Byte &expectedCyclesToUse) {
	Byte operand = getData(opcode, expectedCyclesToUse);

	if (Flags.D) {
		bcdSBC(operand); 
		return;
	}
	
	doADC(~operand);
}

void CPU::ins_sec(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.C = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sed(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.D = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sei(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Flags.I = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sta(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	WriteByte(address, A);
}

void CPU::ins_stx(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	WriteByte(address, X);
}

void CPU::ins_sty(Byte opcode, Byte &expectedCyclesToUse) {
	Word address = getAddress(opcode, expectedCyclesToUse);
	WriteByte(address, Y);
}

void CPU::ins_tax(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X = A;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_tay(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	Y = A;
	SetFlagZ(Y);
	SetFlagN(Y);
	Cycles++;
}

void CPU::ins_tsx(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	X = SP;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_txa(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = X;
	SetFlagZ(A);
	SetFlagN(A);
	Cycles++;
}

void CPU::ins_txs(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	SP = X;
	Cycles++;
}

void CPU::ins_tya(Byte opcode, Byte &expectedCyclesToUse) {
	(void)opcode;		// Suppress '-Wununsed' warnings
	(void)expectedCyclesToUse;
	
	A = Y;
	SetFlagZ(A);
	SetFlagN(A);
	Cycles++;
}

void CPU::Reset(Word ResetVector) {
	PC = ResetVector;
	SP = INITIAL_SP;
	A = X = Y = 0;
	PS = 0; //C = Z = I = D = B = V = N = 0;

	debugMode = false;
	debug_alwaysShowPS = false;
	debug_lastCmd = "";
	InterpretNextOpcodeAsByte = false;
	CPU::setupInstructionMap();
}

void CPU::Exception(const char *fmt_str, ...) {
	va_list args;
	int final_n, n = 256;
	std::unique_ptr<char[]> formatted;

	va_start(args, fmt_str);
	while (1) {
		// Wrap the plain char array into the unique_ptr 
		formatted.reset(new char[n]); 
		strcpy(&formatted[0], fmt_str);

		final_n = vsnprintf(formatted.get(), n, fmt_str, args);

		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	va_end(args);

	printf("CPU Exception: %s\n", formatted.get());
	printf("Entering debugger\n");
	debugMode = true;
	Debug();
}

void CPU::SetFlagZ(Byte val) {
	Flags.Z = (val == 0);
}

void CPU::SetFlagN(Byte val) {
	Flags.N = (val & NegativeBit) != 0;
}

void CPU::SetFlagsForRegister(Byte b) {
	SetFlagZ(b);
	SetFlagN(b);
}

void CPU::SetFlagsForCompare(Byte b, Byte v) {
	Flags.C = (b >= v);
	SetFlagZ(b);
	SetFlagN(b);
}

Byte CPU::ReadByte(Word address) {
	Byte data = mem->ReadByte(address);
	Cycles++;
	return data;
}

void CPU::WriteByte(Word address, Byte value) {
	mem->WriteByte(address, value);
	Cycles++;
}

Word CPU::ReadWord(Word address) {
	Word w = ReadByte(address) | (ReadByte(address + 1) << 8);
	return w;
}

Word CPU::ReadWordAtPC() {
	Word w = ReadByteAtPC() | (ReadByteAtPC() << 8);
	return w;
}

Byte CPU::ReadByteAtPC() {
	Byte opcode = ReadByte(PC);
	PC++;
	return opcode;
}

void CPU::PushWord(Word value) {
	Byte b;

	b = (Byte) (value >> 8); // value high
	Push(b);

	b = (Byte) value & 0xff; // value low
	Push(b);
}

Word CPU::PopWord() {
	Word w;

	// Low byte then high byte
	w = Pop() | (Pop() << 8);
	return w;
}

void CPU::Push(Byte value) {
	Word SPAddress = STACK_FRAME + SP;
	WriteByte(SPAddress, value);
	SP--;
}

Byte CPU::Pop() {
	Word SPAddress;
	SP++;
	SPAddress = STACK_FRAME + SP;
	return ReadByte(SPAddress);
}

Word CPU::getAddress(Byte opcode, Byte &expectedCycles) {
	Word address;
	Byte zpaddr;
	Byte flags;
	SByte rel;

	flags = instructions[opcode].flags;
	
	switch (instructions[opcode].addrmode) {

        // ZeroPage mode (tested)
	case ADDR_MODE_ZP:
		address = ReadByteAtPC();
		break;

	// ZeroPage,X 
	case ADDR_MODE_ZPX:
		zpaddr = ReadByteAtPC() + X;
		address = zpaddr;
		Cycles++;
		break;

        // ZeroPage,Y 
	case ADDR_MODE_ZPY:
		zpaddr = ReadByteAtPC() + Y;
		address = zpaddr;
		Cycles++;
		break;

	case ADDR_MODE_REL:
		rel = SByte(ReadByteAtPC());
		address = PC + rel;
		break;

	// Absolute
	case ADDR_MODE_ABS:
		address = ReadWordAtPC();
		break;

	// Absolute,X 
	case ADDR_MODE_ABX:
		address = ReadWordAtPC();
		// Add a cycle if a page boundry is crossed
		if ((flags == CYCLE_CROSS_PAGE) && ((address + X) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += X;
		break;

	// Absolute,Y 
	case ADDR_MODE_ABY:
		address = ReadWordAtPC();
		// Add a cycle if a page boundry is crossed
		if ((flags == CYCLE_CROSS_PAGE) && ((address + Y) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += Y;
		break;

	// Indirect 
	case ADDR_MODE_IND:
		address = ReadWordAtPC();
		break;

        // (Indirect,X) or Indexed Indirect
	case ADDR_MODE_IDX:	
		zpaddr = ReadByteAtPC() + X;
		address = ReadWord(zpaddr);
		Cycles++;
		break;

	// (Indirect),Y or Indirect Indexed
	case ADDR_MODE_IDY:
		address = ReadByteAtPC();
		address = ReadWord(address);
		address += Y;
		break;

	default:
		Exception("Invalid addressing mode: 0x%ld\n",
			  instructions[opcode].addrmode);
		break;
	}
	
	return address;
}

Byte CPU::getData(Byte opcode, Byte &expectedCycles) {
	Byte data;
	Word address;

	switch (instructions[opcode].addrmode) {
	case ADDR_MODE_IMP:
	case ADDR_MODE_ACC:
		return 0;

	// Immediate mode (tested)
	case ADDR_MODE_IMM:
		data = ReadByteAtPC();
		break;

	default:
		address = getAddress(opcode, expectedCycles);
		data = ReadByte(address);
		break;
	}

	return data;
}

std::tuple<CPU::Cycles_t, CPU::Cycles_t> CPU::ExecuteOneInstruction() {
	Byte opcode;
	Cycles_t startCycles = Cycles;
	Byte expectedCyclesToUse;
	Word startPC;
	opfn_t op;

	startPC = PC;
	opcode = ReadByteAtPC();

	if (instructions.count(opcode) == 0) {
		Exception("Invalid opcode 0x%x at PC 0x%04x\n", opcode, PC - 1);
	}

	expectedCyclesToUse = instructions[opcode].cycles;
	op = instructions[opcode].opfn;

	(this->*op)(opcode, expectedCyclesToUse);

	if (CPU::debug_loopDetection && startPC == PC) {
		printf("# Loop detected, forcing break at %04x\n", PC);
		CPU::addBreakpoint(PC);
	}

	return std::make_tuple(Cycles - startCycles, expectedCyclesToUse);
}

void CPU::Execute() {

	while (1) {
		if (CPU::debugMode || CPU::isBreakpoint(PC))
			CPU::Debug();
		else
			CPU::ExecuteOneInstruction();
	}
}

