//
// Debugger support for emulated 6502.
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

#include <string>
#include <sstream>
#include <cstring>
#include <memory>
#include <iostream>
#include <cstdarg>
#include <algorithm>
#include <vector>
#include <cstdarg>

#include "6502.h"

///////////
// Helper functions

std::string stripSpaces(std::string s) {
	std::string::iterator it;
	std::string new_s;

	for (it = s.begin(); it != s.end(); it++) {
		if (*it != ' ')
			new_s += *it;
	}
	return new_s;
}

bool cmpString(const std::string &s, const std::string &t) {

	size_t t_len = t.length();

	if (s.length() < t_len)
		return false;
	
	return s.compare(0, t_len, t, 0, t_len) == 0;
}

void CPU::dumpstack() {
	Byte p = INITIAL_SP;
	Word a;

	printf("# Stack dump [SP = %02x]:\n", SP);
	while (p != SP) {
		a = STACK_FRAME | p;
		printf("# [%04x] %02x\n", a, mem->Read(a));
		p--;
	}
}
	
static const std::string vformat(const char * const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);

    return std::string(zc.data(), iLen);
}

std::tuple<Byte, Byte> CPU::TraceOneInstruction() {
	CPU::disassemble(PC, 1);
	return CPU::ExecuteOneInstruction();
}

std::string CPU::decodeArgs(Byte ins) {
	Byte mode = instructions[ins].addrmode;
	Byte byteval;
	Word wordval;
	SByte rel;

	switch (mode) {
	case ADDR_MODE_IMP:
		return "";
	case ADDR_MODE_ACC:
		return "A";
	case ADDR_MODE_IMM: 
		return vformat("#$%02x", ReadByteAtPC());
	case ADDR_MODE_ZP:
		return vformat("$%02x", ReadByteAtPC());
	case ADDR_MODE_ZPX:
		byteval = ReadByteAtPC();
		return vformat("$%02x,X  [address %04x]", byteval, byteval + X);
	case ADDR_MODE_ZPY:
		byteval = ReadByteAtPC();
		return vformat("$%02x,Y  [address %04x]", byteval, byteval + Y);
	case ADDR_MODE_REL:
		rel = SByte(ReadByteAtPC());
		return vformat("$%04x", PC + rel);
	case ADDR_MODE_ABS:
		return vformat("$%04x", ReadWordAtPC());
	case ADDR_MODE_ABX:
		wordval = ReadWordAtPC();
		return vformat("$%04x,X  [address %04x]", wordval, wordval + X);
	case ADDR_MODE_ABY:
		wordval = ReadWordAtPC();
		return vformat("$%04x,Y  [address %04x]", wordval, wordval + Y);
	case ADDR_MODE_IND:
		return vformat("($%04x)", ReadWordAtPC());
	case ADDR_MODE_IDX:
		byteval = ReadByteAtPC();
		wordval = byteval + X;
		if (wordval > 0xFF)
			wordval -= 0xFF;
		wordval = ReadWord(wordval);
		return vformat("($%02x,X)  [address %04lx]", byteval, wordval);
	case ADDR_MODE_IDY:
		byteval = ReadByteAtPC();
		wordval = ReadWord(byteval);
		wordval += Y;
		return vformat("($%02x),Y  [address %04lx]", byteval, wordval);
	}

	return vformat("[Invalid addressing mode %d, opcode %02x]", mode, ins);
}

Address_t CPU::disassembleAt(Address_t dPC, std::string &d) {
	Address_t savePC, returnPC;
	Cycles_t saveCycles;
	Byte opcode;
	std::string args;

	savePC = PC;
	saveCycles = Cycles;
	PC = dPC;

	d = vformat("%04x: ", PC);

	opcode = ReadByteAtPC();

	// Assume that, while debugging, the PC can not be at the
	// start of an instruction sequence.
	if (instructions.count(opcode) == 0) {
		d += vformat(".byte $%02x", opcode);
	} else {
		
		d += vformat("%s", instructions[opcode].name);

		args = decodeArgs(opcode);
		if (!args.empty())
			d += "     " + args;
	}

	returnPC = PC;
	PC = savePC;
	Cycles = saveCycles;

	return returnPC;
}

Address_t CPU::disassemble(Address_t dPC, unsigned long cnt) {
	std::string out;
	
	do {
		dPC = CPU::disassembleAt(dPC, out);
		std::cout << out << std::endl;
	} while (--cnt && dPC < MAX_MEM);
	
	return dPC;
}

void CPU::PrintCPUState() {
	printf(" | PC: %04x SP: %02x\n", PC, SP );
	printf(" | C:%d Z:%d I:%d D:%d B:%d U:%d V:%d N:%d (PS: %0x)\n",
		Flags.C, Flags.Z, Flags.I, Flags.D, Flags.B, Flags._unused,
		Flags.V, Flags.N,
		PS);
	printf(" | A: %02x X: %02x Y: %02x\n", A, X, Y );
	printf(" | Cycle: %lu\n", Cycles.get()); 
}

void CPU::debuggerPrompt() {

	std::cout << ": ";
}

void CPU::parseMemCommand(std::string s) {
	unsigned int addr1, addr2, value;
	int e;
	
	s = stripSpaces(s);

	// mem:mem=val
	e = sscanf(s.c_str(), "%x:%x=%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &addr2,
		   (unsigned int *) &value);
	if (e == 3) { 
		for (Address_t a = addr1; a <= addr2; a++)
			mem->Write(a, (Byte) value);

		return;
	}
	
	// mem:mem
	e = sscanf(s.c_str(), "%x:%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &addr2);
	if (e == 2) { 
		mem->hexdump(addr1, addr2);

		return;
	}
	
	// mem=mem
	e = sscanf(s.c_str(), "%x=%x",
		   (unsigned int *) &addr1,
		   (unsigned int *) &value);
	if (e == 2) { 
		Byte oldval = mem->Read(addr1);
		mem->Write(addr1, (Byte) value);
		std::cout << vformat("# [%04x] %02x -> %02x",
				     addr1, oldval, (Byte) value) 
			  << std::endl;

		return;
	}
	
	// mem
	e = sscanf(s.c_str(), "%x",
		   (unsigned int *) &addr1);
	if (e == 1) { 
		std::cout << vformat("[%04x] %02x", addr1, mem->Read(addr1))
			  << std::endl;

		return;
	}
	
	std::cout << "# Parse error" << std::endl;
}

unsigned long CPU::debugPrompt() {
	unsigned long count = 1;
	Address_t listPC;
	std::string command;

	listPC = PC;

	if (CPU::debug_alwaysShowPS)
		CPU::PrintCPUState();

	CPU::disassemble(PC, 1);

	for (std::string line;
	     CPU::debuggerPrompt(), std::getline(std::cin, command); ) {

		if (command.empty() && debug_lastCmd.empty()) // Blank input
			continue;

		if (command.empty() && !debug_lastCmd.empty()) {
			command = debug_lastCmd;
		}

		debug_lastCmd = command;

		std::istringstream iss(command);
		if (iss >> count) {
			return count;
		}

 		// help
		if (cmpString(command, "help") ||
		    cmpString(command, "h") ||
		    cmpString(command, "?")) {
			std::cout <<
				"# help|h" << std::endl <<
				"# <number>" << std::endl <<
				"# list|l <x>" << std::endl <<
				"# run <x>" << std::endl <<
				"# <number>" << std::endl <<
				"# stack|s" << std::endl <<
				"# break|b <address>" << std::endl <<
				"# processor-state|ps" << std::endl <<
				"# psalways|a" << std::endl << 
				"# listpc|pc [address]" << std::endl <<
				"# mem|m start:end" << std::endl <<
				"# mem|m address=value" << std::endl <<
				"# mem|m start:end=value " << std::endl <<
				"# reg|r <reg>=value" << std::endl <<
				"# flags|f X" << std::endl <<
				"# reset" << std::endl <<
				"# continue|c" << std::endl <<
				"# loop-detect|ld" << std::endl <<
				"# backtrace|t" << std::endl <<
				"# where|w" << std::endl <<
				"# quit|q" << std::endl;
			continue;
		}

		// quit
		if (cmpString(command, "quit") ||
		    cmpString(command, "q"))
			exit(0);

		if (cmpString(command, "continue") ||
		    cmpString(command, "co")) {
			CPU::ToggleDebug();
			return 1;
		}

		// where
		if (cmpString(command, "where") ||
		    cmpString(command, "w")) {
			CPU::disassemble(PC, 1);
			continue;
		}

		// psalways
		if (cmpString(command, "psalways") ||
		    cmpString(command, "a")) {
			CPU::debug_alwaysShowPS = !CPU::debug_alwaysShowPS;
			std::cout << "# Processor status auto-display ";
			if (CPU::debug_alwaysShowPS)
				std::cout << "enabled" << std::endl;
			else 
				std::cout << "disabled" << std::endl;

			continue;
		}

		// loop-detect
		if (cmpString(command, "loop-detect") ||
		    cmpString(command, "ld")) {
			toggleLoopDetection();
			std::cout << "# Loop detection ";
			if (CPU::debug_loopDetection)
				std::cout << "enabled" << std::endl;
			else 
				std::cout << "disabled" << std::endl;

			continue;
		}

		// cpu | ps
		if (cmpString(command, "processor-state") ||
		    cmpString(command, "ps")) {
			CPU::PrintCPUState(); 

			continue;
		}

		// stack
		if (cmpString(command, "stack") ||
		    cmpString(command, "s")) {
			CPU::dumpstack();
			continue;
		}

		// backtrace
		if (cmpString(command, "backtrace") ||
		    cmpString(command, "t")) {
			CPU::showBacktrace();
			continue;
		}

		// list instructions
		if (cmpString(command, "list") ||
		    cmpString(command, "l")) {
			unsigned int instructions;
			if (cmpString(command, "list"))
				command.erase(0, 4);
			else
				command.erase(0, 1);
			if (sscanf(command.c_str(), "%d", &instructions) != 1)
				instructions = 10;
			listPC = CPU::disassemble(listPC, instructions);

			continue;
		}

		// list instructions
		if (cmpString(command, "run")) {
			unsigned int instructions;
			if (cmpString(command,"run"))
				command.erase(0, 3);
			else
				command.erase(0, 1);
			if (sscanf(command.c_str(), "%d", &instructions) != 1)
				instructions = 1;
			return instructions;
		}

		if (cmpString(command, "reset")) {
			printf("# Resetting 6502\n");
			CPU::SetDebug(false);
			CPU::Reset();
			return 1;
		}
		
		// reset list PC
		if (cmpString(command, "listpc") ||
		    cmpString(command, "p")) {
			unsigned int i;
			if (cmpString(command, "listpc"))
				command.erase(0, 6);
			else
				command.erase(0, 2);

			if (sscanf(command.c_str(), "%x", &i) == 1)
				listPC = (Word) i;
			else 
				listPC = PC;
			std::cout << "# List reset to PC " <<
				vformat("%04x", listPC) << std::endl;

			continue;
		}

		// mem dump/set
		if (cmpString(command, "mem") ||
		    cmpString(command, "m")) {
			if (cmpString(command, "mem"))
				command.erase(0, 4);
			else
				command.erase(0, 2);
			CPU::parseMemCommand(command);

			continue;
		}

		if (cmpString(command, "register") ||
		    cmpString(command, "reg")) {
			std::string s;
			std::string reg;					
			unsigned int value;
			char *r;

			if (cmpString(command, "register"))
				command.erase(0, 8);
			else
				command.erase(0, 3);
			s = stripSpaces(command);
			printf("%s\n", s.c_str());
			if (sscanf(s.c_str(),
				   "%m[aAxXtYsSpPcC]=%x", &r, &value) != 2) {
				std::cout << "# Parse error" << std::endl;
				continue;
			}

			reg = r;
			std::transform(reg.begin(), reg.end(), reg.begin(),
				       ::toupper);

			if (reg == "A") 
				A = (Byte) value;
			else if (reg == "Y")
				Y = (Byte) value;
			else if (reg == "X")
				X = (Byte) value;
			else if (reg == "PC")
				PC = (Word) value;
			else if (reg == "SP")
				SP = (Byte) value;
			else if (reg == "PS")
				PS = (Byte) value;
			else
				std::cout << "# No register '" << r << "'"
					  << std::endl;
			free(r);

			continue;
		}

		if (cmpString(command, "flag") ||
		    cmpString(command, "fl")) {
			std::string s;
			char c;
			
			command.erase(0, 2);
			s = stripSpaces(command);
			if (sscanf(s.c_str(),"%c", &c) != 1) {
				std::cout << "# Parse error" << std::endl;
				continue;
			}

			c = toupper(c);
			switch (c) {
			case 'C':
				Flags.C = !Flags.C;
				break;
			case 'Z':
				Flags.Z = !Flags.Z;
				break;
			case 'I':
				Flags.I = !Flags.I;
				break;
			case 'D':
				Flags.D = !Flags.D;
				break;
			case 'B':
				Flags.B = !Flags.B;
				break;
			case 'V':
				Flags.V = !Flags.V;
				break;
			case 'N':
				Flags.N = !Flags.N;
				break;
			default:
				std::cout << "# No status flag '" << c
					  << "'" << std::endl;
				break;
			}

			continue;
		}

		if (cmpString(command, "break")) {
			unsigned long addr;

			command.erase(0, 6);
			if (command[0] == '-') {
				command.erase(0,1);
				if (sscanf(command.c_str(), "%lx", &addr)==1) {
					printf("doing it\n");
					CPU::deleteBreakpoint(addr);
				}
			} else if (sscanf(command.c_str(), "%lx", &addr) == 1)
				CPU::addBreakpoint((Address_t) addr);
			else
				CPU::listBreakpoints();

			continue;
		}

		if (cmpString(command, "watch")) {
			unsigned long addr;

			command.erase(0, 6);
			if (command[0] == '-') {
				command.erase(0,1);
				if (sscanf(command.c_str(), "%lx", &addr)==1) {
					mem->clearWatch(addr);
				}
			} else if (sscanf(command.c_str(), "%lx", &addr) == 1)
				mem->enableWatch((Address_t) addr);
			else
				mem->listWatch();

			continue;
		}

	}
	return 1;
}

void CPU::Debug() {
	unsigned long count = 1;

	std::cout << "Debugger starting at PC " << vformat("0x%04x", PC)
		  << std::endl;

	CPU::debugMode = true;

	while (1) {
		count = CPU::debugPrompt();

		if (!CPU::debugMode) {
			printf("# Exiting debugger\n");
			return;
		}
		
		while (count--) {
			CPU::ExecuteOneInstruction();
			if (count)
				CPU::disassemble(PC, 1);
		}
	}
}

void CPU::ToggleDebug() {
	debugMode = !debugMode;
	std::cout << "# Debug mode ";
	if (debugMode)
		std::cout << "enabled\n";
	else
		std::cout << "disabled\n";
}

void CPU::SetDebug(bool d) {
	debugMode = d;
	std::cout << "# Debug mode ";
	if (debugMode)
		std::cout << "enabled\n";
	else
		std::cout << "disabled\n";
}

// Breakpoints
void CPU::listBreakpoints() {
	std::vector<Word>::iterator i;
	int c = 0;

	printf("# Active breakpoints:\n");
	for (i = breakpoints.begin(); i < breakpoints.end(); i++) {
		printf("  %04x ", *i);
		c++;
		if (c == 4) {
			c = 0;
			printf("\n");
		}
	}

	printf("\n");
}

bool CPU::isBreakpoint(Word _pc) {
	std::vector<Word>::iterator i;

	for (i = breakpoints.begin(); i < breakpoints.end(); i++) {
		if (*i == _pc)
			return true;
	}
	return false;
}

void CPU::deleteBreakpoint(Word bp) {
	std::vector<Word>::iterator i;

	for (i = breakpoints.begin(); i < breakpoints.end(); i++) {
		if (*i == bp) {
			breakpoints.erase(i);
			printf("# Removed breakpoint at %04x\n", *i);
			return;
		}
	}
	printf("# No breakpoint set at %04x\n", bp);
}

void CPU::addBreakpoint(Word bp) {
	if (isBreakpoint(bp)) {
		printf("# Breakpoint already set at %04x\n", bp);
		return;
	}
	breakpoints.push_back(bp);
	printf("# Set breakpoint at %04x\n", bp);
}

// Backtrace
void CPU::showBacktrace() {
	std::vector<std::string>::iterator i = backtrace.begin();
	unsigned int cnt = 0;

	printf("# Backtrace: %ld entries\n", backtrace.size());
	for ( ; i < backtrace.end(); i++ )
		printf("#%02d:  %s\n", cnt++, (*i).c_str());

}

void CPU::addBacktrace(Word PC) {
	std::string ins;
	disassembleAt(PC, ins);
	backtrace.push_back(ins);
}

void CPU::removeBacktrace() {
	if (!backtrace.empty())
		backtrace.pop_back();
}

// Loop detection
void CPU::toggleLoopDetection() {
	CPU::debug_loopDetection = !CPU::debug_loopDetection;
}
