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
#include <list>
#include <cstdarg>
#include <regex>

#include <fmt/format.h>
#include <fmt/core.h>

#include <stdio.h>
#if defined(__linux__) || defined(__MACH__)
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#endif

#include "6502.h"

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

bool isHexDigit(const char ch) {
	return std::isxdigit(ch) || ch == 'x' || ch == 'X';
}

bool isHexNumber(const std::string& str) {
    for(char ch : str) {
        if(!isHexDigit(ch))
            return false;
    }
    return true;
}

bool MOS6502::lookupAddress(const std::string& line, Word& address) {
	if (line.empty()) 
		return false;

	if (labelAddress(line, address)) 
		return true;
	
	if (isHexNumber(line)) {
		try {
			address = (Word) std::stoul(line, nullptr, 16);
			return true;
		}
		catch(...) {
			fmt::print("Parse error: Invalid hexadecimal number: {}\n", line);
			return false;
		}
	}

	fmt::print("Address provided is neither a hexadecimal number nor a label: {}\n", line);
	return false;
}

#if defined(__linux__) || defined(__MACH__)
//////////
// readline helpers
void getReadline(std::string &line) {
	char *c_line = readline(": ");
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
    auto commands = MOS6502::setupDebugCommands();
   
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
	auto commands = MOS6502::setupDebugCommands();

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
// Called by MOS6502::CPU() to initialize the debugger
void MOS6502::initDebugger() {
	setupReadline();
	deleteAllBreakpoints();
}

//////////
// CPU State information 

void MOS6502::printCPUState() {
	auto yesno = [](bool b) -> std::string {
		return b ? "Yes" : "No";
	};
	auto fl = [](char c, bool b) -> char {
		return b ? std::toupper(c) : std::tolower(c);
	};

	fmt::print("  | PC: {:04x} SP: {:02x}\n", PC, SP );
	// fmt::print() doesn't like to print out union/bit-field members?
	fmt::print("  | Flags: {}{}{}{}{}{}{} (PS: {:#x})\n",
		fl('C', Flags.C), fl('Z', Flags.Z), fl('I', Flags.I), fl('D', Flags.D),
		fl('B', Flags.B), fl('V', Flags.V), fl('N', Flags.N), PS);
	fmt::print("  | A: {:02x} X: {:02x} Y: {:02x}\n", A, X, Y );
	fmt::print("  | Pending: IRQ - {}, NMI - {}, Reset - {}, "
			   "PendingReset - {}\n",
		   yesno(pendingIRQ()), yesno(pendingNMI()),
		   yesno(_inReset), yesno(_pendingReset));
	fmt::print("  | IRQs: {}, NMIs: {}, BRKs: {}\n",
		    _IRQCount, _NMICount, _BRKCount);
	fmt::print("\n");
}

void MOS6502::dumpStack() {
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

void MOS6502::decodeArgs(bool atPC, Byte ins, std::string& disassembly, 
					 std::string& opcodes, std::string& address, 
					 std::string& computedAddr) {

	auto mode = _instructions.at(ins).addrmode;
	Byte byteval;
	Word wordval;
	Byte rel;
	std::string out, addr, label;

	switch (mode) {
	case AddressingMode::Implied:
		break;

	case AddressingMode::Accumulator:
		disassembly = "A";
		address = "";
		break;

	case AddressingMode::Immediate:  // #$xx
		byteval = readByteAtPC();
		disassembly = fmt::format("#${:02x}", byteval);
		opcodes += fmt::format("{:02x} ", byteval);
		address = "";
		break;

	case AddressingMode::ZeroPage:  // $xx
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:02x}", byteval);
	
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		opcodes += fmt::format("{:02x} ", byteval);
		break;

	case AddressingMode::ZeroPageX:  // $xx,X
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:04x}", byteval);

		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",X";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + X);
		break;

	case AddressingMode::ZeroPageY:  // $xx,Y
		byteval = readByteAtPC();
		label = addressLabelSearch(byteval);
		addr = fmt::format("${:04x}", byteval);
		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else {
			disassembly = addr;
			address = "";
		}
		disassembly += ",Y";
		opcodes += fmt::format("{:02x} ", byteval);
		if (atPC) 
			computedAddr = fmt::format("${:04x}", byteval + Y);
		break;

	case AddressingMode::Relative:
		rel = readByteAtPC();
		wordval = PC + SByte(rel);
		addr = fmt::format("${:04x}", wordval);
		label = addressLabel(wordval);

		if (!label.empty()) {
			disassembly = label;
			address = addr;
		} else { 
			disassembly = fmt::format("#${:02x}", rel);
			address = addr;
		} 
		opcodes += fmt::format("{:02x} ", rel);
		break;

	case AddressingMode::Absolute:  // $xxxx
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
		opcodes += fmt::format("{:02x} {:02x}", wordval & 0xff, (wordval >> 8) & 0xff);
		break;

	case AddressingMode::AbsoluteX:  // $xxxx,X
		wordval = readWordAtPC();
		label = addressLabelSearch(wordval);
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

	case AddressingMode::AbsoluteY:  // $xxxx,Y
		wordval = readWordAtPC();
		label = addressLabelSearch(wordval);
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
		
	case AddressingMode::Indirect:  // $(xxxx)
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

	case AddressingMode::IndirectX: // ($xx,X)
		byteval = readByteAtPC();
		wordval = byteval + X;
		if (wordval > 0xFF)
			wordval -= 0xFF;
		wordval = readWord(wordval);
		
		label = addressLabelSearch(byteval);
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
		
	case AddressingMode::IndirectY:  // ($xx),Y
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
		disassembly += fmt::format("[Invalid addressing mode]");
	}
}

Address_t MOS6502::disassembleAt(Address_t dPC, std::string& disassembly) {
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
		
	disassembly = fmt::format("{:1.1}{:1.1}| {:20.20} | {:9.9}| {:<7}", 
				              marker, bkpoint, addr, opcodes, ins);

	if (!bytes) 
		disassembly += fmt::format("{:<20} | {:<5.5} | {}", args, address, computedAddress);
	dPC = PC;
	PC = savePC;
	Cycles = saveCycles;

	return dPC;
}

Address_t MOS6502::disassemble(Address_t dPC, uint64_t cnt) {
	std::string disassembly;

	if (dPC > MAX_MEM) {
		fmt::print("PC at end of memory");
		return dPC;
	}

	do {
		dPC = disassembleAt(dPC, disassembly);
		fmt::print("{}\n", disassembly);
	} while (--cnt && dPC < MAX_MEM);
	
	return dPC;
}

//////////
// Breakpoints

void MOS6502::listBreakpoints() {
	fmt::print("Active breakpoints:\n");
	for (const auto& address : breakpoints) {
		fmt::print("{:04x}", address);
		auto label = addressLabel(address);
		if (!label.empty()) 
			fmt::print(": {}", label);
		fmt::print("\n");
	}
}

bool MOS6502::isPCBreakpoint() { 
	return isBreakpoint(PC); 
}

bool MOS6502::isBreakpoint(Word bp) {
	if (bp > MAX_MEM) 
		return false;
	return breakpoints.find(bp) != breakpoints.end();
}

void MOS6502::deleteBreakpoint(Word bp) {
	if (bp > MAX_MEM) 
		return;
	
	if (breakpoints.erase(bp) == 0) {
		fmt::print("No breakpoint at {:04x}\n", bp);
		return;
	}
	
	fmt::print("Removed breakpoint at {:04x}", bp);
	auto label = addressLabel(bp);
	if (!label.empty()) 
		fmt::print(": {}", label);
	fmt::print("\n");
}

void MOS6502::addBreakpoint(Word bp) {
	if (bp > MAX_MEM) {
		fmt::print("Error: Breakpoint address outside of available "
			   "address range\n");
		return;
	}
	if (isBreakpoint(bp)) {
		fmt::print("Breakpoint already set at {:04x}\n", bp);
		return;
	}
	breakpoints.insert(bp);

	fmt::print("Set breakpoint at {:04x}", bp);
	auto label = addressLabel(bp);
	if (!label.empty()) 
		fmt::print(": {}", label);
	fmt::print("\n");
}

void MOS6502::deleteAllBreakpoints() { 
	breakpoints.clear();
}

//////////
// Backtrace

void MOS6502::showBacktrace() {
	std::vector<std::string>::iterator i = backtrace.begin();
	unsigned int cnt = 0;

	fmt::print("Backtrace: {} entries\n", backtrace.size());
	for ( ; i < backtrace.end(); i++ )
		fmt::print("#{:02d}:  {}\n", cnt++, (*i).c_str());
}

void MOS6502::addBacktrace(Word backtracePC) {
	std::string ins;
	disassembleAt(backtracePC, ins);
	backtrace.push_back(ins);
}

void MOS6502::addBacktraceInterrupt(Word backtracePC) {
	std::string ins;
	disassembleAt(backtracePC, ins);
	ins += " [IRQ/NMI]";
	backtrace.push_back(ins);
}

void MOS6502::removeBacktrace() {
	if (!backtrace.empty())
		backtrace.pop_back();
}

//////////
// Labels

void MOS6502::showLabels() {
	if (addrToLabel.empty()) {
		fmt::print("No labels\n");
		return;
	}

	fmt::print("Address labels:\n");
	for (const auto& [address, label] : addrToLabel) {
		fmt::print("{:#06x}: {}\n", (Word) address, label);
	}
}

void MOS6502::addLabel(const Word address, const std::string label) {
	addrToLabel[address] = label;
	labelToAddr[label]   = address;
}

void MOS6502::removeLabel(const Word address) {
	auto it = addrToLabel.find(address);
	if (it == addrToLabel.end())
		return;

	auto label = addrToLabel.at(address);
	addrToLabel.erase(address);
	labelToAddr.erase(label);
}

std::string MOS6502::addressLabel(const Word address) {
	std::string label;
	
	auto it = addrToLabel.find(address);
	if (it != addrToLabel.end()) 
		label += fmt::format("{}", it->second);
	
	return label;
}

std::unordered_map<Word, std::string> addrToLabel;
std::list<Word> recentAddresses;  // Cache for recent addresses
const size_t cacheSize = 10;      // Adjust the cache size as needed
std::string MOS6502::addressLabelSearch(const Word address, const int8_t searchWidth) {
	std::string label;

    label = addressLabel(address);
    if (!label.empty())  {
		recentAddresses.remove(address);
        recentAddresses.push_back(address);
        return label;
    }

    int16_t offset;
    bool found = false;
    auto it = recentAddresses.rbegin(); 
  	for (offset = 1; offset <= searchWidth && !found; ++offset) {
    	it = std::find_if(recentAddresses.rbegin(), recentAddresses.rend(),
        	[&](Word addr) { return addr == address + offset || addr == address - offset; });
       	if (it != recentAddresses.rend()) {
			found = true;
			break;
        }
	}

    if (found) {
        label = fmt::format("{}{:+}", addressLabel(*it), offset);
		recentAddresses.remove(*it);
        recentAddresses.push_back(*it);
    } else {
        label = fmt::format("{:04x}", address);
    }

    // Ensure the cache does not exceed its size
    if (recentAddresses.size() >= cacheSize) {
        recentAddresses.pop_front();
    }

    return label;
}

bool MOS6502::labelAddress(const std::string& label, Word& address) {
	auto it = labelToAddr.find(label);
	if (it == labelToAddr.end()) 
		return false;
	address = labelToAddr[label];
	return true;
}

//////////
// Load and save hex files

bool MOS6502::loadHexFile(const std::string& filename) {
	std::ifstream inFile(filename);
	if (!inFile) {
		fmt::print("Error opening file: {}", filename);
		return false;
	}

	std::string line;
	while (std::getline(inFile, line)) {
		std::istringstream lineStream(line);
		Word address;
		char colon;
		lineStream >> std::hex >> address >> colon; // Read the address.
		
		uint64_t element;
		while (lineStream >> std::hex >> element) {
			mem[address] = static_cast<Byte>(element);
			address++;
		}
	}

	return true;
}

bool MOS6502::saveToHexFile(const std::string& filename, const std::vector<std::pair<Word, Word>>& addressRanges) {
	std::ofstream outFile(filename);
	if (!outFile) {
		fmt::print("Error opening file: {}", filename);
		return false;
	}

	for (const auto& range : addressRanges) {
		Word startAddress = range.first;
		Word endAddress = range.second;

		if (startAddress > endAddress) {
			fmt::print("Invalid address range: {:04x} to {:04x}", 
				startAddress, endAddress);
			return false;
		}

		std::string out;
		for (Word i = startAddress; i <= endAddress; i += 16) {
			out = fmt::format("{:0>4X}: ", i);
			for (Word j = 0; j < 16 && (i + j) <= endAddress; ++j) {
				Byte element = mem[i+j];
				out += fmt::format("{:02X} ", element);
			}
			outFile << out << '\n';
		}
	}

	return true;
}

bool MOS6502::saveToHexFile(const std::string& filename, Word startAddress, Word endAddress) {
	std::vector<std::pair<Word, Word>> range = {{startAddress, endAddress}};
	return saveToHexFile(filename, range);
}

//////////
// command file

bool MOS6502::parseCommandFile(const std::string& filename) {
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

		if (!executeDebuggerCmd(line))
			return false;	
    }
    
    return true;
}

//////////
// Debugger
std::vector<MOS6502::debugCommand> MOS6502::setupDebugCommands() {
	return {
		{ "help",      "h",  &MOS6502::helpCmd, false,
		  "This help message" 
		},
		{ "list",      "l",  &MOS6502::listCmd, false,
		  "List next 10 instructions.  'list xxxx' lists from address "
		  "xxxx. 'list' without an address either lists from current "
		  "program counter or continues the last listing."
		},
		{ "load",      "L",  &MOS6502::loadCmd, true,
		  "'load <file> <address>' loads the file named 'file' at "
		  "memory address 'address', overwriting any data.  This "
		  "command will fail if it attempts to load data on non-RAM "
		  "memory."
		},
		{ "script",      "",  &MOS6502::loadScriptCmd, true,
		  "'load a command/script from file <file>"
		},
		{ "loadhex",      "",  &MOS6502::loadhexCmd, true,
		  "'load a kex file <file>"
		},
		{ "stack",     "S",  &MOS6502::stackCmd, false,
		  "Show current stack elements"
		},
		{ "break",     "b",  &MOS6502::breakpointCmd, false,
		  "Add, remove or show current breakpoints.  'break  "
		  "xxxx' adds a breakpoint at address xxxx, 'break "
		  "-xxxx' removes the breakpoint at address xxxx, and "
		  "'break' alone will list active breakpoints"
		},
		{ "save",      "",   &MOS6502::savememCmd, false, 
		  "Save memory in Wozmon format" 
		},
		{ "state",     "p",  &MOS6502::cpustateCmd, false,
		  "Show current CPU state"},
		{ "autostate", "a",  &MOS6502::autostateCmd, false,
		  "Display CPU state after every debugger command"
		},
		{ "listpc",   "P",  &MOS6502::resetListPCCmd, false,
		  "Reset where the 'list' command starts to disassemble"
		},
		{ "mem",       "m",  &MOS6502::memdumpCmd, false,
		  "Examine or change memory"},
		{ "set",       "s",  &MOS6502::setCmd, false,
		  "set a register or CPU flag, (ex. 'set A=ff')"},
		{ "reset",     "" ,  &MOS6502::resetCmd, false,
		  "Reset the CPU and jump through the reset vector"
		},
		{ "continue",  "c",  &MOS6502::continueCmd, false,
		  "Exit the debugger and continue running the CPU.  "
		},
		{ "loopdetect","ld", &MOS6502::loopdetectCmd, false,
		  "Enable or disable loop detection (ie, 'jmp *'"},
		{ "backtrace", "t",  &MOS6502::backtraceCmd, false,
		  "Show the current subroutine and break backtrace"
		},
		{ "where",     "w",  &MOS6502::whereCmd, false,
		  "Display the instruction at the Program Counter"
		},
		{ "watch",     "W",  &MOS6502::watchCmd, false,
		  "Add, remove or show current memory watchpoints. "
		  "'watch xxxx' adds a watchpoint at memory address "
		  "xxxx, 'watch -xxxx' removes the watchpoint at "
		  "memory address xxxx, and 'watch' alone will list "
		  "active watchpoints"
		},
		{ "label",     "",   &MOS6502::labelCmd, false, 
		  "Add, remove or show current address label map"
		},
		{ "map",       "M",  &MOS6502::memmapCmd, false,
		  "Display the current memory map"
		},
		{ "find",      "f",  &MOS6502::findCmd, false, 
		  "Find a string sequence in memory, with optional filter"
		},
		{ "quit",      "q",   &MOS6502::quitCmd, false, 
		  "Quit the emulator"
		}
	};
}

bool MOS6502::helpCmd([[maybe_unused]] std::string &line) {
	for (const auto& cmd : _debugCommands) {
		fmt::print("{:<10}: {}\n", cmd.command,
			   // Add ': '
			   wrapText(cmd.helpMsg, 80 - (10 + 2), 10+2)); 
	}
	return true;
}

bool MOS6502::listCmd(std::string &line) {

	if (!lookupAddress(line, listPC) && !line.empty()) {
		return false;
	};
	listPC = disassemble(listPC, 10);
	return true;
}

bool MOS6502::loadCmd(std::string &line) {
	std::string fname;
	Address_t address;

	std::istringstream iss(line);
	iss >> fname >> std::hex >> address;

	if (address > MOS6502::MAX_MEM) {
		fmt::print("Invalid address: {:04x}\n", address);
		return false;
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
	
	return false;
}

bool MOS6502::loadScriptCmd(std::string &line) {
	std::string fname;

	std::istringstream iss(line);
	iss >> fname;
	
	fmt::print("Loading command file {}\n", fname);
	
	auto r = parseCommandFile(fname);
	if (r == false) 
		fmt::print("Command file failed\n");
	return r;
}

bool MOS6502::loadhexCmd([[maybe_unused]] std::string &line) {
	line = stripLeadingSpaces(line);
	line = stripTrailingSpaces(line);

	return loadHexFile(line);
}

bool MOS6502::savememCmd([[maybe_unused]] std::string &line) {
	std::vector<std::pair<Word, Word>> ranges;
	std::string addressRanges, filename;
	
	auto splitInput = [&addressRanges, &filename](const std::string& input) -> bool {
		std::regex pattern(R"(^([\da-fA-F]{4}:[\da-fA-F]{4}(?:,[\da-fA-F]{4}:[\da-fA-F]{4})*?)\s+([^\s]+)$)");
		std::smatch match;

		if (std::regex_match(input, match, pattern)) {
			addressRanges = match[1].str();
			filename = match[2].str();
			return true;
		}
		return false;
	};

	auto parseAddressRange = [](const std::string& r) -> std::vector<std::pair<Word, Word>> {
    	std::vector<std::pair<Word, Word>> result;
        std::regex pattern(R"([\da-fA-F]{4}:[\da-fA-F]{4})");
        std::smatch match;
        for (std::sregex_iterator it(r.begin(), r.end(), pattern); it != std::sregex_iterator(); ++it) {
        	match = *it;
            std::istringstream iss(match.str());
      	  	Word start, end;
        	char colon;
        	if ((iss >> std::hex >> start >> colon >> end) && colon == ':') {
           		result.push_back({start, end});
        	} else {
				return {};
			}
        }
        return result;
    };

	if (!splitInput(line)) {
		fmt::print("Parse error\n");
		return false;
	}
		
	ranges = parseAddressRange(line);
	if (ranges.empty()) {
		fmt::print("Parse error\n");
		return false;
	}

	return saveToHexFile(filename, ranges);
}

bool MOS6502::stackCmd([[maybe_unused]] std::string &line) {
	dumpStack();
	return true;
}

bool MOS6502::breakpointCmd(std::string &line) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		listBreakpoints();
		return true;
	}

	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	} 

	if (remove && line == "*") {
		deleteAllBreakpoints();
		return true;
	}
	
	if (!lookupAddress(line, addr)) {
		return false;
	}
	
	if (remove) 
		deleteBreakpoint(addr);
	else
		addBreakpoint(addr);
	return true;
}

bool MOS6502::cpustateCmd([[maybe_unused]] std::string &line) {
	printCPUState();
	return true;
}

bool MOS6502::autostateCmd([[maybe_unused]] std::string &line) {
	debug_alwaysShowPS = !debug_alwaysShowPS;
	fmt::print("Processor status auto-display ");
	if (debug_alwaysShowPS)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	return true;
}

bool MOS6502::resetListPCCmd(std::string &line) {
	Word i;

	try {
		i = (Word) std::stoul(line, nullptr, 16);
		if (i > MAX_MEM) {
			fmt::print("Error: Program Counter address outside of "
				   "available address range\n");
			return false;
		}
		listPC = i;
	}
	catch (...) {
		listPC = PC;
	}

	fmt::print("List reset to PC {:04x}\n", listPC);
	return true;
}

bool MOS6502::memdumpCmd(std::string &line) {
	const std::string wordPattern   = R"((\w+))";  				// A word, like a label or identifier
	const std::string offsetPattern = R"(([+-][0-9a-fA-F]+)?)"; // Optional offset, positive or negative, in hexadecimal
	const std::string valuePattern  = R"(([0-9a-fA-F]+))";  	// A value, in hexadecimal
	const std::string expressionPattern = R"((:?([+\-&|^%/*]?\w+)+))";
	const std::string assignmentPattern = wordPattern + offsetPattern + "=" + valuePattern;  
	const std::string rangePattern = wordPattern + offsetPattern + ":" + wordPattern + offsetPattern; 

    std::regex labelWithOptionalOffsetR(wordPattern + offsetPattern);       // xxxx[+/-offset]
	std::regex assignValueToLabelR     (assignmentPattern); 				// xxxx[+/-offset]=zz 
	std::regex rangeBetweenLabelsR     (rangePattern);                      // xxxx[+/-offset]:yyyy[+/-offset]
	std::regex filterRangeWithValueR   (rangePattern + ":" + expressionPattern); // xxxx[+/-offset]:yyyy[+/-offset]:zz
	std::regex assignValueToRangeR     (rangePattern + "=" + valuePattern); // xxxx[+/-offset]:yyyy[+/-offset]=zz

	std::smatch matches;
	Word addr1, addr2, value;	

	auto rangeCheckAddr = [](Word a) {
		return a <= MAX_MEM;
	};

	auto rangeCheckValue =[](Word v) {
		return v <= 0xff;
	};

	auto calculateAddress = [this](const std::smatch& m, Word& address1, Word& address2, bool isRange) {
        if (!lookupAddress(m[1].str(), address1)) 
			return false;
			
		if (m[2].matched) {
			int offset = std::stoi(m[2].str(), nullptr, 16); 
			address1 += offset; 
		}

		if (isRange) {
			if (!lookupAddress(m[3].str(), address2)) 
				return false;
			if (m[4].matched) {
				int offset = std::stoi(m[4].str(), nullptr, 16);
				address2 += offset; 
			}
		}
		
		return true;
	};

    if (std::regex_match(line, matches, labelWithOptionalOffsetR) && matches.size() > 1) {
		if (calculateAddress(matches, addr1, addr2, false) && rangeCheckAddr(addr1)) {
			fmt::print("[{:04x}] {:02x}\n", addr1, mem.Read(addr1));
			return true;
		}
    } else if (std::regex_match(line, matches, assignValueToLabelR) && matches.size() > 3) {
		value = (Word) std::stoul(matches[3], nullptr, 16);
		if (calculateAddress(matches, addr1, addr2, false) && rangeCheckAddr(addr1) && rangeCheckValue(value)) {
			Byte oldval = mem.Read(addr1);
			mem.Write(addr1, (Byte) value);
			fmt::print("[{:04x}] {:02x} -> {:02x}\n", addr1, oldval, (Byte) value);
			return true;
		}
    } else if (std::regex_match(line, matches, rangeBetweenLabelsR) && matches.size() > 2) {
		if (calculateAddress(matches, addr1, addr2, true) && 
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2)) {
			mem.hexdump(addr1, addr2);
			return true;
		}
    } else if (std::regex_match(line, matches, filterRangeWithValueR) && matches.size() > 4) {
        auto valueExpression = matches[5].str();
		if (calculateAddress(matches, addr1, addr2, true) && rangeCheckAddr(addr1) && 
			rangeCheckAddr(addr2)) {
			mem.hexdump(addr1, addr2, valueExpression);
			return true;
		}
    } else if (std::regex_match(line, matches, assignValueToRangeR) && matches.size() > 4) {
        value = (Word) std::stoul(matches[5], nullptr, 16);
		if (calculateAddress(matches, addr1, addr2, true) &&
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2) && rangeCheckValue(value)) {
				mem.assign(addr1, addr2, (Byte) value);
			return true;
		}
	} 

	fmt::print("Parse error: '{}'\n", line);
	return false;
}

bool MOS6502::memmapCmd([[maybe_unused]] std::string &line) {
	mem.printMap();
	return true;
}

bool MOS6502::setCmd(std::string &line) {
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
		return false;
	}

	// reg contains the register, s is the value.
	std::transform(reg.begin(), reg.end(), reg.begin(), ::toupper);
	try {
		value = std::stoul(v, nullptr, 16);
		if ((reg != "PC" && value > 0xff) ||
		    (reg == "PC" && value > 0xffff)) {
			fmt::print("Error: value would overflow register {}\n",
				   reg);
			return false;
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
			return false;
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
	else {
		fmt::print("No register or status flag '{}'\n", reg);
		return false;
	}

	return true;
}

bool MOS6502::resetCmd([[maybe_unused]] std::string &line) {
	fmt::print("Resetting 6502\n");
	Reset();    	// Enter reset
	if (inReset())
		Reset();    // Exit reset
	return true;
}
		
bool MOS6502::continueCmd([[maybe_unused]] std::string &line) {
	if (_hitException) {
		fmt::print("CPU Exception hit; can't continue.  Reset CPU to clear.\n");
		return false;
	}
	_debugMode = false;
	return true;
}

bool MOS6502::loopdetectCmd([[maybe_unused]] std::string &line) {
	debug_loopDetection = !debug_loopDetection;
	fmt::print("Loop detection ");
	if (debug_loopDetection)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	return true;
}

bool MOS6502::backtraceCmd([[maybe_unused]] std::string &line) {
	showBacktrace();
	return true;
}

bool MOS6502::whereCmd([[maybe_unused]] std::string &line) {
	disassemble(PC, 1);
	return true;
}

bool MOS6502::watchCmd(std::string &line) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		mem.listWatch();
		return true;
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
			return false;
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
		return false;
	}

	return true;
}

bool MOS6502::labelCmd(std::string &line) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		showLabels();
		return true;
	}
	
	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	}

	// Handle '-0xf00f' or '-label'
	if (remove) {
		line = stripSpaces(line);
		if (lookupAddress(line, addr)) {
			removeLabel(addr);
			fmt::print("Label for address {:04x} removed\n", addr);
			return true;
		}
	}

	try {
		size_t index = 0;
		addr = (Word) std::stoul(line, &index, 16);
		
		if (addr > MAX_MEM) {
			fmt::print("Error: Label address outside of available address range\n");
			return false;
		}
		if (line[index] != ' ') {
			throw std::invalid_argument("Invalid number");
		}
		auto label = line.substr(index);
		label = stripLeadingSpaces(label);
		label = stripTrailingSpaces(label);
	
		addLabel(addr, label);
		fmt::print("Label '{}' added for memory address {:04x}\n", label, addr);
	}
	catch(...) {
		fmt::print("Parse error: {}\n", line);
		return false;
	}

	return true;
}

bool MOS6502::quitCmd([[maybe_unused]] std::string& line) {
	fmt::print("Exiting emulator\n");
	exit(0);
	return true;
}

bool MOS6502::findCmd(std::string& line) {
	line = stripLeadingSpaces(line);
	auto sequence = split(line, " ");
	if (sequence.empty()) {
		fmt::print("Error: no search sequence provided\n");
		return false;
	}
	line = stripSpaces(line);
	uint8_t filter = 0xff;

	if (!line.empty()) {
		try {
			size_t cProcessed;
			auto len = line.length();
			filter = (uint8_t) std::stoul(line, &cProcessed, 16);
			if (cProcessed != len) {
				fmt::print("Error: filter is not a hexadecimal number\n");
				return false;
			}
		}
		catch(...) {
			fmt::print("Error: filter is not a hexadecimal number\n");
			return false;
		}
	} 

	auto locations = mem.find(sequence, filter);
	if (locations.empty()) {
		fmt::print("Sequence not found\n");
		return true;
	}

	fmt::print("Sequence found at addresses:\n");
	for (const auto& addr : locations) 
		fmt::print(" {:04x}\n", addr);

	return true;
}

bool MOS6502::matchCommand(const std::string &input, debugFn_t &func) {

	for (const auto &cmd : _debugCommands) {
		if (std::strcmp(cmd.command, input.c_str()) == 0 ||
		    std::strcmp(cmd.shortcut, input.c_str()) == 0) {
			func = cmd.func;
			return true;
		}
	}
	return false;
}

bool MOS6502::executeDebuggerCmd(std::string line) {
	debugFn_t f;
	
	line = stripTrailingSpaces(line);
	line = stripLeadingSpaces(line);

	if (line.empty() && debug_lastCmd.empty()) // Blank input
		return true;

	if (line.empty() && !debug_lastCmd.empty()) {
		line = debug_lastCmd;
		fmt::print(": {}\n", line);
	}

	// Check if command is numbers, convert it to an integer and execute that many instructions.
	try {
		uint64_t insCnt = std::stol(line);
		debug_lastCmd = line;
		while (insCnt--) {
			executeOneInstruction();
			if (debug_alwaysShowPS) 
				printCPUState();
			disassemble(PC, 1);
		}
		return true;
	}
	catch(...) {
		// line isn't numbers, continue.
	}

	auto savedLine = line;
	auto command = split(line, " ");
	if (command == "") {
		fmt::print("Invalid command: ""\n");
		return false;
	}

	if (matchCommand(command, f) == false) {
		fmt::print("Unknown command '{}'\n", command);
		return false;
	}

	if (line != "continue") 
		debug_lastCmd = savedLine;

	return (this->*f)(line);
}

void MOS6502::executeDebug() {
	static bool header = false;

	if (!header) {
		listPC = PC;
		header = true;

		fmt::print("\nDebugger starting at PC {:#06x}\n", PC);
		printCPUState();
		disassemble(PC, 1);
	}

	std::string line;
	getReadline(line);
	executeDebuggerCmd(line);

	if (!_debugMode) { 
		fmt::print("Exiting debugger\n");
		header = false;
	}
}

#ifdef TEST_BUILD
// This is used for basic disassembler testing
void MOS6502::traceOneInstruction() {
	disassemble(PC, 1);
	executeOneInstruction();
}
#endif
