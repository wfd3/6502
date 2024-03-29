//
// Debugger commands 
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

#include <stdio.h>
#include <regex>

#if defined(__linux__) || defined(__MACH__)
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#endif

#include <6502.h>
#include <utils.h>

Word listPC;

//////////
// User input

#if defined(__linux__) || defined(__MACH__)
//////////
// readline helpers
void getReadline(std::string& line) {
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
    auto commands = Debugger::setupDebugCommands();
   
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
extern "C" char **readlineCompletionCallback(const char* text, [[maybe_unused]] int start, [[maybe_unused]] int end) {
	auto commands = Debugger::setupDebugCommands();

	rl_attempted_completion_over = 1;

	// See if the text so far is a command that wants file completion
	for(const auto& cmd : commands) {
		size_t cmd_len = strlen(cmd.command);
		if(start >= (int) cmd_len && strncmp(rl_line_buffer, cmd.command, cmd_len) == 0 &&
		   rl_line_buffer[cmd_len] == ' ' && cmd.doFileCompletion) {
			rl_attempted_completion_over = 0;
			return rl_completion_matches(text, rl_filename_completion_function);
		}
	}
	
	// Otherwise use custom command generator
	return rl_completion_matches(text, readlineCommandGenerator);
}

void setupReadline() {
	// Setup GNU readline	
	rl_completion_query_items = 50;
	rl_attempted_completion_function = readlineCompletionCallback;
}

void Debugger::setupConsoleInput() {
    setupReadline();
}

#endif

#ifdef _WIN64
void Debugger::setupConsoleInput() { }

void getReadline(std::string& line) {
	fmt::print(": ");
	std::getline(std::cin, line);
}
#endif

//////////
// command file

bool Debugger::parseCommandFile(const std::string& filename) {
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
std::vector<Debugger::debugCommand> Debugger::setupDebugCommands() {
	return {
		{ "help",      "h",  &Debugger::helpCmd, false,
		  "This help message" 
		}, 
		{ "list",      "l",  &Debugger::listCmd, false,
		  "List next 10 instructions.  'list xxxx' lists from address "
		  "xxxx. 'list' without an address either lists from current "
		  "program counter or continues the last listing."
		},
		{ "load",      "L",  &Debugger::loadCmd, true,
		  "'load <file> <address>' loads the file named 'file' at "
		  "memory address 'address', overwriting any data.  This "
		  "command will fail if it attempts to load data on non-RAM "
		  "memory."
		},
		{ "script",      "",  &Debugger::loadScriptCmd, true,
		  "'load a command/script from file <file>"
		},
		{ "loadhex",      "",  &Debugger::loadhexCmd, true,
		  "'load a kex file <file>"
		},
		{ "stack",     "S",  &Debugger::stackCmd, false,
		  "Show current stack elements"
		},
		{ "break",     "b",  &Debugger::breakpointCmd, false,
		  "Add, remove or show current breakpoints.  'break  "
		  "xxxx' adds a breakpoint at address xxxx, 'break "
		  "-xxxx' removes the breakpoint at address xxxx, and "
		  "'break' alone will list active breakpoints"
		},
		{ "save",      "",   &Debugger::savememCmd, false, 
		  "Save memory in Wozmon format" 
		},
		{ "state",     "p",  &Debugger::cpustateCmd, false,
		  "Show current CPU state"},
		{ "autostate", "a",  &Debugger::autostateCmd, false,
		  "Display CPU state after every debugger command"
		},
		{ "listpc",   "P",  &Debugger::resetListPCCmd, false,
		  "Reset where the 'list' command starts to disassemble"
		},
		{ "mem",       "m",  &Debugger::memdumpCmd, false,
		  "Examine or change memory"},
		{ "set",       "s",  &Debugger::setCmd, false,
		  "set a register or CPU flag, (ex. 'set A=ff')"},
		{ "reset",     "" ,  &Debugger::resetCmd, false,
		  "Reset the CPU and jump through the reset vector"
		},
		{ "continue",  "c",  &Debugger::continueCmd, false,
		  "Exit the debugger and continue running the CPU.  "
		},
		{ "loopdetect","ld", &Debugger::loopdetectCmd, false,
		  "Enable or disable loop detection (ie, 'jmp *'"},
		{ "backtrace", "t",  &Debugger::backtraceCmd, false,
		  "Show the current subroutine and break backtrace"
		},
		{ "where",     "w",  &Debugger::whereCmd, false,
		  "Display the instruction at the Program Counter"
		},
		{ "watch",     "W",  &Debugger::watchCmd, false,
		  "Add, remove or show current memory watchpoints. "
		  "'watch xxxx' adds a watchpoint at memory address "
		  "xxxx, 'watch -xxxx' removes the watchpoint at "
		  "memory address xxxx, and 'watch' alone will list "
		  "active watchpoints"
		},
		{ "label",     "",   &Debugger::labelCmd, false, 
		  "Add, remove or show current address label map"
		},
		{ "map",       "M",  &Debugger::memmapCmd, false,
		  "Display the current memory map"
		},
		{ "find",      "f",  &Debugger::findCmd, false, 
		  "Find a string sequence in memory, with optional filter"
		},
		{ "exception", "",   &Debugger::exceptionCmd, false, 
		  "Enter debugger on CPU exception"
		},
		{ "quit",      "q",  &Debugger::quitCmd, false, 
		  "Quit the emulator"
		}
	};
}

bool Debugger::exceptionCmd([[maybe_unused]] std::string& line) {
	setDebugModeOnException(!debugModeOnException());
	fmt::print("Debugger on exception: {}\n", debugModeOnException() ? "Yes" : "No");
	return true;
}

bool Debugger::helpCmd([[maybe_unused]] std::string& line) {
	for (const auto& cmd : _debugCommands) {
		//  Account for the 2 characters in ": "
		fmt::print("{:<10}: {}\n", cmd.command, wrapText(cmd.helpMsg, 80 - (10 + 2), 10+2));
	}
	return true;
}

bool Debugger::listCmd(std::string& line) {

	if (!lookupAddress(line, listPC) && !line.empty()) {
		return false;
	};
	listPC = _cpu.disassemble(listPC, 10);
	return true;
}

bool Debugger::loadCmd(std::string& line) {
	std::string fname;
	Word address;

	std::istringstream iss(line);
	iss >> fname >> std::hex >> address;

	fmt::print("Loading file {} at address {:04x}\n", fname, address);

	try {
		_cpu.mem.loadDataFromFile(fname, address);
	}
	catch (std::exception &e) {
		fmt::print("Load failed: {}\n", e.what());
	}
	catch(...) {
		fmt::print("Load error: unknown exception\n");
	}
	
	return false;
}

bool Debugger::loadScriptCmd(std::string& line) {
	std::string fname;

	std::istringstream iss(line);
	iss >> fname;
	
	fmt::print("Loading command file {}\n", fname);
	
	auto r = parseCommandFile(fname);
	if (r == false) 
		fmt::print("Command file failed\n");
	return r;
}

bool Debugger::loadhexCmd([[maybe_unused]] std::string& line) {
	line = stripLeadingSpaces(line);
	line = stripTrailingSpaces(line);

	return loadHexFile(line);
}

bool Debugger::savememCmd([[maybe_unused]] std::string& line) {
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

bool Debugger::stackCmd([[maybe_unused]] std::string& line) {
	_cpu.Stack();
	return true;
}

bool Debugger::breakpointCmd(std::string& line) {
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

bool Debugger::cpustateCmd([[maybe_unused]] std::string& line) {
	_cpu.printCPUState();
	return true;
}

bool Debugger::autostateCmd([[maybe_unused]] std::string& line) {
	_showCPUStatusAtDebugPrompt = !_showCPUStatusAtDebugPrompt;
	fmt::print("Processor status auto-display ");
	if (_showCPUStatusAtDebugPrompt)
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	return true;
}

bool Debugger::resetListPCCmd(std::string& line) {
	Word i;

	try {
		i = (Word) std::stoul(line, nullptr, 16);
		if (i > MOS6502::LAST_ADDRESS) {
			fmt::print("Error: Program Counter address outside of available address range\n");
			return false;
		}
		listPC = i;
	}
	catch (...) {
		listPC = _cpu.PC;
	}

	fmt::print("List reset to PC {:04x}\n", listPC);
	return true;
}

bool Debugger::memdumpCmd(std::string& line) {
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
		return a <= MOS6502::LAST_ADDRESS;
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

	line = stripSpaces(line);

    if (std::regex_match(line, matches, labelWithOptionalOffsetR) && matches.size() > 1) {
		if (calculateAddress(matches, addr1, addr2, false) && rangeCheckAddr(addr1)) {
			fmt::print("[{:04x}] {:02x}\n", addr1, _cpu.mem.Read(addr1));
			return true;
		}
    } else if (std::regex_match(line, matches, assignValueToLabelR) && matches.size() > 3) {
		value = (Word) std::stoul(matches[3], nullptr, 16);
		if (calculateAddress(matches, addr1, addr2, false) && rangeCheckAddr(addr1) && rangeCheckValue(value)) {
			Byte oldval = _cpu.mem.Read(addr1);
			_cpu.mem.Write(addr1, (Byte) value);
			fmt::print("[{:04x}] {:02x} -> {:02x}\n", addr1, oldval, (Byte) value);
			return true;
		}
    } else if (std::regex_match(line, matches, rangeBetweenLabelsR) && matches.size() > 2) {
		if (calculateAddress(matches, addr1, addr2, true) && 
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2)) {
			_cpu.mem.hexdump(addr1, addr2);
			return true;
		}
    } else if (std::regex_match(line, matches, filterRangeWithValueR) && matches.size() > 4) {
        auto valueExpression = matches[5].str();
		if (calculateAddress(matches, addr1, addr2, true) && rangeCheckAddr(addr1) && 
			rangeCheckAddr(addr2)) {
			_cpu.mem.hexdump(addr1, addr2, valueExpression);
			return true;
		}
    } else if (std::regex_match(line, matches, assignValueToRangeR) && matches.size() > 4) {
        value = (Word) std::stoul(matches[5], nullptr, 16);
		if (calculateAddress(matches, addr1, addr2, true) &&
			rangeCheckAddr(addr1) && rangeCheckAddr(addr2) && rangeCheckValue(value)) {
				_cpu.mem.assign(addr1, addr2, (Byte) value);
			return true;
		}
	} 

	fmt::print("Parse error: '{}'\n", line);
	return false;
}

bool Debugger::memmapCmd([[maybe_unused]] std::string& line) {
	_cpu.mem.printMap();
	return true;
}

bool Debugger::setCmd(std::string& line) {
	std::string v;
	std::string reg;					
	uint64_t value;
	bool flipFlag = false;

	if (containsChar(line, '=')) {  // "set x=5"
		v = stripSpaces(line);	
		reg = split(v, "=");
	} else {                        // "set x 5" or "set d"
		v = stripLeadingSpaces(line);
		v = stripTrailingSpaces(v);
		v = removeDuplicateSpaces(v);
		reg = split(v, " ");
	}

	if (reg.empty()) {
		fmt::print("Parse Error: register or flag required for set command\n");
		return false;
	}

	// reg contains the register, v is the value.
	upCaseString(reg);
	
	try {
		value = std::stoul(v, nullptr, 16);
		if ((reg != "PC" && value > 0xff) ||
		    (reg == "PC" && value > 0xffff)) {
			fmt::print("Error: value would overflow register {}\n", reg);
			return false;
		}
	}
	catch(...) {
		std::string flagChars = "CZIDBVN";
		bool containsFlagCharacter = std::any_of(flagChars.begin(), flagChars.end(), [&reg](char c) {
				    return reg.find(c) != std::string::npos;
		});
		if (containsFlagCharacter)
			flipFlag = true;
		else {
			fmt::print("Parse Error: '{}' is not a valid value for set\n", reg);
			return false;
		}
	}

	if      (reg == "A") 
		_cpu.A = static_cast<Byte>(value);
	else if (reg == "Y")
		_cpu.Y = static_cast<Byte>(value);
	else if (reg == "X")
		_cpu.X = static_cast<Byte>(value);
	else if (reg == "PC")
		_cpu.PC = static_cast<Word>(value);
	else if (reg == "SP")
		_cpu.SP = static_cast<Byte>(value);
	else if (reg == "PS")
		_cpu.PS = static_cast<Byte>(value);
	else if (reg == "C")
		_cpu.Flags.C = flipFlag ? !_cpu.Flags.C : static_cast<bool>(value);
	else if (reg == "Z")
		_cpu.Flags.Z = flipFlag ? !_cpu.Flags.Z : static_cast<bool>(value);
	else if (reg == "I")
		_cpu.Flags.I = flipFlag ? !_cpu.Flags.I : static_cast<bool>(value);
	else if (reg == "D")
		_cpu.Flags.D = flipFlag ? !_cpu.Flags.D : static_cast<bool>(value);
	else if (reg == "B")
		_cpu.Flags.B = flipFlag ? !_cpu.Flags.B : static_cast<bool>(value);
	else if (reg == "V")
		_cpu.Flags.V = flipFlag ? !_cpu.Flags.V : static_cast<bool>(value);
	else if (reg == "N")
		_cpu.Flags.N = flipFlag ? !_cpu.Flags.N : static_cast<bool>(value);
	else {
		fmt::print("No register or status flag '{}'\n", reg);
		return false;
	}

	return true;
}

bool Debugger::resetCmd([[maybe_unused]] std::string& line) {
	fmt::print("Resetting 6502\n");
	_cpu.Reset();    	// Enter reset
	if (_cpu.inReset())
		_cpu.Reset();    // Exit reset
	return true;
}
		
bool Debugger::continueCmd([[maybe_unused]] std::string& line) {
	if (_cpu.hitException()) {
		fmt::print("CPU Exception hit; can't continue.  Reset CPU to clear.\n");
		return false;
	}
	_cpu._debugMode = false;
	return true;
}

bool Debugger::loopdetectCmd([[maybe_unused]] std::string& line) {
	_cpu.enableLoopDetection(!_cpu.isLoopDetectionEnabled());
	fmt::print("Loop detection ");
	if (_cpu.isLoopDetectionEnabled())
		fmt::print("enabled\n");
	else 
		fmt::print("disabled\n");
	return true;
}

bool Debugger::backtraceCmd([[maybe_unused]] std::string& line) {
	showBacktrace();
	return true;
}

bool Debugger::whereCmd([[maybe_unused]] std::string& line) {
	_cpu.disassemble(_cpu.PC, 1);
	return true;
}

bool Debugger::watchCmd(std::string& line) {
	Word addr;
	bool remove = false;

	if (line.empty()) {
		_cpu.mem.listWatch();
		return true;
	}
	
	if (line[0] == '-') {
		line.erase(0,1);
		remove = true;
	}

	try {
		addr = (Word) std::stoul(line, nullptr, 16);
		if (remove) {
			_cpu.mem.clearWatch(addr);
			fmt::print("Watchpoint at memory address {:04x} "
				   "removed\n", addr);
		} else {
			_cpu.mem.enableWatch(addr);
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

bool Debugger::labelCmd(std::string& line) {
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
		
		if (addr > MOS6502::LAST_ADDRESS) {
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

bool Debugger::quitCmd([[maybe_unused]] std::string& line) {
	fmt::print("Exiting emulator\n");
	exit(0);
	return true;
}

bool Debugger::findCmd(std::string& line) {
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

	auto locations = _cpu.mem.find(sequence, filter);
	if (locations.empty()) {
		fmt::print("Sequence not found\n");
		return true;
	}

	fmt::print("Sequence found at addresses:\n");
	for (const auto& addr : locations) 
		fmt::print(" {:04x}\n", addr);

	return true;
}

bool Debugger::matchCommand(const std::string& input, debugFn_t& func) {
	for (const auto &cmd : _debugCommands) {
		if (std::strcmp(cmd.command, input.c_str()) == 0 ||
		    std::strcmp(cmd.shortcut, input.c_str()) == 0) {
			func = cmd.func;
			return true;
		}
	}
	return false;
}

bool Debugger::executeDebuggerCmd(std::string line) {
	debugFn_t f;
	
	line = stripTrailingSpaces(line);
	line = stripLeadingSpaces(line);

	if (line.empty() && _lastDebuggerCommand.empty()) // Blank input
		return true;

	if (line.empty() && !_lastDebuggerCommand.empty()) {
		line = _lastDebuggerCommand;
		fmt::print(": {}\n", line);
	}

	// Check if command is numbers, convert it to an integer and execute that many instructions.
	try {
		uint64_t insCnt = std::stol(line);
		_lastDebuggerCommand = line;
		while (insCnt--) {
			_cpu.executeOneInstruction();
			if (_showCPUStatusAtDebugPrompt) 
				_cpu.printCPUState();
			_cpu.disassemble(_cpu.PC, 1);
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
		_lastDebuggerCommand = savedLine;

	return (this->*f)(line);
}

void Debugger::executeDebug() {
	static bool header = false;

	if (!header) {
		listPC = _cpu.PC;
		header = true;

		fmt::print("\nDebugger starting at PC {:#06x}\n", _cpu.PC);
		_cpu.printCPUState();
		_cpu.disassemble(_cpu.PC, 1);
	}

	std::string line;
	getReadline(line);
	executeDebuggerCmd(line);

	if (!_cpu._debugMode) { 
		fmt::print("Exiting debugger\n");
		header = false;
	}
}
