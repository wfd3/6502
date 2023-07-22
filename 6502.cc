#include <stdio.h>
#include <map>
#include <bitset>
#include <functional>
#include "6502.h"

extern MEMORY mem;
extern CPU cpu;

constexpr static unsigned long ADDR_MODE_IMM = 1 << 0;  // Immediate
constexpr static unsigned long ADDR_MODE_ZP  = 1 << 1;  // Zero Page
constexpr static unsigned long ADDR_MODE_ZPX = 1 << 2;  // Zero Page,X
constexpr static unsigned long ADDR_MODE_ZPY = 1 << 3;  // Zero Page,Y
constexpr static unsigned long ADDR_MODE_REL = 1 << 4;  // Relative
constexpr static unsigned long ADDR_MODE_ABS = 1 << 5;  // Absolute
constexpr static unsigned long ADDR_MODE_ABX = 1 << 6;  // Absolute,X
constexpr static unsigned long ADDR_MODE_ABY = 1 << 7;  // Absolute,y
constexpr static unsigned long ADDR_MODE_IND = 1 << 8;  // Indirect
constexpr static unsigned long ADDR_MODE_IDX = 1 << 9;  // Indexed Ind
constexpr static unsigned long ADDR_MODE_IDY = 1 << 10; // Indirect Idx
constexpr static unsigned long ADDR_MODE_IMP = 1 << 11; // Implied
constexpr static unsigned long ADDR_MODE_ACC = 1 << 12; // Accumulator
// How to manage cycle counts on branches and when instructions fetch data
// across page boundries.
constexpr static unsigned long CYCLE_BRANCH     = 1 << 13;
constexpr static unsigned long CYCLE_CROSS_PAGE = 1 << 14;
	
/////////////////////////////////////////////////////////////////////////

static void ins_adc(CPU *cpu, unsigned long addrmode) {}
static void ins_and(CPU *cpu, unsigned long addrmode) {
	Byte data;
	data = cpu->getData(addrmode);
	cpu->A &= data;
	cpu->SetFlagsForRegister(cpu->A);
}
static void ins_asl(CPU *cpu, unsigned long addrmode) {}
static void ins_bcc(CPU *cpu, unsigned long addrmode) {}
static void ins_bcs(CPU *cpu, unsigned long addrmode) {}
static void ins_beq(CPU *cpu, unsigned long addrmode) {}
static void ins_bit(CPU *cpu, unsigned long addrmode) {}
static void ins_bmi(CPU *cpu, unsigned long addrmode) {}
static void ins_bne(CPU *cpu, unsigned long addrmode) {}
static void ins_bpl(CPU *cpu, unsigned long addrmode) {}
static void ins_brk(CPU *cpu, unsigned long addrmode) {}
static void ins_bvc(CPU *cpu, unsigned long addrmode) {}
static void ins_bvs(CPU *cpu, unsigned long addrmode) {}
static void ins_clc(CPU *cpu, unsigned long addrmode) {}
static void ins_cld(CPU *cpu, unsigned long addrmode) {}
static void ins_cli(CPU *cpu, unsigned long addrmode) {}
static void ins_clv(CPU *cpu, unsigned long addrmode) {}
static void ins_cmp(CPU *cpu, unsigned long addrmode) {}
static void ins_cpx(CPU *cpu, unsigned long addrmode) {}
static void ins_cpy(CPU *cpu, unsigned long addrmode) {}
static void ins_dec(CPU *cpu, unsigned long addrmode) {}
static void ins_dex(CPU *cpu, unsigned long addrmode) {}
static void ins_dey(CPU *cpu, unsigned long addrmode) {}
static void ins_eor(CPU *cpu, unsigned long addrmode) {}
static void ins_inc(CPU *cpu, unsigned long addrmode) {}
static void ins_inx(CPU *cpu, unsigned long addrmode) {}
static void ins_iny(CPU *cpu, unsigned long addrmode) {}
static void ins_jmp(CPU *cpu, unsigned long addrmode) {}
static void ins_jsr(CPU *cpu, unsigned long addrmode) {}
static void ins_lda(CPU *cpu, unsigned long addrmode) {}
static void ins_ldx(CPU *cpu, unsigned long addrmode) {}
static void ins_ldy(CPU *cpu, unsigned long addrmode) {}
static void ins_lsr(CPU *cpu, unsigned long addrmode) {}
static void ins_nop(CPU *cpu, unsigned long addrmode) {}
static void ins_ora(CPU *cpu, unsigned long addrmode) {}
static void ins_pha(CPU *cpu, unsigned long addrmode) {}
static void ins_php(CPU *cpu, unsigned long addrmode) {}
static void ins_plp(CPU *cpu, unsigned long addrmode) {}
static void ins_rol(CPU *cpu, unsigned long addrmode) {}
static void ins_ror(CPU *cpu, unsigned long addrmode) {}
static void ins_rti(CPU *cpu, unsigned long addrmode) {}
static void ins_rts(CPU *cpu, unsigned long addrmode) {}
static void ins_sbc(CPU *cpu, unsigned long addrmode) {}
static void ins_sec(CPU *cpu, unsigned long addrmode) {}
static void ins_sed(CPU *cpu, unsigned long addrmode) {}
static void ins_sei(CPU *cpu, unsigned long addrmode) {}
static void ins_sta(CPU *cpu, unsigned long addrmode) {}
static void ins_stx(CPU *cpu, unsigned long addrmode) {}
static void ins_sty(CPU *cpu, unsigned long addrmode) {}
static void ins_tax(CPU *cpu, unsigned long addrmode) {}
static void ins_tay(CPU *cpu, unsigned long addrmode) {}
static void ins_tsx(CPU *cpu, unsigned long addrmode) {}
static void ins_txa(CPU *cpu, unsigned long addrmode) {}
static void ins_txs(CPU *cpu, unsigned long addrmode) {}
static void ins_tya(CPU *cpu, unsigned long addrmode) {}

/////////////////////////////////////////////////////////////////////////

struct CPU::instruction makeIns(const char *name, unsigned long addrmode,
				Byte cycles, CPU::opfn_t opfn) {
	struct CPU::instruction ins = { name, addrmode, cycles, opfn };
	return ins;
}

void setupInstructionMap() {
	cpu.instructions[CPU::INS_BRK_IMP] = 
		makeIns("BRK", ADDR_MODE_IMP, 7, ins_brk);
	cpu.instructions[CPU::INS_ORA_IDX] = 
		makeIns("ORA", ADDR_MODE_IDX, 6, ins_ora);
	cpu.instructions[CPU::INS_ASL_ACC] = 
		makeIns("ASL", ADDR_MODE_ACC, 2, ins_asl);
	cpu.instructions[CPU::INS_ADC_ZP] = 
		makeIns("ADC", ADDR_MODE_ZP, 3, ins_adc);
	cpu.instructions[CPU::INS_ROR_ZP] = 
		makeIns("ROR", ADDR_MODE_ZP, 5, ins_ror);
	cpu.instructions[CPU::INS_ADC_IMM] = 
		makeIns("ADC", ADDR_MODE_IMM, 2, ins_adc);
	cpu.instructions[CPU::INS_ROR_ACC] = 
		makeIns("ROR", ADDR_MODE_ACC, 2, ins_ror);
	cpu.instructions[CPU::INS_JMP_IND] = 
		makeIns("JMP", ADDR_MODE_IND, 5, ins_jmp);
	cpu.instructions[CPU::INS_ADC_ABS] = 
		makeIns("ADC", ADDR_MODE_ABS, 4, ins_adc);
	cpu.instructions[CPU::INS_ROR_ABS] = 
		makeIns("ROR", ADDR_MODE_ABS, 6, ins_ror);
	cpu.instructions[CPU::INS_BVS_REL] = 
		makeIns("BVS", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bvs);
	cpu.instructions[CPU::INS_ADC_IDY] = 
		makeIns("ADC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_adc);
	cpu.instructions[CPU::INS_ADC_ZPX] = 
		makeIns("ADC", ADDR_MODE_ZPX, 4, ins_adc);
	cpu.instructions[CPU::INS_ROR_ZPX] = 
		makeIns("ROR", ADDR_MODE_ZPX, 6, ins_ror);
	cpu.instructions[CPU::INS_SEI_IMP] = 
		makeIns("SEI", ADDR_MODE_IMP, 2, ins_sei);
	cpu.instructions[CPU::INS_ADC_ABY] = 
		makeIns("ADC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_adc);
	cpu.instructions[CPU::INS_ADC_ABX] = 
		makeIns("ADC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_adc);
	cpu.instructions[CPU::INS_ROR_ABX] = 
		makeIns("ROR", ADDR_MODE_ABX, 7, ins_ror);
	cpu.instructions[CPU::INS_STA_IDX] = 
		makeIns("STA", ADDR_MODE_IDX, 6, ins_sta);
	cpu.instructions[CPU::INS_ORA_ABS] = 
		makeIns("ORA", ADDR_MODE_ABS, 4, ins_ora);
	cpu.instructions[CPU::INS_STY_ZP] = 
		makeIns("STY", ADDR_MODE_ZP, 3, ins_sty);
	cpu.instructions[CPU::INS_STA_ZP] = 
		makeIns("STA", ADDR_MODE_ZP, 3, ins_sta);
	cpu.instructions[CPU::INS_STX_ZP] = 
		makeIns("STX", ADDR_MODE_ZP, 3, ins_stx);
	cpu.instructions[CPU::INS_DEY_IMP] = 
		makeIns("DEY", ADDR_MODE_IMP, 2, ins_dey);
	cpu.instructions[CPU::INS_TXA_IMP] = 
		makeIns("TXA", ADDR_MODE_IMP, 2, ins_txa);
	cpu.instructions[CPU::INS_ASL_ABS] = 
		makeIns("ASL", ADDR_MODE_ABS, 6, ins_asl);
	cpu.instructions[CPU::INS_STY_ABS] = 
		makeIns("STY", ADDR_MODE_ABS, 4, ins_sty);
	cpu.instructions[CPU::INS_STA_ABS] = 
		makeIns("STA", ADDR_MODE_ABS, 4, ins_sta);
	cpu.instructions[CPU::INS_STX_ABS] = 
		makeIns("STX", ADDR_MODE_ABS, 4, ins_stx);
	cpu.instructions[CPU::INS_BCC_REL] = 
		makeIns("BCC", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bcc);
	cpu.instructions[CPU::INS_STA_IDY] = 
		makeIns("STA", ADDR_MODE_IDY, 6, ins_sta);
	cpu.instructions[CPU::INS_STY_ZPX] = 
		makeIns("STY", ADDR_MODE_ZPX, 4, ins_sty);
	cpu.instructions[CPU::INS_STA_ZPX] = 
		makeIns("STA", ADDR_MODE_ZPX, 4, ins_sta);
	cpu.instructions[CPU::INS_STX_ZPY] = 
		makeIns("STX", ADDR_MODE_ZPY, 4, ins_stx);
	cpu.instructions[CPU::INS_TYA_IMP] = 
		makeIns("TYA", ADDR_MODE_IMP, 2, ins_tya);
	cpu.instructions[CPU::INS_STA_ABY] = 
		makeIns("STA", ADDR_MODE_ABY, 5, ins_sta);
	cpu.instructions[CPU::INS_TXS_IMP] = 
		makeIns("TXS", ADDR_MODE_IMP, 2, ins_txs);
	cpu.instructions[CPU::INS_STA_ABX] = 
		makeIns("STA", ADDR_MODE_ABX, 5, ins_sta);
	cpu.instructions[CPU::INS_BPL_REL] = 
		makeIns("BPL", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bpl);
	cpu.instructions[CPU::INS_LDY_IMM] = 
		makeIns("LDY", ADDR_MODE_IMM, 2, ins_ldy);
	cpu.instructions[CPU::INS_LDA_IDX] = 
		makeIns("LDA", ADDR_MODE_IDX, 6, ins_lda);
	cpu.instructions[CPU::INS_LDX_IMM] = 
		makeIns("LDX", ADDR_MODE_IMM, 2, ins_ldx);
	cpu.instructions[CPU::INS_LDY_ZP] = 
		makeIns("LDY", ADDR_MODE_ZP, 3, ins_ldy);
	cpu.instructions[CPU::INS_LDA_ZP] = 
		makeIns("LDA", ADDR_MODE_ZP, 3, ins_lda);
	cpu.instructions[CPU::INS_LDX_ZP] = 
		makeIns("LDX", ADDR_MODE_ZP, 3, ins_ldx);
	cpu.instructions[CPU::INS_TAY_IMP] = 
		makeIns("TAY", ADDR_MODE_IMP, 2, ins_tay);
	cpu.instructions[CPU::INS_LDA_IMM] = 
		makeIns("LDA", ADDR_MODE_IMM, 2, ins_lda);
	cpu.instructions[CPU::INS_ORA_IDY] = 
		makeIns("ORA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_ora);
	cpu.instructions[CPU::INS_TAX_IMP] = 
		makeIns("TAX", ADDR_MODE_IMP, 2, ins_tax);
	cpu.instructions[CPU::INS_LDY_ABS] = 
		makeIns("LDY", ADDR_MODE_ABS, 4, ins_ldy);
	cpu.instructions[CPU::INS_LDA_ABS] = 
		makeIns("LDA", ADDR_MODE_ABS, 4, ins_lda);
	cpu.instructions[CPU::INS_LDX_ABS] = 
		makeIns("LDX", ADDR_MODE_ABS, 4, ins_ldx);
	cpu.instructions[CPU::INS_BCS_REL] = 
		makeIns("BCS", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bcs);
	cpu.instructions[CPU::INS_LDA_IDY] = 
		makeIns("LDA", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_lda);
	cpu.instructions[CPU::INS_LDY_ZPX] = 
		makeIns("LDY", ADDR_MODE_ZPX, 4, ins_ldy);
	cpu.instructions[CPU::INS_LDA_ZPX] = 
		makeIns("LDA", ADDR_MODE_ZPX, 4, ins_lda);
	cpu.instructions[CPU::INS_LDX_ZPY] =
		makeIns("LDX", ADDR_MODE_ZPY, 4, ins_ldx);
	cpu.instructions[CPU::INS_CLV_IMP] = 
		makeIns("CLV", ADDR_MODE_IMP, 2, ins_clv);
	cpu.instructions[CPU::INS_LDA_ABY] = 
		makeIns("LDA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_lda);
	cpu.instructions[CPU::INS_TSX_IMP] = 
		makeIns("TSX", ADDR_MODE_IMP, 2, ins_tsx);
	cpu.instructions[CPU::INS_LDY_ABX] = 
		makeIns("LDY", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4,  ins_ldy);
	cpu.instructions[CPU::INS_LDA_ABX] = 
		makeIns("LDA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_lda);
	cpu.instructions[CPU::INS_LDX_ABY] = 
		makeIns("LDX", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_ldx);
	cpu.instructions[CPU::INS_CPY_IMM] = 
		makeIns("CPY", ADDR_MODE_IMM, 2, ins_cpy);
	cpu.instructions[CPU::INS_CMP_IDX] = 
		makeIns("CMP", ADDR_MODE_IDX, 6, ins_cmp);
	cpu.instructions[CPU::INS_CPY_ZP] = 
		makeIns("CPY", ADDR_MODE_ZP, 3, ins_cpy);
	cpu.instructions[CPU::INS_CMP_ZP] = 
		makeIns("CMP", ADDR_MODE_ZP, 3, ins_cmp);
	cpu.instructions[CPU::INS_DEC_ZP] = 
		makeIns("DEC", ADDR_MODE_ZP, 5, ins_dec);
	cpu.instructions[CPU::INS_INY_IMP] = 
		makeIns("INY", ADDR_MODE_IMP, 2, ins_iny);
	cpu.instructions[CPU::INS_CMP_IMM] = 
		makeIns("CMP", ADDR_MODE_IMM, 2, ins_cmp);
	cpu.instructions[CPU::INS_DEX_IMP] = 
		makeIns("DEX", ADDR_MODE_IMP, 2, ins_dex);
	cpu.instructions[CPU::INS_CPY_ABS] = 
		makeIns("CPY", ADDR_MODE_ABS, 4, ins_cpy);
	cpu.instructions[CPU::INS_CMP_ABS] = 
		makeIns("CMP", ADDR_MODE_ABS, 4, ins_cmp);
	cpu.instructions[CPU::INS_DEC_ABS] = 
		makeIns("DEC", ADDR_MODE_ABS, 6, ins_dec);
	cpu.instructions[CPU::INS_BNE_REL] = 
		makeIns("BNE", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bne);
	cpu.instructions[CPU::INS_CMP_IDY] = 
		makeIns("CMP", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_cmp);
	cpu.instructions[CPU::INS_ORA_ZPX] = 
		makeIns("ORA", ADDR_MODE_ZPX, 4, ins_ora);
	cpu.instructions[CPU::INS_CMP_ZPX] = 
		makeIns("CMP", ADDR_MODE_ZPX, 4, ins_cmp);
	cpu.instructions[CPU::INS_DEC_ZPX] = 
		makeIns("DEC", ADDR_MODE_ZPX, 6, ins_dec);
	cpu.instructions[CPU::INS_CLD_IMP] = 
		makeIns("CLD", ADDR_MODE_IMP, 2, ins_cld);
	cpu.instructions[CPU::INS_CMP_ABY] = 
		makeIns("CMP", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_cmp);
	cpu.instructions[CPU::INS_ASL_ZPX] = 
		makeIns("ASL", ADDR_MODE_ZPX, 6, ins_asl);
	cpu.instructions[CPU::INS_CMP_ABX] = 
		makeIns("CMP", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_cmp);
	cpu.instructions[CPU::INS_DEC_ABX] = 
		makeIns("DEC", ADDR_MODE_ABX, 7, ins_dec);
	cpu.instructions[CPU::INS_CPX_IMM] = 
		makeIns("CPX", ADDR_MODE_IMM, 2, ins_cpx);
	cpu.instructions[CPU::INS_SBC_IDX] = 
		makeIns("SBC", ADDR_MODE_IDX, 6, ins_sbc);
	cpu.instructions[CPU::INS_CPX_ZP] = 
		makeIns("CPX", ADDR_MODE_ZP, 3, ins_cpx);
	cpu.instructions[CPU::INS_SBC_ZP] = 
		makeIns("SBC", ADDR_MODE_ZP, 3, ins_sbc);
	cpu.instructions[CPU::INS_INC_ZP] = 
		makeIns("INC", ADDR_MODE_ZP, 5, ins_inc);
	cpu.instructions[CPU::INS_INX_IMP] = 
		makeIns("INX", ADDR_MODE_IMP, 2, ins_inx);
	cpu.instructions[CPU::INS_SBC_IMM] = 
		makeIns("SBC", ADDR_MODE_IMM, 2, ins_sbc);
	cpu.instructions[CPU::INS_NOP_IMP] = 
		makeIns("NOP", ADDR_MODE_IMP, 2, ins_nop);
	cpu.instructions[CPU::INS_CPX_ABS] = 
		makeIns("CPX", ADDR_MODE_ABS, 4, ins_cpx);
	cpu.instructions[CPU::INS_SBC_ABS] = 
		makeIns("SBC", ADDR_MODE_ABS, 4, ins_sbc);
	cpu.instructions[CPU::INS_INC_ABS] = 
		makeIns("INC", ADDR_MODE_ABS, 6, ins_inc);
	cpu.instructions[CPU::INS_CLC_IMP] = 
		makeIns("CLC", ADDR_MODE_IMP, 2, ins_clc);
	cpu.instructions[CPU::INS_BEQ_REL] = 
		makeIns("BEQ", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_beq);
	cpu.instructions[CPU::INS_SBC_IDY] = 
		makeIns("SBC", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_sbc);
	cpu.instructions[CPU::INS_SBC_ZPX] = 
		makeIns("SBC", ADDR_MODE_ZPX, 4, ins_sbc);
	cpu.instructions[CPU::INS_INC_ZPX] = 
		makeIns("INC", ADDR_MODE_ZPX, 6, ins_inc);
	cpu.instructions[CPU::INS_SED_IMP] = 
		makeIns("SED", ADDR_MODE_IMP, 2, ins_sed);
	cpu.instructions[CPU::INS_SBC_ABY] = 
		makeIns("SBC", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_sbc);
	cpu.instructions[CPU::INS_ORA_ABY] = 
		makeIns("ORA", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_ora);
	cpu.instructions[CPU::INS_SBC_ABX] = 
		makeIns("SBC", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_sbc);
	cpu.instructions[CPU::INS_INC_ABX] = 
		makeIns("INC", ADDR_MODE_ABX, 7, ins_inc);
	cpu.instructions[CPU::INS_ORA_ABX] = 
		makeIns("ORA", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_ora);
	cpu.instructions[CPU::INS_ASL_ABX] = 
		makeIns("ASL", ADDR_MODE_ABX, 7, ins_asl);
	cpu.instructions[CPU::INS_JSR_ABS] = 
		makeIns("JSR", ADDR_MODE_ABS, 6, ins_jsr);
	cpu.instructions[CPU::INS_AND_IDX] = 
		makeIns("AND", ADDR_MODE_IDX, 6, ins_and);
	cpu.instructions[CPU::INS_BIT_ZP] = 
		makeIns("BIT", ADDR_MODE_ZP, 3, ins_bit);
	cpu.instructions[CPU::INS_AND_ZP] = 
		makeIns("AND", ADDR_MODE_ZP, 3, ins_and);
	cpu.instructions[CPU::INS_ROL_ZP] = 
		makeIns("ROL", ADDR_MODE_ZP, 5, ins_rol);
	cpu.instructions[CPU::INS_PLP_IMP] = 
		makeIns("PLP", ADDR_MODE_IMP, 4, ins_plp);
	cpu.instructions[CPU::INS_AND_IMM] = 
		makeIns("AND", ADDR_MODE_IMM, 2, ins_and);
	cpu.instructions[CPU::INS_ROL_ACC] = 
		makeIns("ROL", ADDR_MODE_ACC, 2, ins_rol);
	cpu.instructions[CPU::INS_BIT_ABS] = 
		makeIns("BIT", ADDR_MODE_ABS, 4, ins_bit);
	cpu.instructions[CPU::INS_AND_ABS] = 
		makeIns("AND", ADDR_MODE_ABS, 4, ins_and);
	cpu.instructions[CPU::INS_ROL_ABS] = 
		makeIns("ROL", ADDR_MODE_ABS, 6, ins_rol);
	cpu.instructions[CPU::INS_BMI_REL] = 
		makeIns("BMI", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bmi);
	cpu.instructions[CPU::INS_AND_IDY] = 
		makeIns("AND", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_and);
	cpu.instructions[CPU::INS_ORA_ZP] = 
		makeIns("ORA", ADDR_MODE_ZP, 3, ins_ora);
	cpu.instructions[CPU::INS_AND_ZPX] = 
		makeIns("AND", ADDR_MODE_ZPX, 4, ins_and);
	cpu.instructions[CPU::INS_ROL_ZPX] = 
		makeIns("ROL", ADDR_MODE_ZPX, 6, ins_rol);
	cpu.instructions[CPU::INS_SEC_IMP] = 
		makeIns("SEC", ADDR_MODE_IMP, 2, ins_sec);
	cpu.instructions[CPU::INS_AND_ABY] = 
		makeIns("AND", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_and);
	cpu.instructions[CPU::INS_ASL_ZP] = 
		makeIns("ASL", ADDR_MODE_ZP, 5, ins_asl);
	cpu.instructions[CPU::INS_AND_ABX] = 
		makeIns("AND", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_and);
	cpu.instructions[CPU::INS_ROL_ABX] = 
		makeIns("ROL", ADDR_MODE_ABX, 7, ins_rol);
	cpu.instructions[CPU::INS_RTI_IMP] = 
		makeIns("RTI", ADDR_MODE_IMP, 6, ins_rti);
	cpu.instructions[CPU::INS_EOR_IDX] = 
		makeIns("EOR", ADDR_MODE_IDX, 6, ins_eor);
	cpu.instructions[CPU::INS_EOR_ZP] = 
		makeIns("EOR", ADDR_MODE_ZP, 3, ins_eor);
	cpu.instructions[CPU::INS_LSR_ZP] = 
		makeIns("LSR", ADDR_MODE_ZP, 5, ins_lsr);
	cpu.instructions[CPU::INS_PHA_IMP] = 
		makeIns("PHA", ADDR_MODE_IMP, 3, ins_pha);
	cpu.instructions[CPU::INS_EOR_IMM] = 
		makeIns("EOR", ADDR_MODE_IMM, 2, ins_eor);
	cpu.instructions[CPU::INS_LSR_ACC] = 
		makeIns("LSR", ADDR_MODE_ACC, 2, ins_lsr);
	cpu.instructions[CPU::INS_JMP_ABS] = 
		makeIns("JMP", ADDR_MODE_ABS, 3, ins_jmp);
	cpu.instructions[CPU::INS_EOR_ABS] = 
		makeIns("EOR", ADDR_MODE_ABS, 4, ins_eor);
	cpu.instructions[CPU::INS_LSR_ABS] = 
		makeIns("LSR", ADDR_MODE_ABS, 6, ins_lsr);
	cpu.instructions[CPU::INS_PHP_IMP] = 
		makeIns("PHP", ADDR_MODE_IMP, 3, ins_php);
	cpu.instructions[CPU::INS_BVC_REL] = 
		makeIns("BVC", ADDR_MODE_REL | CYCLE_BRANCH, 2, ins_bvc);
	cpu.instructions[CPU::INS_EOR_IDY] = 
		makeIns("EOR", ADDR_MODE_IDY | CYCLE_CROSS_PAGE, 5, ins_eor);
	cpu.instructions[CPU::INS_EOR_ZPX] = 
		makeIns("EOR", ADDR_MODE_ZPX, 4, ins_eor);
	cpu.instructions[CPU::INS_LSR_ZPX] = 
		makeIns("LSR", ADDR_MODE_ZPX, 6, ins_lsr);
	cpu.instructions[CPU::INS_CLI_IMP] = 
		makeIns("CLI", ADDR_MODE_IMP, 2, ins_cli);
	cpu.instructions[CPU::INS_EOR_ABY] = 
		makeIns("EOR", ADDR_MODE_ABY | CYCLE_CROSS_PAGE, 4, ins_eor);
	cpu.instructions[CPU::INS_ORA_IMM] = 
		makeIns("ORA", ADDR_MODE_IMM, 2, ins_ora);
	cpu.instructions[CPU::INS_EOR_ABX] = 
		makeIns("EOR", ADDR_MODE_ABX | CYCLE_CROSS_PAGE, 4, ins_eor);
	cpu.instructions[CPU::INS_LSR_ABX] = 
		makeIns("LSR", ADDR_MODE_ABX, 7, ins_lsr);
	cpu.instructions[CPU::INS_RTS_IMP] = 
		makeIns("RTS", ADDR_MODE_IMP, 6, ins_rts);
	cpu.instructions[CPU::INS_ADC_IDX] = 
		makeIns("ADC", ADDR_MODE_IDX, 6, ins_adc);

}

void CPU::Reset(Word ResetVector) {
	PC = ResetVector;
	SP = INITIAL_SP;
	A = X = Y = 0;
	C = Z = I = D = B = V = N = 0;
	setupInstructionMap();
}

void CPU::Exception() {
	printf("CPU Exception");
	throw -1;
}

void CPU::SetFlagsForRegister(Byte b) {
	Z = (b == 0);
	N = (b & 0b10000000) >> 7;
}

void CPU::SetFlagsForCompare(Byte b, Byte v) {
	C = (b >= v);
	Z = b == v;
	N = (b & 0b10000000) >> 7;
}

Byte CPU::ReadByte(Word address) {
	Byte data = mem.ReadByte(address);
	Cycles++;

	return data;
}

void CPU::WriteByte(Word address, Byte value) {
	mem.WriteByte(address, value);
	Cycles++;
}

Word  CPU::ReadWord(Word address) {
	Word w = ReadByte(address) | (ReadByte(address + 1) << 8);
	return w;
}

Byte CPU::FetchIns() {
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
	w = Pop() | (Pop() << 8);
	return w;
}

void CPU::Push(Byte value) {
	Word SPAddress = (0x01 << 8) | SP;
	WriteByte(SP, value);
	SP--;
}

Byte CPU::Pop() {
	Word SPAddress;
	SP++;
	SPAddress = (0x01 << 8) | SP;
	
	return ReadByte(SP);
}

Byte CPU::getData(unsigned long mode) {
	Byte data;
	Word address, addrmode, flags;

	addrmode = mode & 0b00111111111111;
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

Cycles_t CPU::ExecuteOneInstruction() {
	struct instruction ins;
	Cycles_t startCycles;
	Byte opcode;

	startCycles = Cycles;
	opcode = FetchIns();

	if (instructions.count(opcode) == 0) {
		printf("Invalid opcode 0x%x\n", opcode);
		Exception();
	}
	
	ins = cpu.instructions[opcode];
	ins.opfn(this, ins.addrmode);

	return (Cycles - startCycles);
}

#if 0
/////////////////////////////////////////////////////////////////////////
// Tests

void and_imm() {
	mem.Init();
	cpu.Reset(INITIAL_PC);
	mem[0xFFFC] = CPU::INS_AND_IMM;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;
	Cycles_t StartC;

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

	mem[0xFFFC] = CPU::INS_AND_ZP;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;
	Cycles_t StartC;

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
	Cycles_t StartC;

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
	Cycles_t StartC;

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
	Cycles_t StartC;

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
	Cycles_t StartC;

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
	Cycles_t StartC;

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
