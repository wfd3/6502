#include <map>
#include "6502.h"

/////////////////////////////////////////////////////////////////////////
CPU::CPU(Memory *m) {
	mem = m;
}

void CPU::ins_adc(unsigned long addrmode, Byte &expectedCyclesToUse) {
}

void CPU::ins_and(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	A &= data;
	SetFlagsForRegister(A);
}

void CPU::ins_asl(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address;
	Byte data;

	if (addrmode & ADDR_MODE_ACC) {
		Flags.C = (A & (1 << 7)) > 1;
		A = A << 1;
		data = A;
	} else {
		address = getAddress(addrmode, expectedCyclesToUse);
		data = ReadByte(address);
		Flags.C = data & (1 << 7);
		data = data << 1;
		WriteByte(address, data);
	}
	SetFlagN(data);
	SetFlagZ(data);
	Cycles++;
	if (addrmode & ADDR_MODE_ABX)
		Cycles++;	
}

// Set PC to @address if @condition is true
void CPU::do_branch(bool condition, Word address, Byte &expectedCyclesToUse) {
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

void CPU::ins_bcc(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(!Flags.C, address, expectedCyclesToUse);
}

void CPU::ins_bcs(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(Flags.C, address, expectedCyclesToUse);
}

void CPU::ins_beq(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(Flags.Z, address, expectedCyclesToUse);
}

void CPU::ins_bit(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	data &= A;
	SetFlagZ(data);
	SetFlagN(data);
	Flags.V = (data & (1 << 6)) == (1 << 6);
}

void CPU::ins_bmi(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(Flags.N, address, expectedCyclesToUse);
}

void CPU::ins_bne(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(!Flags.Z, address, expectedCyclesToUse);
}

void CPU::ins_bpl(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(!Flags.N, address, expectedCyclesToUse);
}

void CPU::ins_brk(unsigned long addrmode, Byte &expectedCyclesToUse) {
	PushWord(PC);
	Push(PS);
	PC = ReadWord(INT_VECTOR);
	Flags.B = 1;
	Cycles++;
}

void CPU::ins_bvc(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(!Flags.V, address, expectedCyclesToUse);
}

void CPU::ins_bvs(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = getAddress(addrmode, expectedCyclesToUse);
	do_branch(Flags.V, address, expectedCyclesToUse);
}

void CPU::ins_clc(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.C = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cld(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.D = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cli(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.I = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_clv(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.V = 0;
	Cycles++;		// Single byte instruction
}

void CPU::ins_cmp(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	Flags.C = A >= data;
	Flags.Z = A == data;
	Flags.N = A  < data;
}

void CPU::ins_cpx(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	Flags.C = X >= data;
	Flags.Z = X == data;
	Flags.N = X  < data;
}

void CPU::ins_cpy(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	Flags.C = Y >= data;
	Flags.Z = Y == data;
	Flags.N = Y  < data;

}

void CPU::ins_dec(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address;
	Byte data;

	address = getAddress(addrmode, expectedCyclesToUse);
	data = ReadByte(address);
	data--;
	WriteByte(address, data);
	SetFlagZ(data);
	SetFlagN(data);
	Cycles++;
	if (addrmode &ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_dex(unsigned long addrmode, Byte &expectedCyclesToUse){
	X--;
	SetFlagN(X);
	SetFlagZ(X);
	Cycles++;
}

void CPU::ins_dey(unsigned long addrmode, Byte &expectedCyclesToUse){
	Y--;
	SetFlagN(Y);
	SetFlagZ(Y);
	Cycles++;
}

void CPU::ins_eor(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	A ^= data;
	SetFlagsForRegister(A);
}

void CPU::ins_inc(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address;
	Byte data;

	address = getAddress(addrmode, expectedCyclesToUse);
	data = ReadByte(address);
	data++;
	WriteByte(address, data);
	Flags.Z = data == 0;
	Flags.N = (data &NegativeBit) > 0;
	Cycles++;
	if (addrmode &ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_inx(unsigned long addrmode, Byte &expectedCyclesToUse){
	X++;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_iny(unsigned long addrmode, Byte &expectedCyclesToUse){
	Y++;
	SetFlagZ(Y);
	SetFlagN(Y);
	Cycles++;
}

void CPU::ins_jmp(unsigned long addrmode, Byte &expectedCyclesToUse) {
	Word address = PC;

	if (addrmode & ADDR_MODE_IND) {
		address = getAddress(addrmode, expectedCyclesToUse);
	}

	PC = ReadWord(address);
}

void CPU::ins_jsr(unsigned long addrmode, Byte &expectedCyclesToUse) {
	PushWord(PC - 1);
	PC = ReadWord(PC);
	Cycles++;
}

void CPU::ins_lda(unsigned long addrmode, Byte &expectedCyclesToUse){
	A = getData(addrmode, expectedCyclesToUse);
	SetFlagsForRegister(A);
}

void CPU::ins_ldx(unsigned long addrmode, Byte &expectedCyclesToUse){
	X = getData(addrmode, expectedCyclesToUse);
	SetFlagsForRegister(X);
}

void CPU::ins_ldy(unsigned long addrmode, Byte &expectedCyclesToUse){
	Y = getData(addrmode, expectedCyclesToUse);
	SetFlagsForRegister(Y);
}

void CPU::ins_lsr(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address;
	Byte data;

	if (addrmode &ADDR_MODE_ACC) {
		Flags.C = A & 1;
		A = A >> 1;
	} else {
		address = getAddress(addrmode, expectedCyclesToUse);
		data = ReadByte(address);
		Flags.C = data & 1;
		data = data >> 1;
		WriteByte(address, data);
	}
	Flags.N = 0;
	Cycles++;
	if (addrmode &ADDR_MODE_ABX)
		Cycles++;	
}

void CPU::ins_nop(unsigned long addrmode, Byte &expectedCyclesToUse){
	// NOP, like all single byte instructions, takes
	// two cycles.
	Cycles++;
}

void CPU::ins_ora(unsigned long addrmode, Byte &expectedCyclesToUse){
	Byte data;

	data = getData(addrmode, expectedCyclesToUse);
	A |= data;
	SetFlagsForRegister(A);
}

void CPU::ins_pha(unsigned long addrmode, Byte &expectedCyclesToUse){
	Push(A);
	Cycles++;		// Single byte instruction
}

void CPU::ins_pla(unsigned long addrmode, Byte &expectedCyclesToUse){
	A = Pop();
	SetFlagsForRegister(A);
	Cycles += 2;      
}

void CPU::ins_php(unsigned long addrmode, Byte &expectedCyclesToUse){
	Push(PS);
	Cycles++;		// Single byte instruction
}
void CPU::ins_plp(unsigned long addrmode, Byte &expectedCyclesToUse){
	PS = Pop();
	Cycles += 2;
}
void CPU::ins_rol(unsigned long addrmode, Byte &expectedCyclesToUse){

	Word address;
	Byte data;

	if (addrmode &ADDR_MODE_ACC) {
		data = A;
		A = (A << 1) | Flags.C;
	} else {
		address = getAddress(addrmode, expectedCyclesToUse);
		data = ReadByte(address);
		WriteByte(address, (data << 1) | Flags.C);
	}

	Flags.C = (data & (1 << 7)) > 0;
	SetFlagZ(data);
	SetFlagN(data << 1);

	Cycles++;
	if (addrmode &ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_ror(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address;
	Byte data, carry;

	carry = Flags.C;

	if (addrmode &ADDR_MODE_ACC) 
		data = A;
	else {
		address = getAddress(addrmode, expectedCyclesToUse);
		data = ReadByte(address);
	}

	Flags.C = (data & 1) > 0;

	data = (data >> 1) | (carry << 7);

	if (addrmode &ADDR_MODE_ACC)
		A = data;
	else 
		WriteByte(address, data);

	SetFlagN(data);
	SetFlagZ(data);

	Cycles++;
	if (addrmode &ADDR_MODE_ABX)
		Cycles++;
}

void CPU::ins_rti(unsigned long addrmode, Byte &expectedCyclesToUse) {
	PS = Pop();
	PC = PopWord();
	Cycles += 2;
}

void CPU::ins_rts(unsigned long addrmode, Byte &expectedCyclesToUse) {
	PC = PopWord();
	Cycles += 3;	       
}

void CPU::ins_sbc(unsigned long addrmode, Byte &expectedCyclesToUse){}

void CPU::ins_sec(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.C = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sed(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.D = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sei(unsigned long addrmode, Byte &expectedCyclesToUse){
	Flags.I = 1;
	Cycles++;		// Single byte instruction
}

void CPU::ins_sta(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address = getAddress(addrmode, expectedCyclesToUse);
	WriteByte(address, A);
}

void CPU::ins_stx(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address =
		getAddress(addrmode, expectedCyclesToUse);
	WriteByte(address, X);
}

void CPU::ins_sty(unsigned long addrmode, Byte &expectedCyclesToUse){
	Word address =
		getAddress(addrmode, expectedCyclesToUse);
	WriteByte(address, Y);
}

void CPU::ins_tax(unsigned long addrmode, Byte &expectedCyclesToUse){
	X = A;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_tay(unsigned long addrmode, Byte &expectedCyclesToUse){
	Y = A;
	SetFlagZ(Y);
	SetFlagN(Y);
	Cycles++;
}

void CPU::ins_tsx(unsigned long addrmode, Byte &expectedCyclesToUse){
	X = SP;
	SetFlagZ(X);
	SetFlagN(X);
	Cycles++;
}

void CPU::ins_txa(unsigned long addrmode, Byte &expectedCyclesToUse){
	A = X;
	SetFlagZ(A);
	SetFlagN(A);
	Cycles++;
}

void CPU::ins_txs(unsigned long addrmode, Byte &expectedCyclesToUse){
	SP = X;
	Cycles++;
}

void CPU::ins_tya(unsigned long addrmode, Byte &expectedCyclesToUse){
	A = Y;
	SetFlagZ(A);
	SetFlagN(A);
	Cycles++;
}

/////////////////////////////////////////////////////////////////////////

CPU::instruction CPU::makeIns(const char *name, unsigned long addrmode,
			      Byte cycles, opfn_t opfn) {
	struct instruction ins = { name, addrmode, cycles, opfn };
	return ins;
}

void CPU::setupInstructionMap() {
	instructions[CPU::INS_BRK_IMP] = 
		makeIns("BRK", ADDR_MODE_IMP, 7, &CPU::ins_brk);
	instructions[CPU::INS_ORA_IDX] = 
		makeIns("ORA", ADDR_MODE_IDX, 6, &CPU::ins_ora);
	instructions[CPU::INS_ASL_ACC] = 
		makeIns("ASL", ADDR_MODE_ACC, 2, &CPU::ins_asl);
	instructions[CPU::INS_ADC_ZP] = 
		makeIns("ADC", ADDR_MODE_ZP, 3, &CPU::ins_adc);
	instructions[CPU::INS_ROR_ZP] = 
		makeIns("ROR", ADDR_MODE_ZP, 5, &CPU::ins_ror);
	instructions[CPU::INS_ADC_IMM] = 
		makeIns("ADC", ADDR_MODE_IMM, 2, &CPU::ins_adc);
	instructions[CPU::INS_ROR_ACC] = 
		makeIns("ROR", ADDR_MODE_ACC, 2, &CPU::ins_ror);
	instructions[CPU::INS_JMP_IND] = 
		makeIns("JMP", ADDR_MODE_IND, 5, &CPU::ins_jmp);
	instructions[CPU::INS_ADC_ABS] = 
		makeIns("ADC", ADDR_MODE_ABS, 4, &CPU::ins_adc);
	instructions[CPU::INS_ROR_ABS] = 
		makeIns("ROR", ADDR_MODE_ABS, 6, &CPU::ins_ror);
	instructions[CPU::INS_BVS_REL] = 
		makeIns("BVS", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bvs);
	instructions[CPU::INS_ADC_IDY] = 
		makeIns("ADC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_adc);
	instructions[CPU::INS_ADC_ZPX] = 
		makeIns("ADC", ADDR_MODE_ZPX, 4, &CPU::ins_adc);
	instructions[CPU::INS_ROR_ZPX] = 
		makeIns("ROR", ADDR_MODE_ZPX, 6, &CPU::ins_ror);
	instructions[CPU::INS_SEI_IMP] = 
		makeIns("SEI", ADDR_MODE_IMP, 2, &CPU::ins_sei);
	instructions[CPU::INS_ADC_ABY] = 
		makeIns("ADC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_adc);
	instructions[CPU::INS_ADC_ABX] = 
		makeIns("ADC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_adc);
	instructions[CPU::INS_ROR_ABX] = 
		makeIns("ROR", ADDR_MODE_ABX, 7, &CPU::ins_ror);
	instructions[CPU::INS_STA_IDX] = 
		makeIns("STA", ADDR_MODE_IDX, 6, &CPU::ins_sta);
	instructions[CPU::INS_ORA_ABS] = 
		makeIns("ORA", ADDR_MODE_ABS, 4, &CPU::ins_ora);
	instructions[CPU::INS_STY_ZP] = 
		makeIns("STY", ADDR_MODE_ZP, 3, &CPU::ins_sty);
	instructions[CPU::INS_STA_ZP] = 
		makeIns("STA", ADDR_MODE_ZP, 3, &CPU::ins_sta);
	instructions[CPU::INS_STX_ZP] = 
		makeIns("STX", ADDR_MODE_ZP, 3, &CPU::ins_stx);
	instructions[CPU::INS_DEY_IMP] = 
		makeIns("DEY", ADDR_MODE_IMP, 2, &CPU::ins_dey);
	instructions[CPU::INS_TXA_IMP] = 
		makeIns("TXA", ADDR_MODE_IMP, 2, &CPU::ins_txa);
	instructions[CPU::INS_ASL_ABS] = 
		makeIns("ASL", ADDR_MODE_ABS, 6, &CPU::ins_asl);
	instructions[CPU::INS_STY_ABS] = 
		makeIns("STY", ADDR_MODE_ABS, 4, &CPU::ins_sty);
	instructions[CPU::INS_STA_ABS] = 
		makeIns("STA", ADDR_MODE_ABS, 4, &CPU::ins_sta);
	instructions[CPU::INS_STX_ABS] = 
		makeIns("STX", ADDR_MODE_ABS, 4, &CPU::ins_stx);
	instructions[CPU::INS_BCC_REL] = 
		makeIns("BCC", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bcc);
	instructions[CPU::INS_STA_IDY] = 
		makeIns("STA", ADDR_MODE_IDY, 6, &CPU::ins_sta);
	instructions[CPU::INS_STY_ZPX] = 
		makeIns("STY", ADDR_MODE_ZPX, 4, &CPU::ins_sty);
	instructions[CPU::INS_STA_ZPX] = 
		makeIns("STA", ADDR_MODE_ZPX, 4, &CPU::ins_sta);
	instructions[CPU::INS_STX_ZPY] = 
		makeIns("STX", ADDR_MODE_ZPY, 4, &CPU::ins_stx);
	instructions[CPU::INS_TYA_IMP] = 
		makeIns("TYA", ADDR_MODE_IMP, 2, &CPU::ins_tya);
	instructions[CPU::INS_STA_ABY] = 
		makeIns("STA", ADDR_MODE_ABY, 5, &CPU::ins_sta);
	instructions[CPU::INS_TXS_IMP] = 
		makeIns("TXS", ADDR_MODE_IMP, 2, &CPU::ins_txs);
	instructions[CPU::INS_STA_ABX] = 
		makeIns("STA", ADDR_MODE_ABX, 5, &CPU::ins_sta);
	instructions[CPU::INS_BPL_REL] = 
		makeIns("BPL", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bpl);
	instructions[CPU::INS_LDY_IMM] = 
		makeIns("LDY", ADDR_MODE_IMM, 2, &CPU::ins_ldy);
	instructions[CPU::INS_LDA_IDX] = 
		makeIns("LDA", ADDR_MODE_IDX, 6, &CPU::ins_lda);
	instructions[CPU::INS_LDX_IMM] = 
		makeIns("LDX", ADDR_MODE_IMM, 2, &CPU::ins_ldx);
	instructions[CPU::INS_LDY_ZP] = 
		makeIns("LDY", ADDR_MODE_ZP, 3, &CPU::ins_ldy);
	instructions[CPU::INS_LDA_ZP] = 
		makeIns("LDA", ADDR_MODE_ZP, 3, &CPU::ins_lda);
	instructions[CPU::INS_LDX_ZP] = 
		makeIns("LDX", ADDR_MODE_ZP, 3, &CPU::ins_ldx);
	instructions[CPU::INS_TAY_IMP] = 
		makeIns("TAY", ADDR_MODE_IMP, 2, &CPU::ins_tay);
	instructions[CPU::INS_LDA_IMM] = 
		makeIns("LDA", ADDR_MODE_IMM, 2, &CPU::ins_lda);
	instructions[CPU::INS_ORA_IDY] = 
		makeIns("ORA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_ora);
	instructions[CPU::INS_TAX_IMP] = 
		makeIns("TAX", ADDR_MODE_IMP, 2, &CPU::ins_tax);
	instructions[CPU::INS_LDY_ABS] = 
		makeIns("LDY", ADDR_MODE_ABS, 4, &CPU::ins_ldy);
	instructions[CPU::INS_LDA_ABS] = 
		makeIns("LDA", ADDR_MODE_ABS, 4, &CPU::ins_lda);
	instructions[CPU::INS_LDX_ABS] = 
		makeIns("LDX", ADDR_MODE_ABS, 4, &CPU::ins_ldx);
	instructions[CPU::INS_BCS_REL] = 
		makeIns("BCS", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bcs);
	instructions[CPU::INS_LDA_IDY] = 
		makeIns("LDA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_lda);
	instructions[CPU::INS_LDY_ZPX] = 
		makeIns("LDY", ADDR_MODE_ZPX, 4, &CPU::ins_ldy);
	instructions[CPU::INS_LDA_ZPX] = 
		makeIns("LDA", ADDR_MODE_ZPX, 4, &CPU::ins_lda);
	instructions[CPU::INS_LDX_ZPY] =
		makeIns("LDX", ADDR_MODE_ZPY, 4, &CPU::ins_ldx);
	instructions[CPU::INS_CLV_IMP] = 
		makeIns("CLV", ADDR_MODE_IMP, 2, &CPU::ins_clv);
	instructions[CPU::INS_LDA_ABY] = 
		makeIns("LDA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_lda);
	instructions[CPU::INS_TSX_IMP] = 
		makeIns("TSX", ADDR_MODE_IMP, 2, &CPU::ins_tsx);
	instructions[CPU::INS_LDY_ABX] = 
		makeIns("LDY", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_ldy);
	instructions[CPU::INS_LDA_ABX] = 
		makeIns("LDA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_lda);
	instructions[CPU::INS_LDX_ABY] = 
		makeIns("LDX", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_ldx);
	instructions[CPU::INS_CPY_IMM] = 
		makeIns("CPY", ADDR_MODE_IMM, 2, &CPU::ins_cpy);
	instructions[CPU::INS_CMP_IDX] = 
		makeIns("CMP", ADDR_MODE_IDX, 6, &CPU::ins_cmp);
	instructions[CPU::INS_CPY_ZP] = 
		makeIns("CPY", ADDR_MODE_ZP, 3, &CPU::ins_cpy);
	instructions[CPU::INS_CMP_ZP] = 
		makeIns("CMP", ADDR_MODE_ZP, 3, &CPU::ins_cmp);
	instructions[CPU::INS_DEC_ZP] = 
		makeIns("DEC", ADDR_MODE_ZP, 5, &CPU::ins_dec);
	instructions[CPU::INS_INY_IMP] = 
		makeIns("INY", ADDR_MODE_IMP, 2, &CPU::ins_iny);
	instructions[CPU::INS_CMP_IMM] = 
		makeIns("CMP", ADDR_MODE_IMM, 2, &CPU::ins_cmp);
	instructions[CPU::INS_DEX_IMP] = 
		makeIns("DEX", ADDR_MODE_IMP, 2, &CPU::ins_dex);
	instructions[CPU::INS_CPY_ABS] = 
		makeIns("CPY", ADDR_MODE_ABS, 4, &CPU::ins_cpy);
	instructions[CPU::INS_CMP_ABS] = 
		makeIns("CMP", ADDR_MODE_ABS, 4, &CPU::ins_cmp);
	instructions[CPU::INS_DEC_ABS] = 
		makeIns("DEC", ADDR_MODE_ABS, 6, &CPU::ins_dec);
	instructions[CPU::INS_BNE_REL] = 
		makeIns("BNE", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bne);
	instructions[CPU::INS_CMP_IDY] = 
		makeIns("CMP", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_cmp);
	instructions[CPU::INS_ORA_ZPX] = 
		makeIns("ORA", ADDR_MODE_ZPX, 4, &CPU::ins_ora);
	instructions[CPU::INS_CMP_ZPX] = 
		makeIns("CMP", ADDR_MODE_ZPX, 4, &CPU::ins_cmp);
	instructions[CPU::INS_DEC_ZPX] = 
		makeIns("DEC", ADDR_MODE_ZPX, 6, &CPU::ins_dec);
	instructions[CPU::INS_CLD_IMP] = 
		makeIns("CLD", ADDR_MODE_IMP, 2, &CPU::ins_cld);
	instructions[CPU::INS_CMP_ABY] = 
		makeIns("CMP", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_cmp);
	instructions[CPU::INS_ASL_ZPX] = 
		makeIns("ASL", ADDR_MODE_ZPX, 6, &CPU::ins_asl);
	instructions[CPU::INS_CMP_ABX] = 
		makeIns("CMP", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_cmp);
	instructions[CPU::INS_DEC_ABX] = 
		makeIns("DEC", ADDR_MODE_ABX, 7, &CPU::ins_dec);
	instructions[CPU::INS_CPX_IMM] = 
		makeIns("CPX", ADDR_MODE_IMM, 2, &CPU::ins_cpx);
	instructions[CPU::INS_SBC_IDX] = 
		makeIns("SBC", ADDR_MODE_IDX, 6, &CPU::ins_sbc);
	instructions[CPU::INS_CPX_ZP] = 
		makeIns("CPX", ADDR_MODE_ZP, 3, &CPU::ins_cpx);
	instructions[CPU::INS_SBC_ZP] = 
		makeIns("SBC", ADDR_MODE_ZP, 3, &CPU::ins_sbc);
	instructions[CPU::INS_INC_ZP] = 
		makeIns("INC", ADDR_MODE_ZP, 5, &CPU::ins_inc);
	instructions[CPU::INS_INX_IMP] = 
		makeIns("INX", ADDR_MODE_IMP, 2, &CPU::ins_inx);
	instructions[CPU::INS_SBC_IMM] = 
		makeIns("SBC", ADDR_MODE_IMM, 2, &CPU::ins_sbc);
	instructions[CPU::INS_NOP_IMP] = 
		makeIns("NOP", ADDR_MODE_IMP, 2, &CPU::ins_nop);
	instructions[CPU::INS_CPX_ABS] = 
		makeIns("CPX", ADDR_MODE_ABS, 4, &CPU::ins_cpx);
	instructions[CPU::INS_SBC_ABS] = 
		makeIns("SBC", ADDR_MODE_ABS, 4, &CPU::ins_sbc);
	instructions[CPU::INS_INC_ABS] = 
		makeIns("INC", ADDR_MODE_ABS, 6, &CPU::ins_inc);
	instructions[CPU::INS_CLC_IMP] = 
		makeIns("CLC", ADDR_MODE_IMP, 2, &CPU::ins_clc);
	instructions[CPU::INS_BEQ_REL] = 
		makeIns("BEQ", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_beq);
	instructions[CPU::INS_SBC_IDY] = 
		makeIns("SBC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_sbc);
	instructions[CPU::INS_SBC_ZPX] = 
		makeIns("SBC", ADDR_MODE_ZPX, 4, &CPU::ins_sbc);
	instructions[CPU::INS_INC_ZPX] = 
		makeIns("INC", ADDR_MODE_ZPX, 6, &CPU::ins_inc);
	instructions[CPU::INS_SED_IMP] = 
		makeIns("SED", ADDR_MODE_IMP, 2, &CPU::ins_sed);
	instructions[CPU::INS_SBC_ABY] = 
		makeIns("SBC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_sbc);
	instructions[CPU::INS_ORA_ABY] = 
		makeIns("ORA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_ora);
	instructions[CPU::INS_SBC_ABX] = 
		makeIns("SBC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_sbc);
	instructions[CPU::INS_INC_ABX] = 
		makeIns("INC", ADDR_MODE_ABX, 7, &CPU::ins_inc);
	instructions[CPU::INS_ORA_ABX] = 
		makeIns("ORA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_ora);
	instructions[CPU::INS_ASL_ABX] = 
		makeIns("ASL", ADDR_MODE_ABX, 7, &CPU::ins_asl);
	instructions[CPU::INS_JSR_ABS] = 
		makeIns("JSR", ADDR_MODE_ABS, 6, &CPU::ins_jsr);
	instructions[CPU::INS_AND_IDX] = 
		makeIns("AND", ADDR_MODE_IDX, 6, &CPU::ins_and);
	instructions[CPU::INS_BIT_ZP] = 
		makeIns("BIT", ADDR_MODE_ZP, 3, &CPU::ins_bit);
	instructions[CPU::INS_AND_ZP] = 
		makeIns("AND", ADDR_MODE_ZP, 3, &CPU::ins_and);
	instructions[CPU::INS_ROL_ZP] = 
		makeIns("ROL", ADDR_MODE_ZP, 5, &CPU::ins_rol);
	instructions[CPU::INS_PLP_IMP] = 
		makeIns("PLP", ADDR_MODE_IMP, 4, &CPU::ins_plp);
	instructions[CPU::INS_AND_IMM] = 
		makeIns("AND", ADDR_MODE_IMM, 2, &CPU::ins_and);
	instructions[CPU::INS_ROL_ACC] = 
		makeIns("ROL", ADDR_MODE_ACC, 2, &CPU::ins_rol);
	instructions[CPU::INS_BIT_ABS] = 
		makeIns("BIT", ADDR_MODE_ABS, 4, &CPU::ins_bit);
	instructions[CPU::INS_AND_ABS] = 
		makeIns("AND", ADDR_MODE_ABS, 4, &CPU::ins_and);
	instructions[CPU::INS_ROL_ABS] = 
		makeIns("ROL", ADDR_MODE_ABS, 6, &CPU::ins_rol);
	instructions[CPU::INS_BMI_REL] = 
		makeIns("BMI", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bmi);
	instructions[CPU::INS_AND_IDY] = 
		makeIns("AND", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_and);
	instructions[CPU::INS_ORA_ZP] = 
		makeIns("ORA", ADDR_MODE_ZP, 3, &CPU::ins_ora);
	instructions[CPU::INS_AND_ZPX] = 
		makeIns("AND", ADDR_MODE_ZPX, 4, &CPU::ins_and);
	instructions[CPU::INS_ROL_ZPX] = 
		makeIns("ROL", ADDR_MODE_ZPX, 6, &CPU::ins_rol);
	instructions[CPU::INS_SEC_IMP] = 
		makeIns("SEC", ADDR_MODE_IMP, 2, &CPU::ins_sec);
	instructions[CPU::INS_AND_ABY] = 
		makeIns("AND", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_and);
	instructions[CPU::INS_ASL_ZP] = 
		makeIns("ASL", ADDR_MODE_ZP, 5, &CPU::ins_asl);
	instructions[CPU::INS_AND_ABX] = 
		makeIns("AND", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_and);
	instructions[CPU::INS_ROL_ABX] = 
		makeIns("ROL", ADDR_MODE_ABX, 7, &CPU::ins_rol);
	instructions[CPU::INS_RTI_IMP] = 
		makeIns("RTI", ADDR_MODE_IMP, 6, &CPU::ins_rti);
	instructions[CPU::INS_EOR_IDX] = 
		makeIns("EOR", ADDR_MODE_IDX, 6, &CPU::ins_eor);
	instructions[CPU::INS_EOR_ZP] = 
		makeIns("EOR", ADDR_MODE_ZP, 3, &CPU::ins_eor);
	instructions[CPU::INS_LSR_ZP] = 
		makeIns("LSR", ADDR_MODE_ZP, 5, &CPU::ins_lsr);
	instructions[CPU::INS_PHA_IMP] = 
		makeIns("PHA", ADDR_MODE_IMP, 3, &CPU::ins_pha);
	instructions[CPU::INS_PLA_IMP] = 
		makeIns("PLA", ADDR_MODE_IMP, 4, &CPU::ins_pla);
	instructions[CPU::INS_EOR_IMM] = 
		makeIns("EOR", ADDR_MODE_IMM, 2, &CPU::ins_eor);
	instructions[CPU::INS_LSR_ACC] = 
		makeIns("LSR", ADDR_MODE_ACC, 2, &CPU::ins_lsr);
	instructions[CPU::INS_JMP_ABS] = 
		makeIns("JMP", ADDR_MODE_ABS, 3, &CPU::ins_jmp);
	instructions[CPU::INS_EOR_ABS] = 
		makeIns("EOR", ADDR_MODE_ABS, 4, &CPU::ins_eor);
	instructions[CPU::INS_LSR_ABS] = 
		makeIns("LSR", ADDR_MODE_ABS, 6, &CPU::ins_lsr);
	instructions[CPU::INS_PHP_IMP] = 
		makeIns("PHP", ADDR_MODE_IMP, 3, &CPU::ins_php);
	instructions[CPU::INS_BVC_REL] = 
		makeIns("BVC", ADDR_MODE_REL | CYCLE_BRANCH, 2, &CPU::ins_bvc);
	instructions[CPU::INS_EOR_IDY] = 
		makeIns("EOR", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, &CPU::ins_eor);
	instructions[CPU::INS_EOR_ZPX] = 
		makeIns("EOR", ADDR_MODE_ZPX, 4, &CPU::ins_eor);
	instructions[CPU::INS_LSR_ZPX] = 
		makeIns("LSR", ADDR_MODE_ZPX, 6, &CPU::ins_lsr);
	instructions[CPU::INS_CLI_IMP] = 
		makeIns("CLI", ADDR_MODE_IMP, 2, &CPU::ins_cli);
	instructions[CPU::INS_EOR_ABY] = 
		makeIns("EOR", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, &CPU::ins_eor);
	instructions[CPU::INS_ORA_IMM] = 
		makeIns("ORA", ADDR_MODE_IMM, 2, &CPU::ins_ora);
	instructions[CPU::INS_EOR_ABX] = 
		makeIns("EOR", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, &CPU::ins_eor);
	instructions[CPU::INS_LSR_ABX] = 
		makeIns("LSR", ADDR_MODE_ABX, 7, &CPU::ins_lsr);
	instructions[CPU::INS_RTS_IMP] = 
		makeIns("RTS", ADDR_MODE_IMP, 6, &CPU::ins_rts);
	instructions[CPU::INS_ADC_IDX] = 
		makeIns("ADC", ADDR_MODE_IDX, 6, &CPU::ins_adc);

}

void CPU::Reset(Word ResetVector) {
	PC = ResetVector;
	SP = INITIAL_SP;
	A = X = Y = 0;
	PS = 0; //C = Z = I = D = B = V = N = 0;
	setupInstructionMap();
}

void CPU::Exception() {
	printf("CPU Exception");
	throw -1;
}

void CPU::SetFlagZ(Byte val) {
	Flags.Z = (val == 0);
}

void CPU::SetFlagN(Byte val) {
	Flags.N = (val & NegativeBit) >> 7;
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

Byte CPU::ReadByteAtPC() {
	Byte opcode = ReadByte(PC);
	PC++;
	return opcode;
}

void CPU::PushWord(Word value) {
	Byte b;

	b = (Byte) value & 0xff;
	Push(b);
	b = (Byte) (value >> 8) & 0xff;
	Push(b);
}

Word CPU::PopWord() {
	Word w;
	w = (Pop() << 8) | Pop();
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

void CPU::dumpstack() {
	Byte p = SP | STACK_FRAME;
	Word a;
	printf("---\nStack dump:\n");
	printf("STACK POINTER: %02x\n", SP);
	while (p != 0xff) {
		p++;
		a = STACK_FRAME | p;
		printf("[%04x] = %02x\n", a, mem->ReadByte(a));
	}
	printf("---\n");
}
	

Word CPU::getAddress(unsigned long mode, Byte &expectedCycles) {
	Word address, addrmode, flags;
	SByte rel;
	
	addrmode = mode &  0b00111111111111;
	flags    = mode & ~0b00111111111111;
	
	switch (addrmode) {
	// ZeroPage mode (tested)
	case ADDR_MODE_ZP:
		address = ReadByteAtPC();
		break;

	// ZeroPage,X (tested)
	case ADDR_MODE_ZPX:
		address = ReadByteAtPC();
		address += X;
		Cycles++;
		break;

        // ZeroPage,Y (tested)
	case ADDR_MODE_ZPY:
		address = ReadByteAtPC();
		address += Y;
		Cycles++;
		break;
		
	case ADDR_MODE_REL:
		rel = SByte(ReadByteAtPC());
		address = PC + rel;
		printf("pc = %d, rel = %d, address = %d\n", PC, rel, address);
	
		break;

	// Absolute (tested)
	case ADDR_MODE_ABS:
		address = ReadWord(PC);
		PC += 2;
		break;

	// Absolute,X (tested)
	case ADDR_MODE_ABX:
		address = ReadWord(PC);
		PC += 2;
		// Add a cycle if a page boundry is crossed
		if ((flags & CYCLE_CROSS_PAGE) && ((address + X) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += X;
		break;

	// Absolute,Y (tested)
	case ADDR_MODE_ABY:
		address = ReadWord(PC);
		PC += 2;
		// Add a cycle if a page boundry is crossed
		if ((flags & CYCLE_CROSS_PAGE) && ((address + Y) >> 8) !=
		    (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		address += Y;
		break;

	case ADDR_MODE_IND:
		address = ReadWord(PC);
		PC += 2;
		break;

        // (Indirect,X) or Indexed Indirect (tested)
	case ADDR_MODE_IDX:	
		address = ReadByteAtPC() + X;
		if (address > 0xFF)
			address -= 0xFF;
		address = ReadWord(address);
		Cycles++;
		break;

	// (Indirect),Y or Indirect Indexed (tested)
	case ADDR_MODE_IDY:
		address = ReadByteAtPC();
		address = ReadWord(address);
		address += Y;
		break;

	default:
		printf("Invalid addressing mode: 0x%ld\n", mode);
		Exception();
		break;
	}
	
	return address;
}

Byte CPU::getData(unsigned long mode, Byte &expectedCycles) {
	Byte data;
	Word address;
	Word addrmode;

	addrmode = mode & 0b00111111111111;
	
	switch (addrmode) {
	case ADDR_MODE_IMP:
	case ADDR_MODE_ACC:
		return 0;

	// Immediate mode (tested)
	case ADDR_MODE_IMM:
		data = ReadByteAtPC();
		break;

	default:
		address = getAddress(mode, expectedCycles);
		data = ReadByte(address);
		break;
	}

	return data;
}

std::tuple<CPU::Cycles_t, CPU::Cycles_t> CPU::ExecuteOneInstruction() {
	Byte opcode;
	Cycles_t startCycles = Cycles;
	unsigned long addrmode;
	Byte expectedCyclesToUse;
	opfn_t op;

	opcode = ReadByteAtPC();

	if (instructions.count(opcode) == 0) {
		printf("Invalid opcode 0x%x\n", opcode);
		Exception();
	}

	addrmode = instructions[opcode].addrmode;
	expectedCyclesToUse = instructions[opcode].cycles;
	op = instructions[opcode].opfn;

	(this->*op)(addrmode, expectedCyclesToUse);

	return std::make_tuple(Cycles - startCycles,
			       expectedCyclesToUse);
}

