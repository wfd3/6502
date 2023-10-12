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
#include <regex>

#include <fmt/format.h>
#include <fmt/core.h>

#include <stdio.h>
#ifdef __linux__
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
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

std::string stripLeadingSpaces(std::string input) {
	size_t first = input.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }
    return input.substr(first);
}

std::string stripTrailingSpaces(std::string input) {
	size_t last = input.find_last_not_of(" \t\n\r\f\v");
    if (last == std::string::npos) {
        return "";
    }
    return input.substr(0, last + 1);

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

bool isHexNumber(const std::string& str) {
    for(char ch : str) {
        if(!std::isxdigit(ch))
            return false;
    }
    return true;
}

bool CPU::lookupAddress(const std::string& line, Word& address) {
	if (line.empty()) {
		fmt::print("Incomplete command\n");
		return false;
	} 
	
	if (isHexNumber(line)) {
		try {
			address = (Word) std::stoul(line, nullptr, 16);
			return true;
		}
		catch(...) {
			fmt::print("Parse error: {}\n", line);
			return false;
		}
	}

	// see if line contains a label.
	if (!labelAddress(line, address)) {
		fmt::print("No label '{}'\n", line);
		return false;
	}
	
	return true;
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
    
    return nullptr;
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

// Capture ^C and ^-Backslash
void captureSignals() {
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

}

void restoreSignals() {
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
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
	auto fl = [](char c, bool b) -> char {
		return b ? std::toupper(c) : std::tolower(c);
	};

	fmt::print("| PC: {:04x} SP: {:02x}\n", PC, SP );
	// fmt::print() doesn't like to print out union/bit-field members?
	fmt::print("| Flags: {}{}{}{}{}{}{} PS: {:#x}\n",
		fl('C', Flags.C), fl('Z', Flags.Z), fl('I', Flags.I), fl('D', Flags.D),
		fl('B', Flags.B), fl('V', Flags.V), fl('N', Flags.N), PS);
	fmt::print("| A: {:02x} X: {:02x} Y: {:02x}\n", A, X, Y );
	fmt::print("| Pending: IRQ - {}, NMI - {}, Reset - {}, "
			   "PendingReset - {}\n",
		   yesno(pendingIRQ()), yesno(pendingNMI()),
		   yesno(_inReset), yesno(_pendingReset));
	fmt::print("| IRQs: {}, NMIs: {}, BRKs: {}\n",
		    _IRQCount, _NMICount, _BRKCount);
	fmt::print("| Cycle: {}\n", Cycles.get());
}

void CPU::dumpStack() {
	Byte p = INITIAL_SP;
	Word a;

	fmt::print("Stack [SP = {:02x}]\n", SP);
	if (p == SP)
		fmt::print("Empty stack\n");

	while (p != SP) {
		a = STACK_FRAME | p;
		fmt::print("[{:04x}] {:02x}\n", a, mem.Read(a));
		p--;
	}
}

//////////
// Disassembler

void CPU::decodeArgs(bool atPC, Byte ins, std::string& disassembly, 
					 std::string& opcodes, std::string& address, 
					 std::string& computedAddr) {

	Byte mode = _instructions.at(ins).addrmode;
	Byte byteval;
	Word wordval;
	SByte rel;
	std::string out, addr, label;

	switch (mode) {
	case ADDR_MODE_IMP:
		break;

	case ADDR_MODE_ACC:
		disassembly = "A";
		address = "";
		break;

	case ADDR_MODE_IMM:  // #$xx
		byteval = readByteAtPC();
		disassembly = fmt::format("#${:02x}", byteval);
		opcodes += fmt::format("{:02x} ", byteval);
		address = "";
		break;

	case ADDR_MODE_ZP:  // $xx
		byteval = readByteAtPC();
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);
	
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		opcodes += fmt::format("{:04x} ", byteval);
		break;

	case ADDR_MODE_ZPX:  // $xx,X
		byteval = readByteAtPC();
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);

		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:04x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + X);
		break;

	case ADDR_MODE_ZPY:  // $xx,Y
		byteval = readByteAtPC();
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:04x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + Y);
		break;

	case ADDR_MODE_REL:
		rel = SByte(readByteAtPC());
		computedAddr = fmt::format("${:04x}", PC + rel);

		disassembly = "";
		if (rel < 0) {
			disassembly += "-";
			rel = std::abs(rel);
		}
		disassembly += fmt::format("${:02x}", rel); 
		opcodes += fmt::format("{:02x} ", byteval);
		break;

	case ADDR_MODE_ABS:  // $xxxx
		wordval = readWordAtPC();
		label = addressLabel(wordval);
		addr = fmt::format("${:04x}", wordval);

		if (!label.empty()) {
			disassembly = label;
			address = address;
		} else {
			disassembly = addr;
			address = "";
		}
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case ADDR_MODE_ABX:  // $xxxx,X
		wordval = readWordAtPC();
		label = addressLabel(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty())  {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		computedAddr = fmt::format("${:04x}", wordval + X);
	
		break;		

	case ADDR_MODE_ABY:  // $xxxx,Y
		wordval = readWordAtPC();

		label = addressLabel(wordval);
		addr = fmt::format("${:04x}", wordval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval + Y);
		break;
		
	case ADDR_MODE_IND:  // $(xxxx)
		wordval = readWordAtPC();

		label = addressLabel(wordval);
		addr = fmt::format("{:04x}", wordval);
		if (!label.empty()) {
			disassembly = "(" + label + ")";
			address = addr;
		} else {
			disassembly = "(" + addr + ")";
			address = "";
		}
		opcodes += fmt::format("{:02x} {:02x}", 
							  wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case ADDR_MODE_IDX: // ($xx,X)
		byteval = readByteAtPC();
		wordval = byteval + X;
		if (wordval > 0xFF)
			wordval -= 0xFF;
		wordval = readWord(wordval);
		
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty())  {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = addr;
			address = "";
		}
		disassembly = "(" + disassembly + "),X";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval);
		break;
		
	case ADDR_MODE_IDY:  // ($xx),Y
		byteval = readByteAtPC();
		wordval = readWord(byteval);
		wordval += Y;
		
		label = addressLabel(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty()) { 
			disassembly = label;
			address = addr; 
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly = "(" + disassembly + "),Y";
		opcodes += fmt::format("{:02x}", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", wordval);
		break;
	
	default:
		disassembly += fmt::format("[Invalid addressing mode {}]",
			   					   mode);
	}
}

Address_t CPU::disassembleAt(Address_t dPC, std::string& disassembly) {
	Address_t savePC = PC;
	Cycles_t saveCycles = Cycles;
	std::string ins, bkpoint, args, opcodes, marker, address, computedAddress;
	bool atPC = (PC == dPC); 
	bool bytes = false;

	PC = dPC;
	if (isBreakpoint(PC))
		bkpoint = "B";

	if (atPC) 
		marker = "*";

	Byte opcode = readByteAtPC();
	opcodes = fmt::format("{:02x} ", opcode);

	if (_instructions.count(opcode) == 0) {
		ins = fmt::format(".byte ${:02x}", opcode);
		bytes = true;
	}
	else {
		ins = _instructions.at(opcode).name;
		decodeArgs(atPC, opcode, args, opcodes, address, computedAddress);
	}

//  B*| label^addr  : | 23 56 89 | ins     args | [opt. address]
	auto addr = fmt::format("{:04x}", dPC);
	std::string label = addressLabel(dPC);
	if (!label.empty()) 
		 addr += fmt::format(" ({})", label);
		
	disassembly = fmt::format("{:1.1}{:1.1}| {:20.20} | {:9.9}| {}     ", 
				              marker, bkpoint, addr, opcodes, ins);

	if (!bytes) 
		disassembly += fmt::format("{:<20} | {:<5.5} | {}", args, address, computedAddress);
	dPC = PC;
	PC = savePC;
	Cycles = saveCycles;

	return dPC;
}

Address_t CPU::disassemble(Address_t dPC, uint64_t cnt) {
	std::string disassembly;

	bool emulatedTimings = Cycles.timingEmulation();
	Cycles.disableTimingEmulation();
	
	if (dPC > MAX_MEM) {
		fmt::print("PC at end of memory");
		return dPC;
	}

	do {
		dPC = disassembleAt(dPC, disassembly);
		fmt::print("{}\n", disassembly);
	} while (--cnt && dPC < MAX_MEM);
	
	if (emulatedTimings)
		Cycles.enableTimingEmulation();

	return dPC;
}

//////////
// Debug on/off?

void CPU::toggleDebug() {
	debugMode = !debugMode;
}

void CPU::setDebug(bool d) {
	debugMode = d;
}

//////////
// Breakpoints

void CPU::listBreakpoints() {
	fmt::print("Active breakpoints:\n");
	for (size_t i = 0; i < breakpoints.size(); i++) {
		if (breakpoints[i]) {
			fmt::print("{:04x}", i);
			auto label = addressLabel(i);
			if (!label.empty()) 
				fmt::print(": {}", label);
			fmt::print("\n");
		}
	}
}

bool CPU::isBreakpoint(Word _pc) {
	if (_pc > MAX_MEM) 
		return false;
	return breakpoints[_pc];
}

void CPU::deleteBreakpoint(Word bp) {
	if (bp > MAX_MEM) 
		return;
	breakpoints[bp] = false;
	
	fmt::print("Removed breakpoint at {:04x}", bp);
	auto label = addressLabel(bp);
	if (!label.empty()) 
		fmt::print(": {}", label);
	fmt::print("\n");
}

void CPU::addBreakpoint(Word bp) {
	if (bp > MAX_MEM) {
		fmt::print("Error: Breakpoint address outside of available "
			   "address range\n");
		return;
	}
	if (isBreakpoint(bp)) {
		fmt::print("Breakpoint already set at {:04x}\n", bp);
		return;
	}
	breakpoints[bp] = true;

	fmt::print("Set breakpoint at {:04x}", bp);
	auto label = addressLabel(bp);
	if (!label.empty()) 
		fmt::print(": {}", label);
	fmt::print("\n");
}

//////////
// Backtrace

void CPU::showBacktrace() {
	std::vector<std::string>::iterator i = backtrace.begin();
	unsigned int cnt = 0;

	fmt::print("Backtrace: {} entries\n", backtrace.size());
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
// Labels

void CPU::showLabels() {
	if (addrToLabel.empty()) {
		fmt::print("No labels\n");
		return;
	}

	fmt::print("Address labels:\n");
	for (const auto& [address, label] : addrToLabel) {
		fmt::print("{:#04x}: {}\n", address, label);
	}
}

void CPU::addLabel(const Word address, const std::string label) {
	addrToLabel[address] = label;
	labelToAddr[label]   = address;
}

void CPU::removeLabel(const Word address) {
	auto it = addrToLabel.find(address);
	if (it == addrToLabel.end())
		return;

	auto label = addrToLabel.at(address);
	addrToLabel.erase(address);
	labelToAddr.erase(label);
}

std::string CPU::addressLabel(const Word address) {
	std::string label;

	auto it = addrToLabel.find(address);
	if (it != addrToLabel.end()) 
		label += fmt::format("{}", it->second);
	return label;
}

bool CPU::labelAddress(const std::string& label, Word& address) {
	auto it = labelToAddr.find(label);
	if (it == labelToAddr.end()) 
		return false;
	address = labelToAddr[label];
	return true;
}

bool CPU::parseCommandFile(const std::string& filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        fmt::print("Failed to open file '{}'", filename);
        return false;
    }

    std::string line;
    while(std::getline(file, line)) {
        if(line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        std::istringstream iss(line);
        std::string command, hexValue, label;
        iss >> command >> hexValue >> label;

        if(command != "label" || !isHexNumber(hexValue) || label.empty()) {
            std::cerr << "Error: Invalid syntax." << std::endl;
            return false;
        }

        Word address;
        try {
            address = std::stoul(hexValue, nullptr, 16);
        } catch(const std::exception& e) {
            fmt::print("Error: Invalid address\n");
			return false;
        }

        addLabel(address, label);
    }
    
    return true;
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
		{ "loadcmd",      "L",  &CPU::loadcmdCmd, true,
		  "'load a command file <file>"
		},
		{ "run",   "r",  &CPU::runCmd, false,
		  "Run program at current Program Counter.  Optionally "
		  "run for [x] instructions then return to debugger"
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
		  "Reset where the 'list' command starts to disassemble"
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
		{ "label",     "",   &CPU::labelCmd, false, 
		  "Add, remove or show current address label map"
		},
		{ "map",       "M",  &CPU::memmapCmd, false,
		  "Display the current memory map"
		},
		{ "clock",     "",   &CPU::clockCmd, false,
		  "Toggle CPU speed emulation"
		},
		{ "find",      "f",  &CPU::findCmd, false, 
		  "Find a string sequence in memory, with optional filter"
		},
		{ "quit",      "q",   &CPU::quitCmd, false, 
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

int CPU::listCmd(std::string &line, [[maybe_unused]] uint64_t &returnValue) {

	lookupAddress(line, listPC);
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

int CPU::loadcmdCmd(std::string &line, [[maybe_unused]] uint64_t &returnValue) {
	std::string fname;

	std::istringstream iss(line);
	iss >> fname;
	
	fmt::print("Loading command file {}\n", fname);
	
	if (parseCommandFile(fname) == false) 
		fmt::print("Command file failed\n");
	
	
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

	if (remove && line == "*") {
		deleteAllBreakpoints();
		return ACTION_CONTINUE;
	}
	
	if (lookupAddress(line, addr)) {	
		if (remove) 
			deleteBreakpoint(addr);
		else
			addBreakpoint(addr);
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
	fmt::print("Processor status auto-display ");
	if (debug_alwaysShowPS)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	
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

	fmt::print("List reset to PC {:04x}\n", listPC);
	
	return ACTION_CONTINUE;
}


int CPU::memdumpCmd(std::string &line,
		    [[maybe_unused]] uint64_t &returnValue) {
	std::regex pattern1(R"((\w+))");	
    std::regex pattern2(R"((\w+)=([0-9a-fA-F]+))");
    std::regex pattern3(R"((\w+):(\w+))");
    std::regex pattern4(R"((\w+):(\w+):([0-9a-fA-F]+))");
    std::regex pattern5(R"((\w+):(\w+)=([0-9a-fA-F]+))");
    std::smatch matches;
	Word addr1, addr2, value;	

	auto rangeCheckAddr = [](Word a) {
		return a <= MAX_MEM;
	};

	auto rangeCheckValue =[](Word v) {
		return v <= 0xff;
	};
		
    if (std::regex_match(line, matches, pattern1) && matches.size() == 2) {
		if (lookupAddress(matches[1], addr1) && rangeCheckAddr(addr1)) {
			fmt::print("[{:04x}] {:02x}\n", addr1, mem.Read(addr1));
			return ACTION_CONTINUE;
		}
    } else if (std::regex_match(line, matches, pattern2) && matches.size() == 3) {
		value = std::stoul(matches[2], nullptr, 16);
		if (lookupAddress(matches[1], addr1) && rangeCheckAddr(addr1) && rangeCheckValue(value)) {
			Byte oldval = mem.Read(addr1);
			mem.Write(addr1, (Byte) value);
			fmt::print("[{:04x}] {:02x} -> {:02x}\n", addr1, oldval, (Byte) value);
			return ACTION_CONTINUE;
		}
    } else if (std::regex_match(line, matches, pattern3) && matches.size() == 3) {
		if (lookupAddress(matches[1], addr1) && lookupAddress(matches[2], addr2) && 
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2)) {
			mem.hexdump(addr1, addr2);
			return ACTION_CONTINUE;;
		}
    } else if (std::regex_match(line, matches, pattern4) && matches.size() == 4) {
        value = std::stoul(matches[3], nullptr, 16);
		if (lookupAddress(matches[1], addr1) && lookupAddress(matches[2], addr2) && 
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2) && rangeCheckValue(value)) {
			mem.hexdump(addr1, addr2, value);
			return ACTION_CONTINUE;
		}
    } else if (std::regex_match(line, matches, pattern5) && matches.size() == 4) {
        value = std::stoul(matches[3], nullptr, 16);
		if (lookupAddress(matches[1], addr1) && lookupAddress(matches[2], addr2) &&
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2) && rangeCheckValue(value)) {
				mem.assign(addr1, addr2, (Byte) value);
			return ACTION_CONTINUE;
		}
	} 

	fmt::print("Parse error: '{}'\n", line);
	return ACTION_CONTINUE;
}
#if 0
	Word addr1, addr2;
	Word value;		
	char equal, colon;
	std::string s1, s2;
	
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
	iss >> s1;
	if (!iss.fail() && iss.eof() && lookupAddress(s1, addr1) && rangeCheckAddr(addr1)) {
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
		 fmt::print("[{:04x}] {:02x} -> {:02x}\n",
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

	// xxxx:yyyy:ff
	iss.seekg(0);
	iss >> std::hex >> addr1 >> colon >> std::hex >> addr2 >> colon
	    >> std::hex >> value;  // In this case, value is a filter for memdump()
	if (!iss.fail() && iss.eof() && colon == ':' &&
	    rangeCheckAddr(addr1) && rangeCheckAddr(addr2) &&
	    rangeCheckValue(value)) {
			mem.hexdump(addr1, addr2, value);
			return ACTION_CONTINUE;
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
#endif
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
		fmt::print("No register or status flag '{}'\n", reg);

	return ACTION_CONTINUE;
}

int CPU::resetCmd([[maybe_unused]] std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {

	fmt::print("Resetting 6502\n");
	setDebug(false);
	Reset();    // Enter reset
	if (inReset())
		Reset();    // Exit reset
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
	fmt::print("Loop detection ");
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

int CPU::labelCmd(std::string &line,
		  [[maybe_unused]] uint64_t &returnValue) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		showLabels();
		return ACTION_CONTINUE;
	}
	
	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	}

	try {
		size_t index = 0;
		addr = (Word) std::stoul(line, &index, 16);
		
		if (addr > MAX_MEM) {
			fmt::print("Error: Watchpoint address outside of "
				   "available address range\n");
			return ACTION_CONTINUE;
		}
		if (remove) {
			removeLabel(addr);
			fmt::print("Label for address {:04x} removed\n", addr);
		} else {
			if (line[index] != ' ') {
				throw std::invalid_argument("Invalid number");
			}
			auto label = line.substr(index);
			label = stripLeadingSpaces(label);
			label = stripTrailingSpaces(label);
		
			addLabel(addr, label);
			fmt::print("Label '{}' added for memory address {:04x}\n", label, addr);
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

int CPU::findCmd(std::string& line, [[maybe_unused]] uint64_t& returnValue) {
	line = stripLeadingSpaces(line);
	auto sequence = split(line, " ");
	if (sequence.empty()) {
		fmt::print("Error: no search sequence provided\n");
		return ACTION_CONTINUE;
	}
	line = stripSpaces(line);
	uint8_t filter = 0xff;

	if (!line.empty()) {
		try {
			size_t cProcessed;
			auto len = line.length();
			filter = std::stoul(line, &cProcessed, 16);
			if (cProcessed != len) {
				fmt::print("Error: filter is not a hexadecimal number\n");
				return ACTION_CONTINUE;
			}
		}
		catch(...) {
			fmt::print("Error: filter is not a hexadecimal number\n");
			return ACTION_CONTINUE;
		}
	} 

	auto locations = mem.find(sequence, filter);
	if (locations.empty()) {
		fmt::print("Sequence not found\n");
		return ACTION_CONTINUE;
	}
	fmt::print("Sequence found at addresses:\n");
	for (const auto& addr : locations) 
		fmt::print(" {:04x}\n", addr);

	return ACTION_CONTINUE;
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
		if (debug_alwaysShowPS) 
			printCPUState();

		getReadline(line);
		line = stripTrailingSpaces(line);
		line = stripLeadingSpaces(line);

		if (line.empty() && debug_lastCmd.empty()) // Blank input
			continue;

		if (line.empty() && !debug_lastCmd.empty()) {
			line = debug_lastCmd;
			fmt::print(": {}\n", line);
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

		auto savedLine = line;
		auto command = split(line, " ");
		if (command == "") 
			continue;

		if (matchCommand(command, f) == false) {
			fmt::print("Unknown command '{}'\n", command);
			continue;
		}

		if (line != "continue") 
			debug_lastCmd = savedLine;

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

	if (debugEntryFunc) {
		captureSignals();
		debugEntryFunc();
	}

	fmt::print("\nDebugger starting at PC {:#04x}\n", PC);

	debugMode = true;

	while (debugMode) {
		count = debugPrompt();

		while (count--) {
			executeOneInstruction();
			if (count)
				disassemble(PC, 1);
		}
	}

	fmt::print("Exiting debugger\n");

	if (debugExitFunc) {
		restoreSignals();
		debugExitFunc();
	}
}

std::tuple<uint64_t, uint64_t> CPU::traceOneInstruction() {
	disassemble(PC, 1);
	return executeOneInstruction();
}
