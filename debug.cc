#include <string>
#include <sstream>
#include <format>
#include <cstring>
#include <memory>
#include <iostream>
#include <cstdarg>

#include "6502.h"

std::string stripSpaces(std::string s) {
	std::string::iterator it;
	std::string new_s;

	for (it = s.begin(); it != s.end(); it++) {
		if (*it != ' ')
			new_s += *it;
	}
	return new_s;
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
	
std::tuple<CPU::Cycles_t, CPU::Cycles_t> CPU::TraceOneInstruction() {
	
	CPU::disassemble(PC, 1);
	return CPU::ExecuteOneInstruction();
}

void CPU::ExecuteTrace() {
	while (1) {
		CPU::TraceOneInstruction();
	}
}

std::string CPU::decodeArgs(unsigned long mode, Address_t PC) {
	unsigned long addrmode = mode &  0b00111111111111;
	
	switch (addrmode) {
	case ADDR_MODE_IMP:
		return "";
	case ADDR_MODE_IMM: 
		return std::format("#${:02x}", ReadByteAtPC());
	case ADDR_MODE_ACC:
		return "A";
	case ADDR_MODE_ZP:
		return std::format("${:02x}", ReadByteAtPC());
	case ADDR_MODE_ZPX:
		return std::format("${:02x},X", ReadByteAtPC());
	case ADDR_MODE_ZPY:
		return std::format("${:02x},Y", ReadByteAtPC());
	case ADDR_MODE_REL:
		return std::format("${:02x}", SByte(ReadByteAtPC()));
	case ADDR_MODE_ABS:
		return std::format("${:04x}", ReadWord(PC));
	case ADDR_MODE_ABX:
		return std::format("${:04x},X", ReadWord(PC));
	case ADDR_MODE_ABY:
		return std::format("${:04x},Y", ReadWord(PC));
	case ADDR_MODE_IND:
		return std::format("(${:04x})", ReadWord(PC));
	case ADDR_MODE_IDX:	
		return std::format("(${:02x},X)", ReadByteAtPC());
	case ADDR_MODE_IDY:
		return std::format("(${:02x},Y", ReadByteAtPC());
	default:
		Exception("Invalid addressing mode: 0x%ld\n", mode);
		break;
	}
	return "";
}

Address_t CPU::disassemble(Address_t dPC, unsigned long cnt) {
	Word savePC, opcode;
	std::string s;

	savePC = PC;
	PC = dPC;
	do {
		std::cout << std::format("{:04x}: ", PC);
		opcode = ReadByteAtPC();

		if (instructions.count(opcode) == 0) {
			std::cout << std::format("[Invalid opcode 0x{:02x}]",
						 opcode) << std::endl;
			continue;
		}
		
		std::cout <<
			std::format("{} {}", instructions[opcode].name,
			    decodeArgs(instructions[opcode].addrmode, PC))
		  << std::endl;

	} while (--cnt);

	dPC = PC;
	PC = savePC;
	return dPC;
}

void CPU::PrintCPUState() {
	printf( "| PC: %04x SP: %02x\n", PC, SP );
	printf( "| C:%d Z:%d I:%d D:%d B:%d V:%d N:%d\n",
		Flags.C, Flags.Z, Flags.I, Flags.D, Flags.B, Flags.V, Flags.N);
	printf( "| A: %02x X: %02x Y: %02x\n", A, X, Y );
}

void CPU::debuggerPrompt() {

	if (CPU::debug_alwaysShowPS)
		CPU::PrintCPUState();

	std::cout << ": ";
}

unsigned long CPU::debugPrompt() {
	unsigned long count = 1;
	Address_t listPC;
	std::string command, line;

	listPC = PC;
	for (std::string line;
	     CPU::debuggerPrompt(), std::getline(std::cin, line); ) {
		std::istringstream iss(line);
		
		if (!(iss >> command)) // Blank input
			return 1;

		iss.seekg(0);
		if (iss >> count) {
			std::cout << std::format("# Running {} instructions",
						 count) << std::endl;
			return count;
		}

		// help
		if (command == "help" || command == "h" || command == "?") {
			std::cout <<
				"# help" << std::endl <<
				"# list <x>" << std::endl <<
				"# run <x>" << std::endl <<
				"# <number>" << std::endl <<
				"# stack" << std::endl <<
				"# cpu" << std::endl <<
				"# psalways" << std::endl << 
				"# listreset" << std::endl <<
				"# mem start:end" << std::endl <<
				"# mem address=value" << std::endl <<
				"# mem start:end=value " << std::endl <<
				"# quit" << std::endl;
		}

		// quit
		if (command == "quit" || command == "q")
			return 0;

		// psalways
		if (command == "psalways" || command == "a") {
			CPU::debug_alwaysShowPS = !CPU::debug_alwaysShowPS;
			std::cout << "# Processor status auto-display ";
			if (CPU::debug_alwaysShowPS)
				std::cout << "enabled" << std::endl;
			else 
				std::cout << "disabled" << std::endl;
		}
		
		if (command == "cpu" || command == "c" || command == "ps")
			CPU::PrintCPUState();

		// stack
		if (command == "stack" || command == "s")
			CPU::dumpstack();

		// list instructions
		if (command == "list" || command == "l") {
			unsigned int instructions;
			std::string s = iss.str();
			if (command == "list")
				s.erase(0, 4);
			else
				s.erase(0, 1);
			if (sscanf(s.c_str(), "%d", &instructions) != 1)
				instructions = 10;
			listPC = CPU::disassemble(listPC, instructions);
		}

				// list instructions
		if (command == "run" || command == "r") {
			unsigned int instructions;
			std::string s = iss.str();
			if (command == "run")
				s.erase(0, 3);
			else
				s.erase(0, 1);
			if (sscanf(s.c_str(), "%d", &instructions) != 1)
				instructions = 1;
			return instructions;
		}

		// reset list PC
		if (command == "listreset" || command == "lr") {
			listPC = PC;
			std::cout << "# List reset to PC " <<
				std::format("{:04x}", PC) << std::endl;
		}

		// mem dump/set
		if (command == "mem" || command == "m") {
			std::string s = iss.str();
			if (command == "mem")
				s.erase(0, 4);
			else
				s.erase(0, 2);
			CPU::parseMemCommand(s);
		}
	}
	return 0;
}

void CPU::Debug() {
	unsigned long count = 1;

	std::cout << "Starting trace at PC " << std::format("0x{:04x}", PC)
		  << std::endl;
	CPU::PrintCPUState();
	while (count) {
		count--;
		CPU::TraceOneInstruction();
		if (count == 0) 
			count = CPU::debugPrompt();
		else if (CPU::debug_alwaysShowPS)
			CPU::PrintCPUState();
	}
}

void CPU::parseMemCommand(std::string s) {
	unsigned int addr1, addr2, value;
	int e;
	
	s = stripSpaces(s);

	printf("s: %s\n",s.c_str());
	
	// mem:mem=val
	e = sscanf(s.c_str(), "%x:%x=%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &addr2,
		   (unsigned int *) &value);
	if (e == 3) { 
		for (Address_t a = addr1; a <= addr2; a++)
			mem->WriteByte(a, (Byte) value);

		return;
	}
	
	// mem:mem
	e = sscanf(s.c_str(), "%x:%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &addr2);
	if (e == 2) { 
		mem->Hexdump(addr1, addr2);

		return;
	}
	
	// mem=mem
	e = sscanf(s.c_str(), "%x=%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &value);
	if (e == 2) { 
		Byte oldval = mem->ReadByte(addr1);
		mem->WriteByte(addr1, (Byte) value);
		std::cout << std::format("# [{:04x}] {:02x} -> {:02x}",
					 addr1, oldval, (Byte) value) 
			  << std::endl;

		return;
	}
	
	// mem
	e = sscanf(s.c_str(), "%x",
		   (unsigned int *) &addr1);
	if (e == 1) { 
		printf("single value %x\n", addr1);
		std::cout << std::format(
			"[{:04x}] {:02x}",
			addr1, mem->ReadByte(addr1))
			  << std::endl;

		return;
	}
	
	std::cout << "# Parse error" << std::endl;
}
