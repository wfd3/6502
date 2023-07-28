#include <stdio.h>
#include <map>
#include <bitset>
#include <functional>
#include "6502.h"

extern mos6502::MEMORY mem;
extern mos6502::CPU cpu;

constexpr static unsigned long ADDR_MODE_IMM    = 1 << 0;  // Immediate
constexpr static unsigned long ADDR_MODE_ZP     = 1 << 1;  // Zero Page
constexpr static unsigned long ADDR_MODE_ZPX    = 1 << 2;  // Zero Page,X
constexpr static unsigned long ADDR_MODE_ZPY    = 1 << 3;  // Zero Page,Y
constexpr static unsigned long ADDR_MODE_REL    = 1 << 4;  // Relative
constexpr static unsigned long ADDR_MODE_ABS    = 1 << 5;  // Absolute
constexpr static unsigned long ADDR_MODE_ABX    = 1 << 6;  // Absolute,X
constexpr static unsigned long ADDR_MODE_ABY    = 1 << 7;  // Absolute,y
constexpr static unsigned long ADDR_MODE_IND    = 1 << 8;  // Indirect
constexpr static unsigned long ADDR_MODE_IDX    = 1 << 9;  // Indexed Ind
constexpr static unsigned long ADDR_MODE_IDY    = 1 << 10; // Indirect Idx
constexpr static unsigned long ADDR_MODE_IMP    = 1 << 11; // Implied
constexpr static unsigned long ADDR_MODE_ACC    = 1 << 12; // Accumulator

// How the CPU should add cycle counts on branches and when
// instructions fetch data across page boundries.
constexpr static unsigned long CYCLE_BRANCH     = 1 << 13;
constexpr static unsigned long CYCLE_CROSS_PAGE = 1 << 14;
	
/////////////////////////////////////////////////////////////////////////

static void ins_adc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse) {
}

static void ins_and(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse) {
	mos6502::Byte data;

	data = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->A &= data;
	cpu->SetFlagsForRegister(cpu->A);
}

static void ins_asl(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bcc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bcs(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_beq(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}

static void ins_bit(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	mos6502::Byte data;

	data = cpu->getData(addrmode, expectedCyclesToUse);
	data &= cpu->A;
	cpu->Flags.Z = (data == 0);
	cpu->Flags.V = (data & (1 << 6)) == (1 << 6);
	cpu->Flags.N = (data & (1 << 7)) == (1 << 7);
}

static void ins_bmi(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bne(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bpl(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_brk(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bvc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_bvs(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_clc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.C = 0;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_cld(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.D = 0;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_cli(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.I = 0;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_clv(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.V = 0;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_cmp(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_cpx(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_cpy(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_dec(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_dex(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_dey(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_eor(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	mos6502::Byte data;

	data = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->A ^= data;
	cpu->SetFlagsForRegister(cpu->A);
}
static void ins_inc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_inx(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_iny(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_jmp(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_jsr(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_lda(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->A = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->SetFlagsForRegister(cpu->A);
}
static void ins_ldx(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->X = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->SetFlagsForRegister(cpu->X);
}
static void ins_ldy(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Y = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->SetFlagsForRegister(cpu->Y);
}
static void ins_lsr(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_nop(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	// NOP, like all single byte instructions, takes
	// two cycles.
	cpu->Cycles++;
}

static void ins_ora(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	mos6502::Byte data;

	data = cpu->getData(addrmode, expectedCyclesToUse);
	cpu->A |= data;
	cpu->SetFlagsForRegister(cpu->A);
}

static void ins_pha(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Push(cpu->A);
	cpu->Cycles++;		// Single byte instruction
}

static void ins_pla(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->A = cpu->Pop();
	cpu->SetFlagsForRegister(cpu->A);
	cpu->Cycles += 2;      
}

static void ins_php(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Push(cpu->PS);
	cpu->Cycles++;		// Single byte instruction
}
static void ins_plp(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->PS = cpu->Pop();
	cpu->Cycles += 2;
}
static void ins_rol(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_ror(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_rti(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_rts(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_sbc(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_sec(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.C = 1;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_sed(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.D = 1;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_sei(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){
	cpu->Flags.I = 1;
	cpu->Cycles++;		// Single byte instruction
}
static void ins_sta(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_stx(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_sty(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_tax(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_tay(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_tsx(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_txa(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_txs(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}
static void ins_tya(mos6502::CPU *cpu, unsigned long addrmode,
		    mos6502::Byte &expectedCyclesToUse){}

/////////////////////////////////////////////////////////////////////////

struct mos6502::CPU::instruction makeIns(const char *name, unsigned long addrmode,
				mos6502::Byte cycles, mos6502::CPU::opfn_t opfn) {
	struct mos6502::CPU::instruction ins = { name, addrmode, cycles, opfn };
	return ins;
}

void setupInstructionMap() {
	cpu.instructions[mos6502::CPU::INS_BRK_IMP] = 
		makeIns("BRK", ADDR_MODE_IMP, 7, ins_brk);
	cpu.instructions[mos6502::CPU::INS_ORA_IDX] = 
		makeIns("ORA", ADDR_MODE_IDX, 6, ins_ora);
	cpu.instructions[mos6502::CPU::INS_ASL_ACC] = 
		makeIns("ASL", ADDR_MODE_ACC, 2, ins_asl);
	cpu.instructions[mos6502::CPU::INS_ADC_ZP] = 
		makeIns("ADC", ADDR_MODE_ZP, 3, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ROR_ZP] = 
		makeIns("ROR", ADDR_MODE_ZP, 5, ins_ror);
	cpu.instructions[mos6502::CPU::INS_ADC_IMM] = 
		makeIns("ADC", ADDR_MODE_IMM, 2, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ROR_ACC] = 
		makeIns("ROR", ADDR_MODE_ACC, 2, ins_ror);
	cpu.instructions[mos6502::CPU::INS_JMP_IND] = 
		makeIns("JMP", ADDR_MODE_IND, 5, ins_jmp);
	cpu.instructions[mos6502::CPU::INS_ADC_ABS] = 
		makeIns("ADC", ADDR_MODE_ABS, 4, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ROR_ABS] = 
		makeIns("ROR", ADDR_MODE_ABS, 6, ins_ror);
	cpu.instructions[mos6502::CPU::INS_BVS_REL] = 
		makeIns("BVS", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bvs);
	cpu.instructions[mos6502::CPU::INS_ADC_IDY] = 
		makeIns("ADC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ADC_ZPX] = 
		makeIns("ADC", ADDR_MODE_ZPX, 4, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ROR_ZPX] = 
		makeIns("ROR", ADDR_MODE_ZPX, 6, ins_ror);
	cpu.instructions[mos6502::CPU::INS_SEI_IMP] = 
		makeIns("SEI", ADDR_MODE_IMP, 2, ins_sei);
	cpu.instructions[mos6502::CPU::INS_ADC_ABY] = 
		makeIns("ADC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ADC_ABX] = 
		makeIns("ADC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_adc);
	cpu.instructions[mos6502::CPU::INS_ROR_ABX] = 
		makeIns("ROR", ADDR_MODE_ABX, 7, ins_ror);
	cpu.instructions[mos6502::CPU::INS_STA_IDX] = 
		makeIns("STA", ADDR_MODE_IDX, 6, ins_sta);
	cpu.instructions[mos6502::CPU::INS_ORA_ABS] = 
		makeIns("ORA", ADDR_MODE_ABS, 4, ins_ora);
	cpu.instructions[mos6502::CPU::INS_STY_ZP] = 
		makeIns("STY", ADDR_MODE_ZP, 3, ins_sty);
	cpu.instructions[mos6502::CPU::INS_STA_ZP] = 
		makeIns("STA", ADDR_MODE_ZP, 3, ins_sta);
	cpu.instructions[mos6502::CPU::INS_STX_ZP] = 
		makeIns("STX", ADDR_MODE_ZP, 3, ins_stx);
	cpu.instructions[mos6502::CPU::INS_DEY_IMP] = 
		makeIns("DEY", ADDR_MODE_IMP, 2, ins_dey);
	cpu.instructions[mos6502::CPU::INS_TXA_IMP] = 
		makeIns("TXA", ADDR_MODE_IMP, 2, ins_txa);
	cpu.instructions[mos6502::CPU::INS_ASL_ABS] = 
		makeIns("ASL", ADDR_MODE_ABS, 6, ins_asl);
	cpu.instructions[mos6502::CPU::INS_STY_ABS] = 
		makeIns("STY", ADDR_MODE_ABS, 4, ins_sty);
	cpu.instructions[mos6502::CPU::INS_STA_ABS] = 
		makeIns("STA", ADDR_MODE_ABS, 4, ins_sta);
	cpu.instructions[mos6502::CPU::INS_STX_ABS] = 
		makeIns("STX", ADDR_MODE_ABS, 4, ins_stx);
	cpu.instructions[mos6502::CPU::INS_BCC_REL] = 
		makeIns("BCC", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bcc);
	cpu.instructions[mos6502::CPU::INS_STA_IDY] = 
		makeIns("STA", ADDR_MODE_IDY, 6, ins_sta);
	cpu.instructions[mos6502::CPU::INS_STY_ZPX] = 
		makeIns("STY", ADDR_MODE_ZPX, 4, ins_sty);
	cpu.instructions[mos6502::CPU::INS_STA_ZPX] = 
		makeIns("STA", ADDR_MODE_ZPX, 4, ins_sta);
	cpu.instructions[mos6502::CPU::INS_STX_ZPY] = 
		makeIns("STX", ADDR_MODE_ZPY, 4, ins_stx);
	cpu.instructions[mos6502::CPU::INS_TYA_IMP] = 
		makeIns("TYA", ADDR_MODE_IMP, 2, ins_tya);
	cpu.instructions[mos6502::CPU::INS_STA_ABY] = 
		makeIns("STA", ADDR_MODE_ABY, 5, ins_sta);
	cpu.instructions[mos6502::CPU::INS_TXS_IMP] = 
		makeIns("TXS", ADDR_MODE_IMP, 2, ins_txs);
	cpu.instructions[mos6502::CPU::INS_STA_ABX] = 
		makeIns("STA", ADDR_MODE_ABX, 5, ins_sta);
	cpu.instructions[mos6502::CPU::INS_BPL_REL] = 
		makeIns("BPL", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bpl);
	cpu.instructions[mos6502::CPU::INS_LDY_IMM] = 
		makeIns("LDY", ADDR_MODE_IMM, 2, ins_ldy);
	cpu.instructions[mos6502::CPU::INS_LDA_IDX] = 
		makeIns("LDA", ADDR_MODE_IDX, 6, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDX_IMM] = 
		makeIns("LDX", ADDR_MODE_IMM, 2, ins_ldx);
	cpu.instructions[mos6502::CPU::INS_LDY_ZP] = 
		makeIns("LDY", ADDR_MODE_ZP, 3, ins_ldy);
	cpu.instructions[mos6502::CPU::INS_LDA_ZP] = 
		makeIns("LDA", ADDR_MODE_ZP, 3, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDX_ZP] = 
		makeIns("LDX", ADDR_MODE_ZP, 3, ins_ldx);
	cpu.instructions[mos6502::CPU::INS_TAY_IMP] = 
		makeIns("TAY", ADDR_MODE_IMP, 2, ins_tay);
	cpu.instructions[mos6502::CPU::INS_LDA_IMM] = 
		makeIns("LDA", ADDR_MODE_IMM, 2, ins_lda);
	cpu.instructions[mos6502::CPU::INS_ORA_IDY] = 
		makeIns("ORA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_ora);
	cpu.instructions[mos6502::CPU::INS_TAX_IMP] = 
		makeIns("TAX", ADDR_MODE_IMP, 2, ins_tax);
	cpu.instructions[mos6502::CPU::INS_LDY_ABS] = 
		makeIns("LDY", ADDR_MODE_ABS, 4, ins_ldy);
	cpu.instructions[mos6502::CPU::INS_LDA_ABS] = 
		makeIns("LDA", ADDR_MODE_ABS, 4, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDX_ABS] = 
		makeIns("LDX", ADDR_MODE_ABS, 4, ins_ldx);
	cpu.instructions[mos6502::CPU::INS_BCS_REL] = 
		makeIns("BCS", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bcs);
	cpu.instructions[mos6502::CPU::INS_LDA_IDY] = 
		makeIns("LDA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDY_ZPX] = 
		makeIns("LDY", ADDR_MODE_ZPX, 4, ins_ldy);
	cpu.instructions[mos6502::CPU::INS_LDA_ZPX] = 
		makeIns("LDA", ADDR_MODE_ZPX, 4, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDX_ZPY] =
		makeIns("LDX", ADDR_MODE_ZPY, 4, ins_ldx);
	cpu.instructions[mos6502::CPU::INS_CLV_IMP] = 
		makeIns("CLV", ADDR_MODE_IMP, 2, ins_clv);
	cpu.instructions[mos6502::CPU::INS_LDA_ABY] = 
		makeIns("LDA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_lda);
	cpu.instructions[mos6502::CPU::INS_TSX_IMP] = 
		makeIns("TSX", ADDR_MODE_IMP, 2, ins_tsx);
	cpu.instructions[mos6502::CPU::INS_LDY_ABX] = 
		makeIns("LDY", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4,  ins_ldy);
	cpu.instructions[mos6502::CPU::INS_LDA_ABX] = 
		makeIns("LDA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_lda);
	cpu.instructions[mos6502::CPU::INS_LDX_ABY] = 
		makeIns("LDX", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_ldx);
	cpu.instructions[mos6502::CPU::INS_CPY_IMM] = 
		makeIns("CPY", ADDR_MODE_IMM, 2, ins_cpy);
	cpu.instructions[mos6502::CPU::INS_CMP_IDX] = 
		makeIns("CMP", ADDR_MODE_IDX, 6, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_CPY_ZP] = 
		makeIns("CPY", ADDR_MODE_ZP, 3, ins_cpy);
	cpu.instructions[mos6502::CPU::INS_CMP_ZP] = 
		makeIns("CMP", ADDR_MODE_ZP, 3, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_DEC_ZP] = 
		makeIns("DEC", ADDR_MODE_ZP, 5, ins_dec);
	cpu.instructions[mos6502::CPU::INS_INY_IMP] = 
		makeIns("INY", ADDR_MODE_IMP, 2, ins_iny);
	cpu.instructions[mos6502::CPU::INS_CMP_IMM] = 
		makeIns("CMP", ADDR_MODE_IMM, 2, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_DEX_IMP] = 
		makeIns("DEX", ADDR_MODE_IMP, 2, ins_dex);
	cpu.instructions[mos6502::CPU::INS_CPY_ABS] = 
		makeIns("CPY", ADDR_MODE_ABS, 4, ins_cpy);
	cpu.instructions[mos6502::CPU::INS_CMP_ABS] = 
		makeIns("CMP", ADDR_MODE_ABS, 4, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_DEC_ABS] = 
		makeIns("DEC", ADDR_MODE_ABS, 6, ins_dec);
	cpu.instructions[mos6502::CPU::INS_BNE_REL] = 
		makeIns("BNE", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bne);
	cpu.instructions[mos6502::CPU::INS_CMP_IDY] = 
		makeIns("CMP", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_ORA_ZPX] = 
		makeIns("ORA", ADDR_MODE_ZPX, 4, ins_ora);
	cpu.instructions[mos6502::CPU::INS_CMP_ZPX] = 
		makeIns("CMP", ADDR_MODE_ZPX, 4, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_DEC_ZPX] = 
		makeIns("DEC", ADDR_MODE_ZPX, 6, ins_dec);
	cpu.instructions[mos6502::CPU::INS_CLD_IMP] = 
		makeIns("CLD", ADDR_MODE_IMP, 2, ins_cld);
	cpu.instructions[mos6502::CPU::INS_CMP_ABY] = 
		makeIns("CMP", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_ASL_ZPX] = 
		makeIns("ASL", ADDR_MODE_ZPX, 6, ins_asl);
	cpu.instructions[mos6502::CPU::INS_CMP_ABX] = 
		makeIns("CMP", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_cmp);
	cpu.instructions[mos6502::CPU::INS_DEC_ABX] = 
		makeIns("DEC", ADDR_MODE_ABX, 7, ins_dec);
	cpu.instructions[mos6502::CPU::INS_CPX_IMM] = 
		makeIns("CPX", ADDR_MODE_IMM, 2, ins_cpx);
	cpu.instructions[mos6502::CPU::INS_SBC_IDX] = 
		makeIns("SBC", ADDR_MODE_IDX, 6, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_CPX_ZP] = 
		makeIns("CPX", ADDR_MODE_ZP, 3, ins_cpx);
	cpu.instructions[mos6502::CPU::INS_SBC_ZP] = 
		makeIns("SBC", ADDR_MODE_ZP, 3, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_INC_ZP] = 
		makeIns("INC", ADDR_MODE_ZP, 5, ins_inc);
	cpu.instructions[mos6502::CPU::INS_INX_IMP] = 
		makeIns("INX", ADDR_MODE_IMP, 2, ins_inx);
	cpu.instructions[mos6502::CPU::INS_SBC_IMM] = 
		makeIns("SBC", ADDR_MODE_IMM, 2, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_NOP_IMP] = 
		makeIns("NOP", ADDR_MODE_IMP, 2, ins_nop);
	cpu.instructions[mos6502::CPU::INS_CPX_ABS] = 
		makeIns("CPX", ADDR_MODE_ABS, 4, ins_cpx);
	cpu.instructions[mos6502::CPU::INS_SBC_ABS] = 
		makeIns("SBC", ADDR_MODE_ABS, 4, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_INC_ABS] = 
		makeIns("INC", ADDR_MODE_ABS, 6, ins_inc);
	cpu.instructions[mos6502::CPU::INS_CLC_IMP] = 
		makeIns("CLC", ADDR_MODE_IMP, 2, ins_clc);
	cpu.instructions[mos6502::CPU::INS_BEQ_REL] = 
		makeIns("BEQ", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_beq);
	cpu.instructions[mos6502::CPU::INS_SBC_IDY] = 
		makeIns("SBC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_SBC_ZPX] = 
		makeIns("SBC", ADDR_MODE_ZPX, 4, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_INC_ZPX] = 
		makeIns("INC", ADDR_MODE_ZPX, 6, ins_inc);
	cpu.instructions[mos6502::CPU::INS_SED_IMP] = 
		makeIns("SED", ADDR_MODE_IMP, 2, ins_sed);
	cpu.instructions[mos6502::CPU::INS_SBC_ABY] = 
		makeIns("SBC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_ORA_ABY] = 
		makeIns("ORA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_ora);
	cpu.instructions[mos6502::CPU::INS_SBC_ABX] = 
		makeIns("SBC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_sbc);
	cpu.instructions[mos6502::CPU::INS_INC_ABX] = 
		makeIns("INC", ADDR_MODE_ABX, 7, ins_inc);
	cpu.instructions[mos6502::CPU::INS_ORA_ABX] = 
		makeIns("ORA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_ora);
	cpu.instructions[mos6502::CPU::INS_ASL_ABX] = 
		makeIns("ASL", ADDR_MODE_ABX, 7, ins_asl);
	cpu.instructions[mos6502::CPU::INS_JSR_ABS] = 
		makeIns("JSR", ADDR_MODE_ABS, 6, ins_jsr);
	cpu.instructions[mos6502::CPU::INS_AND_IDX] = 
		makeIns("AND", ADDR_MODE_IDX, 6, ins_and);
	cpu.instructions[mos6502::CPU::INS_BIT_ZP] = 
		makeIns("BIT", ADDR_MODE_ZP, 3, ins_bit);
	cpu.instructions[mos6502::CPU::INS_AND_ZP] = 
		makeIns("AND", ADDR_MODE_ZP, 3, ins_and);
	cpu.instructions[mos6502::CPU::INS_ROL_ZP] = 
		makeIns("ROL", ADDR_MODE_ZP, 5, ins_rol);
	cpu.instructions[mos6502::CPU::INS_PLP_IMP] = 
		makeIns("PLP", ADDR_MODE_IMP, 4, ins_plp);
	cpu.instructions[mos6502::CPU::INS_AND_IMM] = 
		makeIns("AND", ADDR_MODE_IMM, 2, ins_and);
	cpu.instructions[mos6502::CPU::INS_ROL_ACC] = 
		makeIns("ROL", ADDR_MODE_ACC, 2, ins_rol);
	cpu.instructions[mos6502::CPU::INS_BIT_ABS] = 
		makeIns("BIT", ADDR_MODE_ABS, 4, ins_bit);
	cpu.instructions[mos6502::CPU::INS_AND_ABS] = 
		makeIns("AND", ADDR_MODE_ABS, 4, ins_and);
	cpu.instructions[mos6502::CPU::INS_ROL_ABS] = 
		makeIns("ROL", ADDR_MODE_ABS, 6, ins_rol);
	cpu.instructions[mos6502::CPU::INS_BMI_REL] = 
		makeIns("BMI", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bmi);
	cpu.instructions[mos6502::CPU::INS_AND_IDY] = 
		makeIns("AND", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_and);
	cpu.instructions[mos6502::CPU::INS_ORA_ZP] = 
		makeIns("ORA", ADDR_MODE_ZP, 3, ins_ora);
	cpu.instructions[mos6502::CPU::INS_AND_ZPX] = 
		makeIns("AND", ADDR_MODE_ZPX, 4, ins_and);
	cpu.instructions[mos6502::CPU::INS_ROL_ZPX] = 
		makeIns("ROL", ADDR_MODE_ZPX, 6, ins_rol);
	cpu.instructions[mos6502::CPU::INS_SEC_IMP] = 
		makeIns("SEC", ADDR_MODE_IMP, 2, ins_sec);
	cpu.instructions[mos6502::CPU::INS_AND_ABY] = 
		makeIns("AND", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_and);
	cpu.instructions[mos6502::CPU::INS_ASL_ZP] = 
		makeIns("ASL", ADDR_MODE_ZP, 5, ins_asl);
	cpu.instructions[mos6502::CPU::INS_AND_ABX] = 
		makeIns("AND", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_and);
	cpu.instructions[mos6502::CPU::INS_ROL_ABX] = 
		makeIns("ROL", ADDR_MODE_ABX, 7, ins_rol);
	cpu.instructions[mos6502::CPU::INS_RTI_IMP] = 
		makeIns("RTI", ADDR_MODE_IMP, 6, ins_rti);
	cpu.instructions[mos6502::CPU::INS_EOR_IDX] = 
		makeIns("EOR", ADDR_MODE_IDX, 6, ins_eor);
	cpu.instructions[mos6502::CPU::INS_EOR_ZP] = 
		makeIns("EOR", ADDR_MODE_ZP, 3, ins_eor);
	cpu.instructions[mos6502::CPU::INS_LSR_ZP] = 
		makeIns("LSR", ADDR_MODE_ZP, 5, ins_lsr);
	cpu.instructions[mos6502::CPU::INS_PHA_IMP] = 
		makeIns("PHA", ADDR_MODE_IMP, 3, ins_pha);
	cpu.instructions[mos6502::CPU::INS_PLA_IMP] = 
		makeIns("PLA", ADDR_MODE_IMP, 4, ins_pla);
	cpu.instructions[mos6502::CPU::INS_EOR_IMM] = 
		makeIns("EOR", ADDR_MODE_IMM, 2, ins_eor);
	cpu.instructions[mos6502::CPU::INS_LSR_ACC] = 
		makeIns("LSR", ADDR_MODE_ACC, 2, ins_lsr);
	cpu.instructions[mos6502::CPU::INS_JMP_ABS] = 
		makeIns("JMP", ADDR_MODE_ABS, 3, ins_jmp);
	cpu.instructions[mos6502::CPU::INS_EOR_ABS] = 
		makeIns("EOR", ADDR_MODE_ABS, 4, ins_eor);
	cpu.instructions[mos6502::CPU::INS_LSR_ABS] = 
		makeIns("LSR", ADDR_MODE_ABS, 6, ins_lsr);
	cpu.instructions[mos6502::CPU::INS_PHP_IMP] = 
		makeIns("PHP", ADDR_MODE_IMP, 3, ins_php);
	cpu.instructions[mos6502::CPU::INS_BVC_REL] = 
		makeIns("BVC", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bvc);
	cpu.instructions[mos6502::CPU::INS_EOR_IDY] = 
		makeIns("EOR", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_eor);
	cpu.instructions[mos6502::CPU::INS_EOR_ZPX] = 
		makeIns("EOR", ADDR_MODE_ZPX, 4, ins_eor);
	cpu.instructions[mos6502::CPU::INS_LSR_ZPX] = 
		makeIns("LSR", ADDR_MODE_ZPX, 6, ins_lsr);
	cpu.instructions[mos6502::CPU::INS_CLI_IMP] = 
		makeIns("CLI", ADDR_MODE_IMP, 2, ins_cli);
	cpu.instructions[mos6502::CPU::INS_EOR_ABY] = 
		makeIns("EOR", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_eor);
	cpu.instructions[mos6502::CPU::INS_ORA_IMM] = 
		makeIns("ORA", ADDR_MODE_IMM, 2, ins_ora);
	cpu.instructions[mos6502::CPU::INS_EOR_ABX] = 
		makeIns("EOR", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_eor);
	cpu.instructions[mos6502::CPU::INS_LSR_ABX] = 
		makeIns("LSR", ADDR_MODE_ABX, 7, ins_lsr);
	cpu.instructions[mos6502::CPU::INS_RTS_IMP] = 
		makeIns("RTS", ADDR_MODE_IMP, 6, ins_rts);
	cpu.instructions[mos6502::CPU::INS_ADC_IDX] = 
		makeIns("ADC", ADDR_MODE_IDX, 6, ins_adc);

}

void mos6502::CPU::Reset(mos6502::Word ResetVector) {
	PC = ResetVector;
	SP = INITIAL_SP;
	A = X = Y = 0;
	PS = 0; //C = Z = I = D = B = V = N = 0;
	setupInstructionMap();
}

void mos6502::CPU::Exception() {
	printf("mos6502::CPU Exception");
	throw -1;
}

void mos6502::CPU::SetFlagsForRegister(mos6502::Byte b) {
	Flags.Z = (b == 0);
	Flags.N = (b & 0b10000000) >> 7;
}

void mos6502::CPU::SetFlagsForCompare(mos6502::Byte b, mos6502::Byte v) {
	Flags.C = (b >= v);
	Flags.Z = b == v;
	Flags.N = (b & 0b10000000) >> 7;
}

mos6502::Byte mos6502::CPU::ReadByte(mos6502::Word address) {
	mos6502::Byte data = mem.ReadByte(address);
	Cycles++;
	return data;
}

void mos6502::CPU::WriteByte(mos6502::Word address, mos6502::Byte value) {
	mem.WriteByte(address, value);
	Cycles++;
}

mos6502::Word  mos6502::CPU::ReadWord(mos6502::Word address) {
	mos6502::Word w = ReadByte(address) | (ReadByte(address + 1) << 8);
	return w;
}

mos6502::Byte mos6502::CPU::FetchIns() {
	mos6502::Byte opcode = ReadByte(PC);
	PC++;
	return opcode;
}

void mos6502::CPU::PushWord(mos6502::Word value) {
	mos6502::Byte b;

	b = (mos6502::Byte) value & 0xff;
	Push(b);
	b = (mos6502::Byte) (value >> 8) & 0xff;
	Push(b);
}

mos6502::Word mos6502::CPU::PopWord() {
	mos6502::Word w;
	w = Pop() | (Pop() << 8);
	return w;
}

void mos6502::CPU::Push(mos6502::Byte value) {
	mos6502::Word SPAddress = mos6502::CPU::STACK_FRAME + SP;
	WriteByte(SPAddress, value);
	SP--;
}

mos6502::Byte mos6502::CPU::Pop() {
	mos6502::Word SPAddress;
	SP++;
	SPAddress = STACK_FRAME + SP;
	return ReadByte(SPAddress);
}

mos6502::Byte mos6502::CPU::getData(unsigned long mode, mos6502::Byte &expectedCycles) {
	mos6502::Byte data;
	mos6502::Word address, addrmode, flags;
	
	addrmode = mode &  0b00111111111111;
	flags    = mode & ~0b00111111111111;
	
	switch (addrmode) {
	case ADDR_MODE_IMP:
		return 0;
		
	case ADDR_MODE_ACC:
		return 0;

	// Immediate mode (tested)
	case ADDR_MODE_IMM:
		data = FetchIns();
		break;

	// ZeroPage mode (tested)
	case ADDR_MODE_ZP:
		address = FetchIns();
		data = ReadByte(address);
		break;

	// ZeroPage,X (tested)
	case ADDR_MODE_ZPX:
		address = FetchIns();
		address += X; Cycles++;
		data = ReadByte(address);
		break;

        // ZeroPage,Y (tested)
	case ADDR_MODE_ZPY:
		data = FetchIns();
		data += Y;
		Cycles += 2;
		break;

	case ADDR_MODE_REL:
		data = FetchIns();
		data += PC;
		break;

	// Absolute (tested)
	case ADDR_MODE_ABS:
		address = ReadWord(PC);
		PC += 2;
		data = ReadByte(address);
		break;

	// Absolute,X (tested)
	case ADDR_MODE_ABX:
		address = ReadWord(PC);
		PC += 2;
		// Check access across page boundry, add a cycle if we do.
		if (flags & CYCLE_CROSS_PAGE &&
		    ((address + X) >> 8) != (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		data = ReadByte(address + X);
		break;

	// Absolute,Y (tested)
	case ADDR_MODE_ABY:
		address = ReadWord(PC);
		PC += 2;
		// Check access across page boundry, add a cycle if we do.
		if (flags & CYCLE_CROSS_PAGE &&
		    ((address + Y) >> 8) != (address >> 8)) {
			expectedCycles++;
			Cycles++;
		}
		data = ReadByte(address + Y);
		break;

	case ADDR_MODE_IND:
		return 0;	// Only used by JMP

        // (Indirect,X) or Indexed Indirect (tested)
	case ADDR_MODE_IDX:	
		address = FetchIns() + X;
		data = ReadByte(address);
		Cycles += 3;	// HACK
		break;

	// (Indirect),Y or Indirect Indexed
	case ADDR_MODE_IDY:
		address = FetchIns();
		address = ReadWord(address);
		address += Y;
		data = ReadByte(address);
		break;

	default:
		printf("Invalid addressing mode: 0x%ld\n", mode);
		Exception();
		break;
	}
	
	return data;
}

std::tuple<mos6502::Cycles_t, mos6502::Cycles_t>
mos6502::CPU::ExecuteOneInstruction() {
	struct instruction ins;
	mos6502::Byte opcode;
	mos6502::Byte expectedCyclesToUse;
	mos6502::Cycles_t startCycles = Cycles;

	opcode = FetchIns();

	if (instructions.count(opcode) == 0) {
		printf("Invalid opcode 0x%x\n", opcode);
		Exception();
	}
	
	ins = cpu.instructions[opcode];
	expectedCyclesToUse = ins.cycles;
	ins.opfn(this, ins.addrmode, expectedCyclesToUse);

	return std::make_tuple(Cycles - startCycles,
			       expectedCyclesToUse);
}

#if 0
/////////////////////////////////////////////////////////////////////////
// Tests

void and_imm() {
	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xFFFC] = mos6502::CPU::INS_AND_IMM;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;
	mos6502::Cycles_t StartC;

	StartC = cpu.Cycles;
	cpu.ExecuteOneInstruction();

	printf("and_imm: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", cpu.Cycles - StartC);

	if (cpu.A == 0x0f) {
		printf("PASSED\n");
	} else {
		printf("FAILED - Expected A == 0x0f\n");
		throw -1;
	}
}


void and_zp() {
	mem.Init();
	cpu.Reset(INITIAL_PC);

	mem[0xFFFC] = mos6502::CPU::INS_AND_ZP;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;
	mos6502::Cycles_t StartC;

	StartC = cpu.Cycles;

	cpu.ExecuteOneInstruction();

	printf("and_zp: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", cpu.Cycles - StartC);
	if (cpu.A == 0x0f) {
		printf("PASSED\n");
	} else {
		printf("FAILED - Expected A == 0x0f\n");
		throw -1;
	}
}

void and_z_flag_is_unset() {
	mos6502::Cycles_t StartC;

	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xFFFC] = 0x29;
	mem[0xFFFD] = 0xFF;
	cpu.A = 0xFF;

	StartC = cpu.Cycles;
	cpu.ExecuteOneInstruction();

	printf("and_set_z_flag: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", cpu.Cycles - StartC);
	if (cpu.A != 0 && cpu.Z == 0) {
		printf("PASSED\n");
	} else {
		printf("FAILED\n");
		throw -1;
	}
}

void and_z_flag_is_set() {
	mos6502::Cycles_t StartC;

	mem.Init();
	cpu.Reset(INITIAL_PC);

	mem[0xFFFC] = 0x29;
	mem[0xFFFD] = 0x00;
	cpu.A = 0xFF;

	StartC = cpu.Cycles;
	cpu.ExecuteOneInstruction();

	printf("and_set_z_flag: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", cpu.Cycles - StartC);
	if (cpu.A == 0 && cpu.Z == 1) {
		printf("PASSED\n");
	} else {
		printf("FAILED\n");
		throw -1;
	}
		     }


void ora_imm() {
	mos6502::Cycles_t StartC;

	printf("======= ora_imm():\n");
	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xfffc] = 0x09;
	mem[0xfffd] = 0x10;
	cpu.A = 0x00;
	
	StartC = cpu.Cycles;
	cpu.ExecuteOneInstruction();

	printf("ora_imm: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);

}


void ora_zp() {
	mos6502::Cycles_t StartC;

	printf("======= ora_zp():\n");
	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xfffc] = 0x05;
	mem[0xfffd] = 0x10;
	mem[0x0010] == 0x00;
	cpu.A = 0x00;
	
	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("ora_zp: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);

}

void pha() {
	mos6502::Cycles_t StartC;

	printf("======= pha():\n");
	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xfffc] = 0x48;
	cpu.A = 0x10;
	
	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("ora_zp: SP_START = 0x%x\n", mem[cpu.INITIAL_SP]);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);
}

int main() {
	printf("MOS 6502\n");
	and_imm();
	and_zp();
	and_z_flag_is_unset();
	and_z_flag_is_set();
//
//	ora_imm();
//	ora_zp();
//	pha();
}

	
#endif       
