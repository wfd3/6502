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

#include <fmt/format.h>
#include <fmt/core.h>

#include <stdio.h>
#ifdef __linux__
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "6502.h"

constexpr uint8_t ACTION_RETURN = 0;
constexpr uint8_t ACTION_CONTINUE = 1;

Word listPC;

///////////
// Helper functions

std::string stripSpaces(std::string input) {
	std::string result = input;
	
	result.erase(std::remove(result.begin(), result.end(), ' '),
		     result.end());
	return result;
}

std::string split(std::string &input, const std::string &delim) {
    size_t pos = input.find(delim);

    if (pos == std::string::npos) {
	    std::string part = input;
	    input = "";
	    return part;
    }

    std::string part = input.substr(0, pos);
    input = input.substr(pos + delim.length());
    return part;
}

std::string wrapText(const std::string& text, int width, int tabLength) {
    std::string result;
    std::string line;
    int lineLength = 0;

    for (char c : text) {
        if (c == ' ' && lineLength >= width - tabLength) {
            result += line + '\n';
            line.clear();
            lineLength = 0;
	    // Add a tab on the new line
            line.append(tabLength, ' ');
            lineLength += tabLength;
        } else if (c == '\t') {
            int spacesToAdd = tabLength - (lineLength % tabLength);
            line.append(spacesToAdd, ' '); // Add spaces for the tab
            lineLength += spacesToAdd;
        } else {
            line += c;
            lineLength++;
        }
    }

    result += line; // Add any remaining text to the result
    return result;
}

#ifdef __linux__
//////////
// readline helpers
void getReadline(std::string &line) {
	char *c_line = readline(":");
	if (c_line == nullptr) { // ^D
		line = "continue";
	} else if (c_line[0] == '\0') {
		line = "";
	} else {
		line = c_line;
		add_history(c_line);
		free(c_line);
	}
}

// Non-lambda completion generator function.  This requires a non-class version
// of setupDebugCommands.
char* readlineCommandGenerator(const char* text, int state) {
    static size_t listIndex = 0;
    static size_t length;
    auto commands = CPU::setupDebugCommands();
   
    if (!state) {
	    listIndex = 0;
	    length = strlen(text);
    }

    while (listIndex < commands.size()) {
	    const auto& cmd = commands[listIndex++];
	    if (strncmp(cmd.command, text, length) == 0) {
		    return strdup(cmd.command);
	    }
    }
    
    return NULL;
}

// Static completion callback function outside the class
extern "C" char **readlineCompletionCallback(const char* text,
					     [[maybe_unused]] int start,
					     [[maybe_unused]] int end) {
	auto commands = CPU::setupDebugCommands();

	rl_attempted_completion_over = 1;

	// See if the text so far is a command that wants file completion
	for(const auto& cmd : commands) {
		size_t cmd_len = strlen(cmd.command);
		if(start >= (int) cmd_len &&
		   strncmp(rl_line_buffer, cmd.command, cmd_len) == 0 &&
		   rl_line_buffer[cmd_len] == ' ' &&			
		   cmd.doFileCompletion) {
			rl_attempted_completion_over = 0;
			return rl_completion_matches(text,
					      rl_filename_completion_function);
		}
	}
	
	// Otherwise use custom command generator
	return rl_completion_matches(text,
				     readlineCommandGenerator);
}

void setupReadline() {
	// Setup GNU readline	
	rl_completion_query_items = 50;
	rl_attempted_completion_function = readlineCompletionCallback;
}
#endif

#ifdef _WIN64
void setupReadline() { }

void getReadline(std::string& line) {
	fmt::print(": ");
	std::getline(std::cin, line);
}
#endif

//////////
// CPU State information 

void CPU::printCPUState() {
	auto yesno = [](bool b) -> std::string {
		return b ? "Yes" : "No";
	};
	
	fmt::print(" | PC: {:04x} SP: {:02x}\n", PC, SP );
	// fmt::print() doesn't like to print out union/bit-field members?
	fmt::print(" | C:{} Z:{} I:{} D:{} B:{} U:{} V:{} N:{} (PS: {:#x})\n",
		   (int) Flags.C, (int) Flags.Z, (int) Flags.I, (int) Flags.D,
		   (int) Flags.B, (int) Flags._unused, (int) Flags.V,
		   (int) Flags.N, PS);
	fmt::print(" | A: {:02x} X: {:02x} Y: {:02x}\n", A, X, Y );
	fmt::print(" | Pending: IRQ - {}, NMI - {}, Reset - {}\n",
		   yesno(pendingIRQ()), yesno(pendingNMI()),
		   yesno(pendingReset()));
	fmt::print(" | IRQs: {}, NMIs: {}, BRKs: {}\n",
		    _IRQCount, _NMICount, _BRKCount);
	fmt::print(" | Cycle: {}\n", Cycles.get());
}

void CPU::dumpStack() {
	Byte p = INITIAL_SP;
	Word a;

	fmt::print("# Stack [SP = {:02x}]\n", SP);
	if (p == SP)
		fmt::print("# Empty stack\n");

	while (p != SP) {
		a = STACK_FRAME | p;
		fmt::print("# [{:04x}] {:02x}\n", a, mem.Read(a));
		p--;
	}
}

//////////
// Disassembler

std::string CPU::decodeArgs(Byte ins, bool atPC) {

	Byte mode = _instructions.at(ins).addrmode;
	Byte byteval;
	Word wordval;
	SByte rel;
	std::string out;

	switch (mode) {
	case ADDR_MODE_IMP:
		return "";

	case ADDR_MODE_ACC:
		return "A";

	case ADDR_MODE_IMM: 
		return fmt::format("#${:02x}", readByteAtPC());

	case ADDR_MODE_ZP:
		return fmt::format("${:02x}", readByteAtPC());

	case ADDR_MODE_ZPX:
		byteval = readByteAtPC();
		out = fmt::format("${:02x},X",  byteval);
		if (atPC)
			out += fmt::format(" [address {:04x}]", byteval + X);
		return out;

	case ADDR_MODE_ZPY:
		byteval = readByteAtPC();
		out = fmt::format("${:02x},Y", byteval);
		if (atPC)
			out += fmt::format("[address {:04x}]", byteval + Y);
		return out;

	case ADDR_MODE_REL:
		rel = SByte(readByteAtPC());
		return fmt::format("${:04x}", PC + rel);

	case ADDR_MODE_ABS:
		return fmt::format("${:04x}", readWordAtPC());

	case ADDR_MODE_ABX:
		wordval = readWordAtPC();
		out = fmt::format("${:04x},X", wordval);
		if (atPC)
			out += fmt::format("[address {:04x}]", wordval + X);
		return out;

	case ADDR_MODE_ABY:
		wordval = readWordAtPC();
		out = fmt::format("${:04x},Y", wordval);
		if (atPC)
			out += fmt::format("[address {:04x}]", wordval + Y);
		return out;
		
	case ADDR_MODE_IND:
		return fmt::format("(${:04x})", readWordAtPC());

	case ADDR_MODE_IDX:
		byteval = readByteAtPC();
		wordval = byteval + X;
		if (wordval > 0xFF)
			wordval -= 0xFF;
		wordval = readWord(wordval);
		out = fmt::format("(${:02x},X)", byteval);
		if (atPC)
			out += fmt::format("[address {:04x}]", wordval);
		return out;
		
	case ADDR_MODE_IDY:
		byteval = readByteAtPC();
		wordval = readWord(byteval);
		wordval += Y;
		out = fmt::format("(${:02x}),Y", byteval);
		if (atPC)
			out += fmt::format("[address {:04x}]", wordval);
		return out;
	}

	return fmt::format("[Invalid addressing mode {}, opcode {:02x}]",
			   mode, ins);
}

Address_t CPU::disassembleAt(Address_t dPC, std::string& disassembly) {
	Address_t savePC = PC;
	Cycles_t saveCycles = Cycles;

	// Are we looking at the next address to execute?  If we are, then
	// the processor state is correct to calculate address offsets.
	bool atPC = (PC == dPC);

	PC = dPC;
	disassembly = fmt::format("{:04x}: ", PC);
	Byte opcode = readByteAtPC();

	// Assume that, while debugging, the PC must be at the
	// start of an instruction sequence.
	if (_instructions.count(opcode) == 0) {
		disassembly += fmt::format(".byte ${:02x}", opcode);
	}
	else {

		disassembly += _instructions.at(opcode).name;

		auto args = decodeArgs(opcode, atPC);
		if (!args.empty())
			disassembly += "     " + args;
	}

	Address_t returnPC = PC;
	PC = savePC;
	Cycles = saveCycles;

	return returnPC;
}

Address_t CPU::disassemble(Address_t dPC, uint64_t cnt) {
	std::string ins;

	if (dPC > MAX_MEM) {
		fmt::print("PC at end of memory");
		return dPC;
	}

	do {
		dPC = disassembleAt(dPC, ins);
		fmt::print("{}\n", ins);
	} while (--cnt && dPC < MAX_MEM);
	
	return dPC;
}

//////////
// Debug on/off?

void CPU::toggleDebug() {
	debugMode = !debugMode;
	fmt::print("# Debug mode ");
	if (debugMode)
		fmt::print("enabled\n");
	else
		fmt::print("disabled\n");
}

void CPU::setDebug(bool d) {
	debugMode = d;
	fmt::print("# Debug mode ");
	if (debugMode)
		fmt::print("enabled\n");
	else
		fmt::print("disabled\n");
}

//////////
// Breakpoints

void CPU::listBreakpoints() {
	std::vector<Word>::iterator i;
	int c = 0;

	fmt::print("# Active breakpoints:\n");
	for (i = breakpoints.begin(); i < breakpoints.end(); i++) {
		fmt::print("  {:04x} ", *i);
		c++;
		if (c == 4) {
			c = 0;
			fmt::print("\n");
		}
	}
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
			fmt::print("# Removed breakpoint at {:04x}\n", *i);
			return;
		}
	}
	fmt::print("# No breakpoint set at {:04x}\n", bp);
}

void CPU::addBreakpoint(Word bp) {
	if (bp > MAX_MEM) {
		fmt::print("Error: Breakpoint address outside of available "
			   "address range\n");
		return;
	}
	if (isBreakpoint(bp)) {
		fmt::print("# Breakpoint already set at {:04x}\n", bp);
		return;
	}
	breakpoints.push_back(bp);
	fmt::print("# Set breakpoint at {:04x}\n", bp);
}

//////////
// Backtrace

void CPU::showBacktrace() {
	std::vector<std::string>::iterator i = backtrace.begin();
	unsigned int cnt = 0;

	fmt::print("# Backtrace: {} entries\n", backtrace.size());
	for ( ; i < backtrace.end(); i++ )
		fmt::print("#{:02d}:  {}\n", cnt++, (*i).c_str());

}

void CPU::addBacktrace(Word backtracePC) {
	std::string ins;
	disassembleAt(backtracePC, ins);
	backtrace.push_back(ins);
}

void CPU::addBacktraceInterrupt(Word backtracePC) {
	std::string ins;
	disassembleAt(backtracePC, ins);
	ins += " [IRQ/NMI]";
	backtrace.push_back(ins);
}

void CPU::removeBacktrace() {
	if (!backtrace.empty())
		backtrace.pop_back();
}

//////////
// Debugger
std::vector<CPU::debugCommand> CPU::setupDebugCommands() {
	return {
		{ "help",      "h",  &CPU::helpCmd, false,
		  "This help message" 
		},
		{ "list",      "l",  &CPU::listCmd, false,
		  "List next 10 instructions.  'list xxxx' lists from address "
		  "xxxx. 'list' without an address either lists from current "
		  "program counter or continues the last listing."
		},
		{ "load",      "L",  &CPU::loadCmd, true,
		  "'load <file> <address>' loads the file named 'file' at "
		  "memory address 'address', overwriting any data.  This "
		  "command will fail if it attempts to load data on non-RAM "
		  "memory."
		},
		{ "run",   "r",  &CPU::runCmd, false,
		  "Run program at current Program Counter.  Optionally "
		  "run for [x] instructions then reutrn to debugger"
		},
		{ "stack",     "S",  &CPU::stackCmd, false,
		  "Show current stack elements"
		},
		{ "break",     "b",  &CPU::breakpointCmd, false,
		  "Add, remove or show current breakpoints.  'break  "
		  "xxxx' adds a breakpoint at address xxxx, 'break "
		  "-xxxx' removes the breakpoint at address xxxx, and "
		  "'break' alone will list active breakpoints"
		},
		{ "state",     "p",  &CPU::cpustateCmd, false,
		  "Show current CPU state"},
		{ "autostate", "a",  &CPU::autostateCmd, false,
		  "Display CPU state after every debugger command"
		},
		{ "listpc",   "P",  &CPU::resetListPCCmd, false,
		  "Reset where the 'list' command start to disassembe"
		},
		{ "mem",       "m",  &CPU::memdumpCmd, false,
		  "Examine or change memory"},
		{ "set",       "s",  &CPU::setCmd, false,
		  "set a register or CPU flag, (ex. 'set A=ff')"},
		{ "reset",     "" ,  &CPU::resetCmd, false,
		  "Reset the CPU and jump through the reset vector"
		},
		{ "continue",  "c",  &CPU::continueCmd, false,
		  "Exit the debugger and contunue running the CPU.  "
		},
		{ "loopdetect","ld", &CPU::loopdetectCmd, false,
		  "Enable or disable loop detection (ie, 'jmp *'"},
		{ "backtrace", "t",  &CPU::backtraceCmd, false,
		  "Show the current subroutine and break backtrace"
		},
		{ "where",     "w",  &CPU::whereCmd, false,
		  "Display the instruction at the Program Counter"
		},
		{ "watch",     "W",  &CPU::watchCmd, false,
		  "Add, remove or show current memory watchpoints. "
		  "'watch xxxx' adds a watchpoint at memory address "
		  "xxxx, 'watch -xxxx' removes the watchpoint at "
		  "memory address xxxx, and 'watch' alone will list "
		  "active watchpoints"
		},
		{ "map",       "M",  &CPU::memmapCmd, false,
		  "Display the current memory map"
		},
		{ "clock",     "",   &CPU::clockCmd, false,
		  "Toggle CPU speed emulation"
		},
		{ "quit",      "",   &CPU::quitCmd, false, 
		  "Quit the emulator"
		}
	};
}

int CPU::helpCmd([[maybe_unused]] std::string &line,
		 [[maybe_unused]] uint64_t &returnValue) {

	for (const auto& cmd : _debugCommands) {
		fmt::print("{:<10}: {}\n", cmd.command,
			   // Add ': '
			   wrapText(cmd.helpMsg, 80 - (10 + 2), 10+2)); 
	}
	
	return ACTION_CONTINUE;
}

int CPU::listCmd(std::string &line,
		 [[maybe_unused]] uint64_t &returnValue) {
	try {
		listPC = std::stoi(line, nullptr, 16);
	}
	catch (std::out_of_range& e) {
		fmt::print("Parse error: {}\n", e.what());
	}
	catch (...) {
	}

	listPC = disassemble(listPC, 10);
	
	return ACTION_CONTINUE;
}

int CPU::loadCmd(std::string &line,
		 [[maybe_unused]] uint64_t &returnValue) {
	std::string fname;
	Address_t address;

	std::istringstream iss(line);
	iss >> fname >> std::hex >> address;

	if (address > CPU::MAX_MEM) {
		fmt::print("Invalid address: {:04x}\n", address);
		return ACTION_CONTINUE;
	}

	fmt::print("Loading file {} at address {:04x}\n", fname, address);

	try {
		mem.loadDataFromFile(fname, address);
	}
	catch (std::exception &e) {
		fmt::print("Load failed: {}\n", e.what());
	}
	catch(...) {
		fmt::print("Load error: unknown exception\n");
	}
	
	return ACTION_CONTINUE;
}

int CPU::runCmd(std::string &line, uint64_t &returnValue) {

	try {
		returnValue = std::stoul(line);
	}
	catch(...) {
		returnValue = 1;
	}
	return ACTION_RETURN;
}

int CPU::stackCmd([[maybe_unused]] std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {

	dumpStack();
	return ACTION_CONTINUE;
}

int CPU::breakpointCmd(std::string &line,
		       [[maybe_unused]] uint64_t &returnValue) {

	Word addr;
	bool remove = false;

	if (line.empty()) {
		listBreakpoints();
		return ACTION_CONTINUE;
	}
		
	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	} 

	try {
		addr = (Word) std::stoul(line, nullptr, 16);
		if (remove)
			deleteBreakpoint(addr);
		else
			addBreakpoint(addr);
	}
	catch(...) {
		fmt::print("Parse error: {}\n", line);
	}
	
	return ACTION_CONTINUE;
}

int CPU::cpustateCmd([[maybe_unused]] std::string &line,
		     [[maybe_unused]] uint64_t &returnValue) {

	printCPUState(); 
	return ACTION_CONTINUE;
}

int CPU::autostateCmd([[maybe_unused]] std::string &line,
		      [[maybe_unused]] uint64_t &returnValue) {

	debug_alwaysShowPS = !debug_alwaysShowPS;
	fmt::print("# Processor status auto-display ");
	if (debug_alwaysShowPS)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled");
	
	return ACTION_CONTINUE;
}

int CPU::resetListPCCmd(std::string &line,
			[[maybe_unused]] uint64_t &returnValue) {
	Word i;

	try {
		i = (Word) std::stoul(line, nullptr, 16);
		if (i > MAX_MEM) {
			fmt::print("Error: Program Counter address outside of "
				   "available address range\n");
			return ACTION_CONTINUE;
		}
		listPC = i;
	}
	catch (...) {
		listPC = PC;
	}

	fmt::print("# List reset to PC {:04x}\n", listPC);
	
	return ACTION_CONTINUE;
}

int CPU::memdumpCmd(std::string &line,
		    [[maybe_unused]] uint64_t &returnValue) {
	Word addr1, addr2;
	Word value;		
	char equal, colon;
	
	auto s = stripSpaces(line);
	std::istringstream iss(s);

	auto rangeCheckAddr = [](Word a) {
		return a <= MAX_MEM;
	};

	auto rangeCheckValue =[](Word v) {
		return v <= 0xff;
	};
		
	// xxxx
	iss.seekg(0);
	iss >> std::hex >> addr1;
	if (!iss.fail() && iss.eof() && rangeCheckAddr(addr1)) {
		fmt::print("[{:04x}] {:02x}\n", addr1, mem.Read(addr1));
		return ACTION_CONTINUE;
	}

	// xxxx=val
	iss.seekg(0);
	iss >> std::hex >> addr1 >> equal >> std::hex >> value;
	if (!iss.fail() && iss.eof() && equal == '=' &&
	    rangeCheckAddr(addr1) && rangeCheckValue(value)) {
		Byte oldval = mem.Read(addr1);
		mem.Write(addr1, (Byte) value);
		 fmt::print("# [{:04x}] {:02x} -> {:02x}\n",
			    addr1, oldval, (Byte) value);
		return ACTION_CONTINUE;
	}

	// xxxx:yyyy
	iss.seekg(0);
	iss >> std::hex >> addr1 >> colon >> std::hex >> addr2;
	if (!iss.fail() && iss.eof() && colon == ':' &&
	    rangeCheckAddr(addr1) && rangeCheckAddr(addr2)) {
		mem.hexdump(addr1, addr2);
		return ACTION_CONTINUE;;
	}

	// xxxx:yyyy=val
	iss.seekg(0);
	value = 0;
	iss >> std::hex >> addr1 >> colon >> std::hex >> addr2 >> equal
	    >> std::hex >> value;
	if (!iss.fail() && iss.eof() && colon == ':' && equal == '=' &&
	    rangeCheckAddr(addr1) && rangeCheckAddr(addr2) &&
	    rangeCheckValue(value)) {
		for (Address_t a = addr1; a <= addr2; a++)
			mem.Write(a, (Byte) value);
		return ACTION_CONTINUE;
	}

	// Handle parsing errors
	fmt::print("Parse error: '{}'\n", s);
	return ACTION_CONTINUE;
}

int CPU::memmapCmd([[maybe_unused]] std::string &line,
		   [[maybe_unused]] uint64_t &returnValue) {
	mem.printMap();
	return ACTION_CONTINUE;
}

int CPU::setCmd(std::string &line,
		[[maybe_unused]] uint64_t &returnValue) {
	std::string v;
	std::string reg;					
	uint64_t value;
	bool flipFlag = false;

	// TODO:  make 'set x 5' and 'set x=5' work.
	v = stripSpaces(line);
	reg = split(v, "=");
	if (reg.empty()) {
		fmt::print("Parse Error: register or flag required for set "
			   "command\n");
		return ACTION_CONTINUE;
	}


	// reg contains the register, s is the value.
	std::transform(reg.begin(), reg.end(), reg.begin(), ::toupper);
	try {
		value = std::stoul(v, nullptr, 16);
		if ((reg != "PC" && value > 0xff) ||
		    (reg == "PC" && value > 0xffff)) {
			fmt::print("Error: value would overflow register {}\n",
				   reg);
			return ACTION_CONTINUE;
		}
	}
	catch(...) {
		std::string flagChars = "CZIDBVN";
		bool containsFlagCharacter =
			std::any_of(flagChars.begin(), flagChars.end(),
				 [&reg](char c) {
				    return reg.find(c) != std::string::npos;
				 }
			      );
		if (containsFlagCharacter)
			flipFlag = true;
		else {
			fmt::print("Parse Error: '{}' is not a valid value for "
				   "set\n", v);
			return ACTION_CONTINUE;
		}
	}

	if (reg == "A") 
		A = (Byte) value & 0xff;
	else if (reg == "Y")
		Y = (Byte) value & 0xff;
	else if (reg == "X")
		X = (Byte) value & 0xff;
	else if (reg == "PC")
		PC = (Word) value & 0xffff;
	else if (reg == "SP")
		SP = (Byte) value & 0xff;
	else if (reg == "PS")
		PS = (Byte) value & 0xff;
	else if (reg == "C")
		if (flipFlag)
			Flags.C = !Flags.C;
		else
			Flags.C = (bool) value;
	else if (reg == "Z")
		if (flipFlag)
			Flags.Z = !Flags.Z;
		else
			Flags.Z = (bool) value;
	else if (reg == "I")
		if (flipFlag)
			Flags.I = !Flags.I;
		else
			Flags.I = (bool) value;
	else if (reg == "D")
		if (flipFlag)
			Flags.D = !Flags.D;
		else
			Flags.D = (bool) value;
	else if (reg == "B")
		if (flipFlag)
			Flags.B = !Flags.B;
		else
			Flags.B = (bool) value;
	else if (reg == "V")
		if (flipFlag)
			Flags.V = !Flags.V;
		else
			Flags.V = (bool) value;
	else if (reg == "N")
		if (flipFlag)
			Flags.N = !Flags.N;
		else
			Flags.N = (bool) value;
	else 
		fmt::print("# No register or status flag '{}'\n", reg);

	return ACTION_CONTINUE;
}

int CPU::resetCmd([[maybe_unused]] std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {

	fmt::print("# Resetting 6502\n");
	setDebug(false);
	Reset();
	returnValue = 1;
	return ACTION_RETURN;
}
		
int CPU::continueCmd([[maybe_unused]] std::string &line,
		     [[maybe_unused]] uint64_t &returnValue) {
	if (_hitException) {
		fmt::print("CPU Exception hit; can't continue.  Reset CPU to clear.\n");
		return ACTION_CONTINUE;
	}
	toggleDebug();
	returnValue = 1;
	return ACTION_RETURN;
}

int CPU::loopdetectCmd([[maybe_unused]] std::string &line,
		       [[maybe_unused]] uint64_t &returnValue) {

	toggleLoopDetection();
	fmt::print("# Loop detection ");
	if (debug_loopDetection)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	
	return ACTION_CONTINUE;
}

int CPU::backtraceCmd([[maybe_unused]] std::string &line,
		      [[maybe_unused]] uint64_t &returnValue) {

	showBacktrace();
	return ACTION_CONTINUE;
}

int CPU::whereCmd([[maybe_unused]] std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {

	disassemble(PC, 1);
	return ACTION_CONTINUE;
}

int CPU::watchCmd(std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		mem.listWatch();
		return ACTION_CONTINUE;
	}
	
	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	}

	try {
		addr = (Word) std::stoul(line, nullptr, 16);
		if (addr > MAX_MEM) {
			fmt::print("Error: Watchpoint address outside of "
				   "available address range\n");
			return ACTION_CONTINUE;
		}
		if (remove) {
			mem.clearWatch(addr);
			fmt::print("Watchpoint at memory address {:04x} "
				   "removed\n", addr);
		} else {
			mem.enableWatch(addr);
			fmt::print("Watchpoint at memory address {:04x} "
				   "added\n", addr);
		}
	}
	catch(...) {
		fmt::print("Parse error: {}\n", line);
	}
	
	return ACTION_CONTINUE;
}

int CPU::clockCmd([[maybe_unused]] std::string &line,
	     [[maybe_unused]] uint64_t &returnValue) {

	Cycles.toggleTimingEmulation();
	fmt::print("CPU timing emulation is ");
	if (Cycles.timingEmulation()) 
		fmt::print("enabled, per-cycle delay is {} ns\n",
			   Cycles.delayTimeNs());
	else
		fmt::print("disabled\n");

	return ACTION_CONTINUE;
}

int CPU::quitCmd([[maybe_unused]] std::string& line,
	[[maybe_unused]] uint64_t& returnValue) {
	fmt::print("Exiting emulator\n");
	exit(0);
}

bool CPU::matchCommand(const std::string &input, debugFn_t &func) {

	for (const auto &cmd : _debugCommands) {
		if (std::strcmp(cmd.command, input.c_str()) == 0 ||
		    std::strcmp(cmd.shortcut, input.c_str()) == 0) {
			func = cmd.func;
			return true;
		}
	}
	return false;
}

uint64_t CPU::debugPrompt() {
	uint64_t returnValue, count = 1;
	std::string line;
	debugFn_t f;

	setupReadline();
	
	listPC = PC;
	disassemble(PC, 1);

	while(1) {
		getReadline(line);

		if (line.empty() && debug_lastCmd.empty()) // Blank input
			continue;

		if (line.empty() && !debug_lastCmd.empty()) {
			line = debug_lastCmd;
		}

		// Check if command is numbers, convert them to
		// integer and return it.
		try {
			count = std::stol(line);
			debug_lastCmd = line;
			return count;
		}
		catch(...) {
			// line isn't numbers, continue.
		}

		auto command = split(line, " ");
		
		if (matchCommand(command, f) ==false) {
			fmt::print("Unknown command '{}'\n", command);
			continue;
		}

		if (line != "continue")
			debug_lastCmd = line;

		auto action = (this->*f)(line, returnValue);
		if (action == ACTION_CONTINUE)
			continue;
		if (action == ACTION_RETURN)
			return returnValue;

	}
	return 1;
}

void CPU::debug() {
	uint64_t count;

	if (debugEntryFunc)
		debugEntryFunc();

	fmt::print("Debugger starting at PC {:#04x}\n", PC);

	debugMode = true;

	while (debugMode) {
		count = debugPrompt();

		while (count--) {
			executeOneInstruction();
			if (count)
				disassemble(PC, 1);
		}
	}

	fmt::print("# Exiting debugger\n");
	if (debugExitFunc)
		debugExitFunc();
}

std::tuple<uint64_t, uint64_t> CPU::traceOneInstruction() {
	disassemble(PC, 1);
	return executeOneInstruction();
}
