//
// Opcode to implementation map.
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

#include "./6502.h"

CPU::instruction CPU::makeIns(const char *name, Byte addrmode, Byte bytes,
			      Byte cycles, Byte flags, opfn_t opfn) {
	struct instruction ins = { name, addrmode, flags, bytes, cycles, opfn };
	return ins;
}

// The instructions.Flags field provides information about any special
// handling the current instruction requires.  For the 6502, the flags
// are:
//  - CYCLE_BRANCH    : Add a cycle if conditional branch is taken (all
//                      conditional branches.
//  - CYCLE_CROSS_PAGE: Add two cycles if an instruction causes a read from
//                      an address on an adjacent page, specifically indexed
//                      addressing modes.
//
// See http://www.6502.org/users/obelisk/6502/addressing.html for more
// information.
void CPU::setupInstructionMap() {

	_instructions[INS_BRK_IMP] = 
		makeIns("brk", ADDR_MODE_IMP, 1, 7, NONE,
			&CPU::ins_brk);

	_instructions[INS_ORA_IDX] = 
		makeIns("ora", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_ora);

	_instructions[INS_ASL_ACC] = 
		makeIns("asl", ADDR_MODE_ACC, 1, 2, NONE,
			&CPU::ins_asl);

	_instructions[INS_ADC_ZP] = 
		makeIns("adc", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_adc);

	_instructions[INS_ROR_ZP] = 
		makeIns("ror", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_ror);

	_instructions[INS_PLA_IMP] = 
		makeIns("pla", ADDR_MODE_IMP, 1, 4, NONE,
			&CPU::ins_pla);

	_instructions[INS_ADC_IMM] = 
		makeIns("adc", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_adc);

	_instructions[INS_ROR_ACC] = 
		makeIns("ror", ADDR_MODE_ACC, 1, 2, NONE,
			&CPU::ins_ror);

	_instructions[INS_JMP_IND] = 
		makeIns("jmp", ADDR_MODE_IND, 3, 5, NONE,
			&CPU::ins_jmp);

	_instructions[INS_ADC_ABS] = 
		makeIns("adc", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_adc);

	_instructions[INS_ROR_ABS] = 
		makeIns("ror", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_ror);

	_instructions[INS_BVS_REL] = 
		makeIns("bvs", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bvs);

	_instructions[INS_ADC_IDY] = 
		makeIns("adc", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_adc);

	_instructions[INS_ADC_ZPX] = 
		makeIns("adc", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_adc);

	_instructions[INS_ROR_ZPX] = 
		makeIns("ror", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_ror);

	_instructions[INS_SEI_IMP] = 
		makeIns("sei", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_sei);

	_instructions[INS_ADC_ABY] = 
		makeIns("adc", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_adc);

	_instructions[INS_ADC_ABX] = 
		makeIns("adc", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_adc);

	_instructions[INS_ROR_ABX] = 
		makeIns("ror", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_ror);

	_instructions[INS_STA_IDX] = 
		makeIns("sta", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_sta);

	_instructions[INS_ORA_ABS] = 
		makeIns("ora", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_ora);

	_instructions[INS_STY_ZP] = 
		makeIns("sty", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_sty);

	_instructions[INS_STA_ZP] = 
		makeIns("sta", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_sta);

	_instructions[INS_STX_ZP] = 
		makeIns("stx", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_stx);

	_instructions[INS_DEY_IMP] = 
		makeIns("dey", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_dey);

	_instructions[INS_TXA_IMP] = 
		makeIns("txa", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_txa);

	_instructions[INS_ASL_ABS] = 
		makeIns("asl", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_asl);

	_instructions[INS_STY_ABS] = 
		makeIns("sty", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_sty);

	_instructions[INS_STA_ABS] = 
		makeIns("sta", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_sta);

	_instructions[INS_STX_ABS] = 
		makeIns("stx", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_stx);

	_instructions[INS_BCC_REL] = 
		makeIns("bcc", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bcc);

	_instructions[INS_STA_IDY] = 
		makeIns("sta", ADDR_MODE_IDY, 2, 6, NONE,
			&CPU::ins_sta);

	_instructions[INS_STY_ZPX] = 
		makeIns("sty", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_sty);

	_instructions[INS_STA_ZPX] = 
		makeIns("sta", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_sta);

	_instructions[INS_STX_ZPY] = 
		makeIns("stx", ADDR_MODE_ZPY, 2, 4, NONE,
			&CPU::ins_stx);

	_instructions[INS_TYA_IMP] = 
		makeIns("tya", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_tya);

	_instructions[INS_STA_ABY] = 
		makeIns("sta", ADDR_MODE_ABY, 3, 5, NONE,
			&CPU::ins_sta);

	_instructions[INS_TXS_IMP] = 
		makeIns("txs", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_txs);

	_instructions[INS_STA_ABX] = 
		makeIns("sta", ADDR_MODE_ABX, 3, 5, NONE,
			&CPU::ins_sta);

	_instructions[INS_BPL_REL] = 
		makeIns("bpl", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bpl);

	_instructions[INS_LDY_IMM] = 
		makeIns("ldy", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_ldy);

	_instructions[INS_LDA_IDX] = 
		makeIns("lda", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_lda);

	_instructions[INS_LDX_IMM] = 
		makeIns("ldx", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_ldx);

	_instructions[INS_LDY_ZP] = 
		makeIns("ldy", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_ldy);

	_instructions[INS_LDA_ZP] = 
		makeIns("lda", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_lda);

	_instructions[INS_LDX_ZP] = 
		makeIns("ldx", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_ldx);

	_instructions[INS_TAY_IMP] = 
		makeIns("tay", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_tay);

	_instructions[INS_LDA_IMM] = 
		makeIns("lda", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_lda);

	_instructions[INS_ORA_IDY] = 
		makeIns("ora", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_ora);

	_instructions[INS_TAX_IMP] = 
		makeIns("tax", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_tax);

	_instructions[INS_LDY_ABS] = 
		makeIns("ldy", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_ldy);

	_instructions[INS_LDA_ABS] = 
		makeIns("lda", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_lda);

	_instructions[INS_LDX_ABS] = 
		makeIns("ldx", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_ldx);

	_instructions[INS_BCS_REL] = 
		makeIns("bcs", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bcs);

	_instructions[INS_LDA_IDY] = 
		makeIns("lda", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_lda);

	_instructions[INS_LDY_ZPX] = 
		makeIns("ldy", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_ldy);

	_instructions[INS_LDA_ZPX] = 
		makeIns("lda", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_lda);

	_instructions[INS_LDX_ZPY] = 
		makeIns("ldx", ADDR_MODE_ZPY, 2, 4, NONE,
			&CPU::ins_ldx);

	_instructions[INS_CLV_IMP] = 
		makeIns("clv", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_clv);

	_instructions[INS_LDA_ABY] = 
		makeIns("lda", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_lda);

	_instructions[INS_TSX_IMP] = 
		makeIns("tsx", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_tsx);

	_instructions[INS_LDY_ABX] = 
		makeIns("ldy", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_ldy);

	_instructions[INS_LDA_ABX] = 
		makeIns("lda", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_lda);

	_instructions[INS_LDX_ABY] = 
		makeIns("ldx", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_ldx);

	_instructions[INS_CPY_IMM] = 
		makeIns("cpy", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_cpy);

	_instructions[INS_CMP_IDX] = 
		makeIns("cmp", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_cmp);

	_instructions[INS_CPY_ZP] = 
		makeIns("cpy", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_cpy);

	_instructions[INS_CMP_ZP] = 
		makeIns("cmp", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_cmp);

	_instructions[INS_DEC_ZP] = 
		makeIns("dec", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_dec);

	_instructions[INS_INY_IMP] = 
		makeIns("iny", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_iny);

	_instructions[INS_CMP_IMM] = 
		makeIns("cmp", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_cmp);

	_instructions[INS_DEX_IMP] = 
		makeIns("dex", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_dex);

	_instructions[INS_CPY_ABS] = 
		makeIns("cpy", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_cpy);

	_instructions[INS_CMP_ABS] = 
		makeIns("cmp", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_cmp);

	_instructions[INS_DEC_ABS] = 
		makeIns("dec", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_dec);

	_instructions[INS_BNE_REL] = 
		makeIns("bne", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bne);

	_instructions[INS_CMP_IDY] = 
		makeIns("cmp", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_cmp);

	_instructions[INS_ORA_ZPX] = 
		makeIns("ora", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_ora);

	_instructions[INS_CMP_ZPX] = 
		makeIns("cmp", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_cmp);

	_instructions[INS_DEC_ZPX] = 
		makeIns("dec", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_dec);

	_instructions[INS_CLD_IMP] = 
		makeIns("cld", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_cld);

	_instructions[INS_CMP_ABY] = 
		makeIns("cmp", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_cmp);

	_instructions[INS_ASL_ZPX] = 
		makeIns("asl", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_asl);

	_instructions[INS_CMP_ABX] = 
		makeIns("cmp", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_cmp);

	_instructions[INS_DEC_ABX] = 
		makeIns("dec", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_dec);

	_instructions[INS_CPX_IMM] = 
		makeIns("cpx", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_cpx);

	_instructions[INS_SBC_IDX] = 
		makeIns("sbc", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_sbc);

	_instructions[INS_CPX_ZP] = 
		makeIns("cpx", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_cpx);

	_instructions[INS_SBC_ZP] = 
		makeIns("sbc", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_sbc);

	_instructions[INS_INC_ZP] = 
		makeIns("inc", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_inc);

	_instructions[INS_INX_IMP] = 
		makeIns("inx", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_inx);

	_instructions[INS_SBC_IMM] = 
		makeIns("sbc", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_sbc);

	_instructions[INS_NOP_IMP] = 
		makeIns("nop", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_nop);

	_instructions[INS_CPX_ABS] = 
		makeIns("cpx", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_cpx);

	_instructions[INS_SBC_ABS] = 
		makeIns("sbc", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_sbc);

	_instructions[INS_INC_ABS] = 
		makeIns("inc", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_inc);

	_instructions[INS_CLC_IMP] = 
		makeIns("clc", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_clc);

	_instructions[INS_BEQ_REL] = 
		makeIns("beq", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_beq);

	_instructions[INS_SBC_IDY] = 
		makeIns("sbc", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_sbc);

	_instructions[INS_SBC_ZPX] = 
		makeIns("sbc", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_sbc);

	_instructions[INS_INC_ZPX] = 
		makeIns("inc", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_inc);

	_instructions[INS_SED_IMP] = 
		makeIns("sed", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_sed);

	_instructions[INS_SBC_ABY] = 
		makeIns("sbc", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_sbc);

	_instructions[INS_ORA_ABY] = 
		makeIns("ora", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_ora);

	_instructions[INS_SBC_ABX] = 
		makeIns("sbc", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_sbc);

	_instructions[INS_INC_ABX] = 
		makeIns("inc", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_inc);

	_instructions[INS_ORA_ABX] = 
		makeIns("ora", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_ora);

	_instructions[INS_ASL_ABX] = 
		makeIns("asl", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_asl);

	_instructions[INS_JSR_ABS] = 
		makeIns("jsr", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_jsr);

	_instructions[INS_AND_IDX] = 
		makeIns("and", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_and);

	_instructions[INS_BIT_ZP] = 
		makeIns("bit", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_bit);

	_instructions[INS_AND_ZP] = 
		makeIns("and", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_and);

	_instructions[INS_ROL_ZP] = 
		makeIns("rol", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_rol);

	_instructions[INS_PLP_IMP] = 
		makeIns("plp", ADDR_MODE_IMP, 1, 4, NONE,
			&CPU::ins_plp);

	_instructions[INS_AND_IMM] = 
		makeIns("and", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_and);

	_instructions[INS_ROL_ACC] = 
		makeIns("rol", ADDR_MODE_ACC, 1, 2, NONE,
			&CPU::ins_rol);

	_instructions[INS_BIT_ABS] = 
		makeIns("bit", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_bit);

	_instructions[INS_AND_ABS] = 
		makeIns("and", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_and);

	_instructions[INS_ROL_ABS] = 
		makeIns("rol", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_rol);

	_instructions[INS_BMI_REL] = 
		makeIns("bmi", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bmi);

	_instructions[INS_AND_IDY] = 
		makeIns("and", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_and);

	_instructions[INS_ORA_ZP] = 
		makeIns("ora", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_ora);

	_instructions[INS_AND_ZPX] = 
		makeIns("and", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_and);

	_instructions[INS_ROL_ZPX] = 
		makeIns("rol", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_rol);

	_instructions[INS_SEC_IMP] = 
		makeIns("sec", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_sec);

	_instructions[INS_AND_ABY] = 
		makeIns("and", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_and);

	_instructions[INS_ASL_ZP] = 
		makeIns("asl", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_asl);

	_instructions[INS_AND_ABX] = 
		makeIns("and", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_and);

	_instructions[INS_ROL_ABX] = 
		makeIns("rol", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_rol);

	_instructions[INS_RTI_IMP] = 
		makeIns("rti", ADDR_MODE_IMP, 1, 6, NONE,
			&CPU::ins_rti);

	_instructions[INS_EOR_IDX] = 
		makeIns("eor", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_eor);

	_instructions[INS_EOR_ZP] = 
		makeIns("eor", ADDR_MODE_ZP, 2, 3, NONE,
			&CPU::ins_eor);

	_instructions[INS_LSR_ZP] = 
		makeIns("lsr", ADDR_MODE_ZP, 2, 5, NONE,
			&CPU::ins_lsr);

	_instructions[INS_PHA_IMP] = 
		makeIns("pha", ADDR_MODE_IMP, 1, 3, NONE,
			&CPU::ins_pha);

	_instructions[INS_EOR_IMM] = 
		makeIns("eor", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_eor);

	_instructions[INS_LSR_ACC] = 
		makeIns("lsr", ADDR_MODE_ACC, 1, 2, NONE,
			&CPU::ins_lsr);

	_instructions[INS_JMP_ABS] = 
		makeIns("jmp", ADDR_MODE_ABS, 3, 3, NONE,
			&CPU::ins_jmp);

	_instructions[INS_EOR_ABS] = 
		makeIns("eor", ADDR_MODE_ABS, 3, 4, NONE,
			&CPU::ins_eor);

	_instructions[INS_LSR_ABS] = 
		makeIns("lsr", ADDR_MODE_ABS, 3, 6, NONE,
			&CPU::ins_lsr);

	_instructions[INS_PHP_IMP] = 
		makeIns("php", ADDR_MODE_IMP, 1, 3, NONE,
			&CPU::ins_php);

	_instructions[INS_BVC_REL] = 
		makeIns("bvc", ADDR_MODE_REL, 2, 2, CYCLE_BRANCH,
			&CPU::ins_bvc);

	_instructions[INS_EOR_IDY] = 
		makeIns("eor", ADDR_MODE_IDY, 2, 5, CYCLE_CROSS_PAGE,
			&CPU::ins_eor);

	_instructions[INS_EOR_ZPX] = 
		makeIns("eor", ADDR_MODE_ZPX, 2, 4, NONE,
			&CPU::ins_eor);

	_instructions[INS_LSR_ZPX] = 
		makeIns("lsr", ADDR_MODE_ZPX, 2, 6, NONE,
			&CPU::ins_lsr);

	_instructions[INS_CLI_IMP] = 
		makeIns("cli", ADDR_MODE_IMP, 1, 2, NONE,
			&CPU::ins_cli);

	_instructions[INS_EOR_ABY] = 
		makeIns("eor", ADDR_MODE_ABY, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_eor);

	_instructions[INS_ORA_IMM] = 
		makeIns("ora", ADDR_MODE_IMM, 2, 2, NONE,
			&CPU::ins_ora);

	_instructions[INS_EOR_ABX] = 
		makeIns("eor", ADDR_MODE_ABX, 3, 4, CYCLE_CROSS_PAGE,
			&CPU::ins_eor);

	_instructions[INS_LSR_ABX] = 
		makeIns("lsr", ADDR_MODE_ABX, 3, 7, NONE,
			&CPU::ins_lsr);
	
	_instructions[INS_RTS_IMP] = 
		makeIns("rts", ADDR_MODE_IMP, 1, 6, NONE,
			&CPU::ins_rts);

	_instructions[INS_ADC_IDX] = 
		makeIns("adc", ADDR_MODE_IDX, 2, 6, NONE,
			&CPU::ins_adc);

}
