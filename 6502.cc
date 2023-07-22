#include <stdio.h>
#include <map>
#include <bitset>
#include <functional>

using namespace std;

using Byte = unsigned char;
using Word = unsigned short;

const unsigned int MAX_MEM = 64 * 1024;

unsigned long Cycles = 0;

struct Memory {
	Byte m[MAX_MEM];

	void exception() {
		printf("Memory exception!\n");
		throw -1;
	}

	Byte& operator[](Word address) {
		if (address > MAX_MEM) {
			printf("Memory out of range (indexed 0x%x\n", address);
			exception();
		}
		return m[address];
	}

	void Init() {
		unsigned long i;
		for (i = 0; i < MAX_MEM; i++)
			m[i] = 0;
	}

	Byte ReadByte(Word address) {
		if (address > MAX_MEM)
			exception();
		return m[address];
	}

	void WriteByte(Word address, Byte byte) {
		if (address > MAX_MEM)
			exception();
		m[address] = byte;
	}

	Word ReadWord(Word address) {
		return ReadByte(address) | (ReadByte(address + 1) << 8);
	}

	void WriteWord(Word address, Word word) {
		WriteByte(address, word & 0xff);
		WriteByte(address + 1, word >> 8);
	}
};

Memory mem;


class CPU {
public:
	constexpr static Byte INS_BRK_IMP = 0x00;
	constexpr static Byte INS_ORA_IDX = 0x01;
	constexpr static Byte INS_ASL_ACC = 0x0a;
	constexpr static Byte INS_ADC_ZP  = 0x65;
	constexpr static Byte INS_ROR_ZP  = 0x66;
	constexpr static Byte INS_ADC_IMM = 0x69;
	constexpr static Byte INS_ROR_ACC = 0x6a;
	constexpr static Byte INS_JMP_IND = 0x6c;
	constexpr static Byte INS_ADC_ABS = 0x6d;
	constexpr static Byte INS_ROR_ABS = 0x6e;
	constexpr static Byte INS_BVS_REL = 0x70;
	constexpr static Byte INS_ADC_IDY = 0x71;
	constexpr static Byte INS_ADC_ZPX = 0x75;
	constexpr static Byte INS_ROR_ZPX = 0x76;
	constexpr static Byte INS_SEI_IMP = 0x78;
	constexpr static Byte INS_ADC_ABY = 0x79;
	constexpr static Byte INS_ADC_ABX = 0x7d;
	constexpr static Byte INS_ROR_ABX = 0x7e;
	constexpr static Byte INS_STA_IDX = 0x81;
	constexpr static Byte INS_ORA_ABS = 0x0d;
	constexpr static Byte INS_STY_ZP  = 0x84;
	constexpr static Byte INS_STA_ZP  = 0x85;
	constexpr static Byte INS_STX_ZP  = 0x86;
	constexpr static Byte INS_DEY_IMP = 0x88;
	constexpr static Byte INS_TXA_IMP = 0x8a;
	constexpr static Byte INS_ASL_ABS = 0x0e;
	constexpr static Byte INS_STY_ABS = 0x8c;
	constexpr static Byte INS_STA_ABS = 0x8d;
	constexpr static Byte INS_STX_ABS = 0x8e;
	constexpr static Byte INS_BCC_REL = 0x90;
	constexpr static Byte INS_STA_IDY = 0x91;
	constexpr static Byte INS_STY_ZPX = 0x94;
	constexpr static Byte INS_STA_ZPX = 0x95;
	constexpr static Byte INS_STX_ZPY = 0x96;
	constexpr static Byte INS_TYA_IMP = 0x98;
	constexpr static Byte INS_STA_ABY = 0x99;
	constexpr static Byte INS_TXS_IMP = 0x9a;
	constexpr static Byte INS_STA_ABX = 0x9d;
	constexpr static Byte INS_BPL_REL = 0x10;
	constexpr static Byte INS_LDY_IMM = 0xa0;
	constexpr static Byte INS_LDA_IDX = 0xa1;
	constexpr static Byte INS_LDX_IMM = 0xa2;
	constexpr static Byte INS_LDY_ZP  = 0xa4;
	constexpr static Byte INS_LDA_ZP  = 0xa5;
	constexpr static Byte INS_LDX_ZP  = 0xa6;
	constexpr static Byte INS_TAY_IMP = 0xa8;
	constexpr static Byte INS_LDA_IMM = 0xa9;
	constexpr static Byte INS_ORA_IDY = 0x11;
	constexpr static Byte INS_TAX_IMP = 0xaa;
	constexpr static Byte INS_LDY_ABS = 0xac;
	constexpr static Byte INS_LDA_ABS = 0xad;
	constexpr static Byte INS_LDX_ABS = 0xae;
	constexpr static Byte INS_BCS_REL = 0xb0;
	constexpr static Byte INS_LDA_IDY = 0xb1;
	constexpr static Byte INS_LDY_ZPX = 0xb4;
	constexpr static Byte INS_LDA_ZPX = 0xb5;
	constexpr static Byte INS_LDX_ZPY = 0xb6;
	constexpr static Byte INS_CLV_IMP = 0xb8;
	constexpr static Byte INS_LDA_ABY = 0xb9;
	constexpr static Byte INS_TSX_IMP = 0xba;
	constexpr static Byte INS_LDY_ABX = 0xbc;
	constexpr static Byte INS_LDA_ABX = 0xbd;
	constexpr static Byte INS_LDX_ABY = 0xbe;
	constexpr static Byte INS_CPY_IMM = 0xc0;
	constexpr static Byte INS_CMP_IDX = 0xc1;
	constexpr static Byte INS_CPY_ZP  = 0xc4;
	constexpr static Byte INS_CMP_ZP  = 0xc5;
	constexpr static Byte INS_DEC_ZP  = 0xc6;
	constexpr static Byte INS_INY_IMP = 0xc8;
	constexpr static Byte INS_CMP_IMM = 0xc9;
	constexpr static Byte INS_DEX_IMP = 0xca;
	constexpr static Byte INS_CPY_ABS = 0xcc;
	constexpr static Byte INS_CMP_ABS = 0xcd;
	constexpr static Byte INS_DEC_ABS = 0xce;
	constexpr static Byte INS_BNE_REL = 0xd0;
	constexpr static Byte INS_CMP_IDY = 0xd1;
	constexpr static Byte INS_ORA_ZPX = 0x15;
	constexpr static Byte INS_CMP_ZPX = 0xd5;
	constexpr static Byte INS_DEC_ZPX = 0xd6;
	constexpr static Byte INS_CLD_IMP = 0xd8;
	constexpr static Byte INS_CMP_ABY = 0xd9;
	constexpr static Byte INS_ASL_ZPX = 0x16;
	constexpr static Byte INS_CMP_ABX = 0xdd;
	constexpr static Byte INS_DEC_ABX = 0xde;
	constexpr static Byte INS_CPX_IMM = 0xe0;
	constexpr static Byte INS_SBC_IDX = 0xe1;
	constexpr static Byte INS_CPX_ZP  = 0xe4;
	constexpr static Byte INS_SBC_ZP  = 0xe5;
	constexpr static Byte INS_INC_ZP  = 0xe6;
	constexpr static Byte INS_INX_IMP = 0xe8;
	constexpr static Byte INS_SBC_IMM = 0xe9;
	constexpr static Byte INS_NOP_IMP = 0xea;
	constexpr static Byte INS_CPX_ABS = 0xec;
	constexpr static Byte INS_SBC_ABS = 0xed;
	constexpr static Byte INS_INC_ABS = 0xee;
	constexpr static Byte INS_CLC_IMP = 0x18;
	constexpr static Byte INS_BEQ_REL = 0xf0;
	constexpr static Byte INS_SBC_IDY = 0xf1;
	constexpr static Byte INS_SBC_ZPX = 0xf5;
	constexpr static Byte INS_INC_ZPX = 0xf6;
	constexpr static Byte INS_SED_IMP = 0xf8;
	constexpr static Byte INS_SBC_ABY = 0xf9;
	constexpr static Byte INS_ORA_ABY = 0x19;
	constexpr static Byte INS_SBC_ABX = 0xfd;
	constexpr static Byte INS_INC_ABX = 0xfe;
	constexpr static Byte INS_ORA_ABX = 0x1d;
	constexpr static Byte INS_ASL_ABX = 0x1e;
	constexpr static Byte INS_JSR_ABS = 0x20;
	constexpr static Byte INS_AND_IDX = 0x21;
	constexpr static Byte INS_BIT_ZP  = 0x24;
	constexpr static Byte INS_AND_ZP  = 0x25;
	constexpr static Byte INS_ROL_ZP  = 0x26;
	constexpr static Byte INS_PLP_IMP = 0x28;
	constexpr static Byte INS_AND_IMM = 0x29;
	constexpr static Byte INS_ROL_ACC = 0x2a;
	constexpr static Byte INS_BIT_ABS = 0x2c;
	constexpr static Byte INS_AND_ABS = 0x2d;
	constexpr static Byte INS_ROL_ABS = 0x2e;
	constexpr static Byte INS_BMI_REL = 0x30;
	constexpr static Byte INS_AND_IDY = 0x31;
	constexpr static Byte INS_ORA_ZP  = 0x05;
	constexpr static Byte INS_AND_ZPX = 0x35;
	constexpr static Byte INS_ROL_ZPX = 0x36;
	constexpr static Byte INS_SEC_IMP = 0x38;
	constexpr static Byte INS_AND_ABY = 0x39;
	constexpr static Byte INS_ASL_ZP  = 0x06;
	constexpr static Byte INS_AND_ABX = 0x3d;
	constexpr static Byte INS_ROL_ABX = 0x3e;
	constexpr static Byte INS_RTI_IMP = 0x40;
	constexpr static Byte INS_EOR_IDX = 0x41;
	constexpr static Byte INS_EOR_ZP  = 0x45;
	constexpr static Byte INS_LSR_ZP  = 0x46;
	constexpr static Byte INS_PHA_IMP = 0x48;
	constexpr static Byte INS_EOR_IMM = 0x49;
	constexpr static Byte INS_LSR_ACC = 0x4a;
	constexpr static Byte INS_JMP_ABS = 0x4c;
	constexpr static Byte INS_EOR_ABS = 0x4d;
	constexpr static Byte INS_LSR_ABS = 0x4e;
	constexpr static Byte INS_PHP_IMP = 0x08;
	constexpr static Byte INS_BVC_REL = 0x50;
	constexpr static Byte INS_EOR_IDY = 0x51;
	constexpr static Byte INS_EOR_ZPX = 0x55;
	constexpr static Byte INS_LSR_ZPX = 0x56;
	constexpr static Byte INS_CLI_IMP = 0x58;
	constexpr static Byte INS_EOR_ABY = 0x59;
	constexpr static Byte INS_ORA_IMM = 0x09;
	constexpr static Byte INS_EOR_ABX = 0x5d;
	constexpr static Byte INS_LSR_ABX = 0x5e;
	constexpr static Byte INS_RTS_IMP = 0x60;
	constexpr static Byte INS_ADC_IDX = 0x61;

	
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

	typedef void (*opfn_t)(CPU *, unsigned long);
	struct instruction {
		const char *name;
		unsigned long addrmode;
		opfn_t opfn;
	};
	std::map<Byte, instruction> instructions;

	struct instruction makeIns(const char *name,
				   unsigned long addrmode, 
				   opfn_t opfn) {
		
		struct instruction n_ins = { name, addrmode, opfn };
		return n_ins;
	}
	
	
/////////////////////////////////////////////////////////////////////////
	
	static void ins_adc(CPU *cpu, unsigned long addrmode) {}
	static void ins_and(CPU *cpu, unsigned long addrmode) {
		Byte data = cpu->getData(addrmode);
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
/////////////////////////////////////////////////////////////////////////
		
	void setupInstructionMap() {
		instructions[INS_AND_IMM] = 
			makeIns("AND", ADDR_MODE_IMM, ins_and);
		instructions[INS_BRK_IMP] = 
			makeIns("BRK", ADDR_MODE_IMP, ins_brk);
		instructions[INS_ORA_IDX] = 
			makeIns("ORA", ADDR_MODE_IDX, ins_ora);
		instructions[INS_ASL_ACC] = 
			makeIns("ASL", ADDR_MODE_ACC, ins_asl);
		instructions[INS_ADC_ZP] = 
			makeIns("ADC", ADDR_MODE_ZP, ins_adc);
		instructions[INS_ROR_ZP] = 
			makeIns("ROR", ADDR_MODE_ZP, ins_ror);
		instructions[INS_ADC_IMM] = 
			makeIns("ADC", ADDR_MODE_IMM, ins_adc);
		instructions[INS_ROR_ACC] = 
			makeIns("ROR", ADDR_MODE_ACC, ins_ror);
		instructions[INS_JMP_IND] = 
			makeIns("JMP", ADDR_MODE_IND, ins_jmp);
		instructions[INS_ADC_ABS] = 
			makeIns("ADC", ADDR_MODE_ABS, ins_adc);
		instructions[INS_ROR_ABS] = 
			makeIns("ROR", ADDR_MODE_ABS, ins_ror);
		instructions[INS_BVS_REL] = 
			makeIns("BVS", ADDR_MODE_REL, ins_bvs);
		instructions[INS_ADC_IDY] = 
			makeIns("ADC", ADDR_MODE_IDY, ins_adc);
		instructions[INS_ADC_ZPX] = 
			makeIns("ADC", ADDR_MODE_ZPX, ins_adc);
		instructions[INS_ROR_ZPX] = 
			makeIns("ROR", ADDR_MODE_ZPX, ins_ror);
		instructions[INS_SEI_IMP] = 
			makeIns("SEI", ADDR_MODE_IMP, ins_sei);
		instructions[INS_ADC_ABY] = 
			makeIns("ADC", ADDR_MODE_ABY, ins_adc);
		instructions[INS_ADC_ABX] = 
			makeIns("ADC", ADDR_MODE_ABX, ins_adc);
		instructions[INS_ROR_ABX] = 
			makeIns("ROR", ADDR_MODE_ABX, ins_ror);
		instructions[INS_STA_IDX] = 
			makeIns("STA", ADDR_MODE_IDX, ins_sta);
		instructions[INS_ORA_ABS] = 
			makeIns("ORA", ADDR_MODE_ABS, ins_ora);
		instructions[INS_STY_ZP] = 
			makeIns("STY", ADDR_MODE_ZP, ins_sty);
		instructions[INS_STA_ZP] = 
			makeIns("STA", ADDR_MODE_ZP, ins_sta);
		instructions[INS_STX_ZP] = 
			makeIns("STX", ADDR_MODE_ZP, ins_stx);
		instructions[INS_DEY_IMP] = 
			makeIns("DEY", ADDR_MODE_IMP, ins_dey);
		instructions[INS_TXA_IMP] = 
			makeIns("TXA", ADDR_MODE_IMP, ins_txa);
		instructions[INS_ASL_ABS] = 
			makeIns("ASL", ADDR_MODE_ABS, ins_asl);
		instructions[INS_STY_ABS] = 
			makeIns("STY", ADDR_MODE_ABS, ins_sty);
		instructions[INS_STA_ABS] = 
			makeIns("STA", ADDR_MODE_ABS, ins_sta);
		instructions[INS_STX_ABS] = 
			makeIns("STX", ADDR_MODE_ABS, ins_stx);
		instructions[INS_BCC_REL] = 
			makeIns("BCC", ADDR_MODE_REL, ins_bcc);
		instructions[INS_STA_IDY] = 
			makeIns("STA", ADDR_MODE_IDY, ins_sta);
		instructions[INS_STY_ZPX] = 
			makeIns("STY", ADDR_MODE_ZPX, ins_sty);
		instructions[INS_STA_ZPX] = 
			makeIns("STA", ADDR_MODE_ZPX, ins_sta);
		instructions[INS_STX_ZPY] = 
			makeIns("STX", ADDR_MODE_ZPY, ins_stx);
		instructions[INS_TAY_IMP] = 
			makeIns("TAY", ADDR_MODE_IMP, ins_tay);
		instructions[INS_STA_ABY] = 
			makeIns("STA", ADDR_MODE_ABY, ins_sta);
		instructions[INS_TXS_IMP] = 
			makeIns("TXS", ADDR_MODE_IMP, ins_txs);
		instructions[INS_STA_ABX] = 
			makeIns("STA", ADDR_MODE_ABX, ins_sta);
		instructions[INS_BPL_REL] = 
			makeIns("BPL", ADDR_MODE_REL, ins_bpl);
		instructions[INS_LDY_IMM] = 
			makeIns("LDY", ADDR_MODE_IMM, ins_ldy);
		instructions[INS_LDA_IDX] = 
			makeIns("LDA", ADDR_MODE_IDX, ins_lda);
		instructions[INS_LDX_IMM] = 
			makeIns("LDX", ADDR_MODE_IMM, ins_ldx);
		instructions[INS_LDY_ZP] = 
			makeIns("LDY", ADDR_MODE_ZP, ins_ldy);
		instructions[INS_LDA_ZP] = 
			makeIns("LDA", ADDR_MODE_ZP, ins_lda);
		instructions[INS_LDX_ZP] = 
			makeIns("LDX", ADDR_MODE_ZP, ins_ldx);
		instructions[INS_TAY_IMP] = 
			makeIns("TAY", ADDR_MODE_IMP, ins_tay);
		instructions[INS_LDA_IMM] = 
			makeIns("LDA", ADDR_MODE_IMM, ins_lda);
		instructions[INS_ORA_IDY] = 
			makeIns("ORA", ADDR_MODE_IDY, ins_ora);
		instructions[INS_TAX_IMP] = 
			makeIns("TAX", ADDR_MODE_IMP, ins_tax);
		instructions[INS_LDY_ABS] = 
			makeIns("LDY", ADDR_MODE_ABS, ins_ldy);
		instructions[INS_LDA_ABS] = 
			makeIns("LDA", ADDR_MODE_ABS, ins_lda);
		instructions[INS_LDX_ABS] = 
			makeIns("LDX", ADDR_MODE_ABS, ins_ldx);
		instructions[INS_BCS_REL] = 
			makeIns("BCS", ADDR_MODE_REL, ins_bcs);
		instructions[INS_LDA_IDY] = 
			makeIns("LDA", ADDR_MODE_IDY, ins_lda);
		instructions[INS_LDY_ZPX] = 
			makeIns("LDY", ADDR_MODE_ZPX, ins_ldy);
		instructions[INS_LDA_ZPX] = 
			makeIns("LDA", ADDR_MODE_ZPX, ins_lda);
		instructions[INS_LDX_ZPY] = 
			makeIns("LDX", ADDR_MODE_ZPY, ins_ldx);
		instructions[INS_CLV_IMP] = 
			makeIns("CLV", ADDR_MODE_IMP, ins_clv);
		instructions[INS_LDA_ABY] = 
			makeIns("LDA", ADDR_MODE_ABY, ins_lda);
		instructions[INS_TSX_IMP] = 
			makeIns("TSX", ADDR_MODE_IMP, ins_tsx);
		instructions[INS_LDY_ABX] = 
			makeIns("LDY", ADDR_MODE_ABX, ins_ldy);
		instructions[INS_LDA_ABX] = 
			makeIns("LDA", ADDR_MODE_ABX, ins_lda);
		instructions[INS_LDX_ABY] = 
			makeIns("LDX", ADDR_MODE_ABY, ins_ldx);
		instructions[INS_CPY_IMM] = 
			makeIns("CPY", ADDR_MODE_IMM, ins_cpy);
		instructions[INS_CMP_IDX] = 
			makeIns("CMP", ADDR_MODE_IDX, ins_cmp);
		instructions[INS_CPY_ZP] = 
			makeIns("CPY", ADDR_MODE_ZP, ins_cpy);
		instructions[INS_CMP_ZP] = 
			makeIns("CMP", ADDR_MODE_ZP, ins_cmp);
		instructions[INS_DEC_ZP] = 
			makeIns("DEC", ADDR_MODE_ZP, ins_dec);
		instructions[INS_INY_IMP] = 
			makeIns("INY", ADDR_MODE_IMP, ins_iny);
		instructions[INS_CMP_IMM] = 
			makeIns("CMP", ADDR_MODE_IMM, ins_cmp);
		instructions[INS_DEX_IMP] = 
			makeIns("DEX", ADDR_MODE_IMP, ins_dex);
		instructions[INS_CPY_ABS] = 
			makeIns("CPY", ADDR_MODE_ABS, ins_cpy);
		instructions[INS_CMP_ABS] = 
			makeIns("CMP", ADDR_MODE_ABS, ins_cmp);
		instructions[INS_DEC_ABS] = 
			makeIns("DEC", ADDR_MODE_ABS, ins_dec);
		instructions[INS_BNE_REL] = 
			makeIns("BNE", ADDR_MODE_REL, ins_bne);
		instructions[INS_CMP_IDY] = 
			makeIns("CMP", ADDR_MODE_IDY, ins_cmp);
		instructions[INS_ORA_ZPX] = 
			makeIns("ORA", ADDR_MODE_ZPX, ins_ora);
		instructions[INS_CMP_ZPX] = 
			makeIns("CMP", ADDR_MODE_ZPX, ins_cmp);
		instructions[INS_DEC_ZPX] = 
			makeIns("DEC", ADDR_MODE_ZPX, ins_dec);
		instructions[INS_CLD_IMP] = 
			makeIns("CLD", ADDR_MODE_IMP, ins_cld);
		instructions[INS_CMP_ABY] = 
			makeIns("CMP", ADDR_MODE_ABY, ins_cmp);
		instructions[INS_ASL_ZPX] = 
			makeIns("ASL", ADDR_MODE_ZPX, ins_asl);
		instructions[INS_CMP_ABX] = 
			makeIns("CMP", ADDR_MODE_ABX, ins_cmp);
		instructions[INS_DEC_ABX] = 
			makeIns("DEC", ADDR_MODE_ABX, ins_dec);
		instructions[INS_CPX_IMM] = 
			makeIns("CPX", ADDR_MODE_IMM, ins_cpx);
		instructions[INS_SBC_IDX] = 
			makeIns("SBC", ADDR_MODE_IDX, ins_sbc);
		instructions[INS_CPX_ZP] = 
			makeIns("CPX", ADDR_MODE_ZP, ins_cpx);
		instructions[INS_SBC_ZP] = 
			makeIns("SBC", ADDR_MODE_ZP, ins_sbc);
		instructions[INS_INC_ZP] = 
			makeIns("INC", ADDR_MODE_ZP, ins_inc);
		instructions[INS_INX_IMP] = 
			makeIns("INX", ADDR_MODE_IMP, ins_inx);
		instructions[INS_SBC_IMM] = 
			makeIns("SBC", ADDR_MODE_IMM, ins_sbc);
		instructions[INS_NOP_IMP] = 
			makeIns("NOP", ADDR_MODE_IMP, ins_nop);
		instructions[INS_CPX_ABS] = 
			makeIns("CPX", ADDR_MODE_ABS, ins_cpx);
		instructions[INS_SBC_ABS] = 
			makeIns("SBC", ADDR_MODE_ABS, ins_sbc);
		instructions[INS_INC_ABS] = 
			makeIns("INC", ADDR_MODE_ABS, ins_inc);
		instructions[INS_CLC_IMP] = 
			makeIns("CLC", ADDR_MODE_IMP, ins_clc);
		instructions[INS_BEQ_REL] = 
			makeIns("BEQ", ADDR_MODE_REL, ins_beq);
		instructions[INS_SBC_IDY] = 
			makeIns("SBC", ADDR_MODE_IDY, ins_sbc);
		instructions[INS_SBC_ZPX] = 
			makeIns("SBC", ADDR_MODE_ZPX, ins_sbc);
		instructions[INS_INC_ZPX] = 
			makeIns("INC", ADDR_MODE_ZPX, ins_inc);
		instructions[INS_SED_IMP] = 
			makeIns("SED", ADDR_MODE_IMP, ins_sed);
		instructions[INS_SBC_ABY] = 
			makeIns("SBC", ADDR_MODE_ABY, ins_sbc);
		instructions[INS_ORA_ABY] = 
			makeIns("ORA", ADDR_MODE_ABY, ins_ora);
		instructions[INS_SBC_ABX] = 
			makeIns("SBC", ADDR_MODE_ABX, ins_sbc);
		instructions[INS_INC_ABX] = 
			makeIns("INC", ADDR_MODE_ABX, ins_inc);
		instructions[INS_ORA_ABX] = 
			makeIns("ORA", ADDR_MODE_ABX, ins_ora);
		instructions[INS_ASL_ABX] = 
			makeIns("ASL", ADDR_MODE_ABX, ins_asl);
		instructions[INS_JSR_ABS] = 
			makeIns("JSR", ADDR_MODE_ABS, ins_jsr);
		instructions[INS_AND_IDX] = 
			makeIns("AND", ADDR_MODE_IDX, ins_and);
		instructions[INS_BIT_ZP] = 
			makeIns("BIT", ADDR_MODE_ZP, ins_bit);
		instructions[INS_AND_ZP] = 
			makeIns("AND", ADDR_MODE_ZP, ins_and);
		instructions[INS_ROL_ZP] = 
			makeIns("ROL", ADDR_MODE_ZP, ins_rol);
		instructions[INS_PLP_IMP] = 
			makeIns("PLP", ADDR_MODE_IMP, ins_plp);
		instructions[INS_AND_IMM] = 
			makeIns("AND", ADDR_MODE_IMM, ins_and);
		instructions[INS_ROL_ACC] = 
			makeIns("ROL", ADDR_MODE_ACC, ins_rol);
		instructions[INS_BIT_ABS] = 
			makeIns("BIT", ADDR_MODE_ABS, ins_bit);
		instructions[INS_AND_ABS] = 
			makeIns("AND", ADDR_MODE_ABS, ins_and);
		instructions[INS_ROL_ABS] = 
			makeIns("ROL", ADDR_MODE_ABS, ins_rol);
		instructions[INS_BMI_REL] = 
			makeIns("BMI", ADDR_MODE_REL, ins_bmi);
		instructions[INS_AND_IDY] = 
			makeIns("AND", ADDR_MODE_IDY, ins_and);
		instructions[INS_ORA_ZP] = 
			makeIns("ORA", ADDR_MODE_ZP, ins_ora);
		instructions[INS_AND_ZPX] = 
			makeIns("AND", ADDR_MODE_ZPX, ins_and);
		instructions[INS_ROL_ZPX] = 
			makeIns("ROL", ADDR_MODE_ZPX, ins_rol);
		instructions[INS_SEC_IMP] = 
			makeIns("SEC", ADDR_MODE_IMP, ins_sec);
		instructions[INS_AND_ABY] = 
			makeIns("AND", ADDR_MODE_ABY, ins_and);
		instructions[INS_ASL_ZP] = 
			makeIns("ASL", ADDR_MODE_ZP, ins_asl);
		instructions[INS_AND_ABX] = 
			makeIns("AND", ADDR_MODE_ABX, ins_and);
		instructions[INS_ROL_ABX] = 
			makeIns("ROL", ADDR_MODE_ABX, ins_rol);
		instructions[INS_RTI_IMP] = 
			makeIns("RTI", ADDR_MODE_IMP, ins_rti);
		instructions[INS_EOR_IDX] = 
			makeIns("EOR", ADDR_MODE_IDX, ins_eor);
		instructions[INS_EOR_ZP] = 
			makeIns("EOR", ADDR_MODE_ZP, ins_eor);
		instructions[INS_LSR_ZP] = 
			makeIns("LSR", ADDR_MODE_ZP, ins_lsr);
		instructions[INS_PHA_IMP] = 
			makeIns("PHA", ADDR_MODE_IMP, ins_pha);
		instructions[INS_EOR_IMM] = 
			makeIns("EOR", ADDR_MODE_IMM, ins_eor);
		instructions[INS_LSR_ACC] = 
			makeIns("LSR", ADDR_MODE_ACC, ins_lsr);
		instructions[INS_JMP_ABS] = 
			makeIns("JMP", ADDR_MODE_ABS, ins_jmp);
		instructions[INS_EOR_ABS] = 
			makeIns("EOR", ADDR_MODE_ABS, ins_eor);
		instructions[INS_LSR_ABS] = 
			makeIns("LSR", ADDR_MODE_ABS, ins_lsr);
		instructions[INS_PHP_IMP] = 
			makeIns("PHP", ADDR_MODE_IMP, ins_php);
		instructions[INS_BVC_REL] = 
			makeIns("BVC", ADDR_MODE_REL, ins_bvc);
		instructions[INS_EOR_IDY] = 
			makeIns("EOR", ADDR_MODE_IDY, ins_eor);
		instructions[INS_EOR_ZPX] = 
			makeIns("EOR", ADDR_MODE_ZPX, ins_eor);
		instructions[INS_LSR_ZPX] = 
			makeIns("LSR", ADDR_MODE_ZPX, ins_lsr);
		instructions[INS_CLI_IMP] = 
			makeIns("CLI", ADDR_MODE_IMP, ins_cli);
		instructions[INS_EOR_ABY] = 
			makeIns("EOR", ADDR_MODE_ABY, ins_eor);
		instructions[INS_ORA_IMM] = 
			makeIns("ORA", ADDR_MODE_IMM, ins_ora);
		instructions[INS_EOR_ABX] = 
			makeIns("EOR", ADDR_MODE_ABX, ins_eor);
		instructions[INS_LSR_ABX] = 
			makeIns("LSR", ADDR_MODE_ABX, ins_lsr);
		instructions[INS_RTS_IMP] = 
			makeIns("RTS", ADDR_MODE_IMP, ins_rts);
		instructions[INS_ADC_IDX] = 
			makeIns("ADC", ADDR_MODE_IDX, ins_adc);
	}

	Word PC;
	Byte SP;
	Byte A, X, Y;
	Byte    C:1,
		Z:1,
		I:1,
		D:1,
		B:1,
		UNUSED:1,
		V:1,
		N:1;

	const Word INITIAL_SP = 0x01FF;
	const Word INITIAL_PC = 0xFFFC;

	void Init() {
		PC = INITIAL_PC;
		SP = INITIAL_SP;
		A = X = Y = 0;
		C = Z = I = D = B = V = N = 0;
		setupInstructionMap();
	}

	void Exception() {
		printf("CPU Exception");
		throw -1;
	}

	void SetFlagsForRegister(Byte b) {
		Z = (b == 0);
		N = (b & 0b10000000) >> 7;
	}

	void SetFlagsForCompare(Byte b, Byte v) {
		C = (b >= v);
		Z = b == v;
		N = (b & 0b10000000) >> 7;
	}

	Byte FetchIns() {
		Byte ins = mem.ReadByte(PC);
		PC++;
		return ins;
	}

	void Push(Byte value) {
		mem.WriteByte(SP, value);
		SP--;
	}

	Byte Pop() {
		SP++;
		return mem.ReadByte(SP);
	}

	Byte getData(unsigned long mode) {
		Byte data;
		Word address;

		switch (mode) {
		case ADDR_MODE_IMP:
			return 0;
		case ADDR_MODE_ACC:
			return 0;
		case ADDR_MODE_IMM:
			data = FetchIns();
			break;
		case ADDR_MODE_ZP:
			address = FetchIns();
		        data = mem.ReadByte(address);
			break;
		case ADDR_MODE_ZPX:
			address = FetchIns();
			address += X;
			data = mem.ReadByte(address);
			break;
		case ADDR_MODE_ZPY:
			data = FetchIns();
			data += Y;
			Cycles += 2;
			break;
		case ADDR_MODE_REL:
			data = FetchIns();
			data += PC;
			break;
		case ADDR_MODE_ABS:
			address = mem.ReadWord(PC);
			PC += 2;
			data = mem.ReadByte(address);
			break;
		case ADDR_MODE_ABX:
			address = mem.ReadWord(PC);
			address += X;
			PC += 3;
			break;
		case ADDR_MODE_ABY:
			address = mem.ReadWord(PC);
			address += Y;
			PC += 3;
			break;
		case ADDR_MODE_IND:
			return 0;	// Only used by JMP
		case ADDR_MODE_IDX:
			data = FetchIns();
			data += X;
			Cycles++;
			data = mem.ReadByte(data);
			break;
		case ADDR_MODE_IDY:
			data = FetchIns();
			address = mem.ReadWord(data);
			address += Y;
			Cycles++;
			data = mem.ReadByte(address);
			break;
		default:
			printf("Invalid addressing mode: 0x%ld\n", mode);
			Exception();
			break;
		}

		return data;
	}

	void ExecuteOneInstruction() {
		Byte opcode = FetchIns();
		struct instruction ins;

		if (instructions.count(opcode) == 0) {
			printf("Invalid opcode 0x%x\n", opcode);
			Exception();
		}

		ins = instructions[opcode];
		printf ("  instruction %s  addrmode %lx\n",
			ins.name, ins.addrmode);
		
					
		ins.opfn(this, ins.addrmode);
	}
		
};

CPU cpu;

void and_imm() {
	mem.Init();
	cpu.Init();
	mem[0xFFFC] = CPU::INS_AND_IMM;
	mem[0xFFFD] = 0x0F;
	cpu.A = 0xFF;
	unsigned long StartC;

	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("and_imm: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);

	if (cpu.A == 0x0f) {
		printf("PASSED\n");
	} else {
		printf("FAILED - Expected A == 0x0f\n");
		throw -1;
	}
}


void and_zp() {
	mem.Init();
	cpu.Init();

	mem[0xFFFC] = CPU::INS_AND_ZP;
	mem[0xFFFD] = 0x10;
	mem[0x0010] = 0x0F;
	cpu.A = 0xFF;
	unsigned long StartC;

	StartC = Cycles;

	cpu.ExecuteOneInstruction();

	printf("and_zp: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);
	if (cpu.A == 0x0f) {
		printf("PASSED\n");
	} else {
		printf("FAILED - Expected A == 0x0f\n");
		throw -1;
	}
}

void and_set_z_flag() {
	unsigned long StartC;

	mem.Init();
	cpu.Init();
	mem[0xFFFC] = 0x29;
	mem[0xFFFD] = 0;
	cpu.A = 0xFF;

	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("and_set_z_flag: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);
}
#if 0
void ora_imm() {
	unsigned long StartC;

	printf("======= ora_imm():\n");
	mem.Init();
	cpu.Init();
	mem[0xfffc] = 0x09;
	mem[0xfffd] = 0x10;
	cpu.A = 0x00;
	
	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("ora_imm: A = 0x%x\n", cpu.A);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);

}


void ora_zp() {
	unsigned long StartC;

	printf("======= ora_zp():\n");
	mem.Init();
	cpu.Init();
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
	unsigned long StartC;

	printf("======= pha():\n");
	mem.Init();
	cpu.Init();
	mem[0xfffc] = 0x48;
	cpu.A = 0x10;
	
	StartC = Cycles;
	cpu.ExecuteOneInstruction();

	printf("ora_zp: SP_START = 0x%x\n", mem[cpu.INITIAL_SP]);
	printf("cpu.Z == %d\n", cpu.Z);
	printf("Cycles used = %ld\n", Cycles - StartC);
}
#endif       
int main() {
	printf("MOS 6502\n");
	and_imm();
	and_zp();
	and_set_z_flag();
//
//	ora_imm();
//	ora_zp();
//	pha();
}

	
