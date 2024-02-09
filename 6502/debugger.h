//
// CPU debugger
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

#include <6502.h>

class MOS6502;

// GNU Readline forward declaration with extern "C" linkage for later use as a
// friend in class CPU. 
extern "C" char **readlineCompletionCallback(const char* text, int start, int end);

class Debugger {
public:
    Debugger(MOS6502& cpu) : _cpu(cpu), _debugCommands(setupDebugCommands()) {
        setupConsoleInput();
	    deleteAllBreakpoints();
    }

	void executeDebug();
    bool debugModeOnException() { return _debugModeOnException; }
    void setDebugModeOnException(bool b) { _debugModeOnException = b; }

    void setCPUStatusAtPrompt(bool b) { _showCPUStatusAtDebugPrompt = b; }

    void listBreakpoints();
	bool isBreakpoint(Word);
	bool isPCBreakpoint();
	void deleteBreakpoint(Word);
	void addBreakpoint(Word);
	void deleteAllBreakpoints();

	// Backtrace
	void showBacktrace();
	void addBacktrace(Word);
	void addBacktraceInterrupt(Word);
	void removeBacktrace();

	// Labels
	void showLabels();
	void addLabel(Word, const std::string);
	bool labelAddress(const std::string&, Word&);
	void removeLabel(const Word);
	bool lookupAddress(const std::string&, Word&);
	bool parseCommandFile(const std::string&);
	std::string getLabelByte(const uint8_t);
	std::string addressLabel(const Word);
	std::string addressLabelSearch(const Word, const int8_t searchWidth = 3);

private:
    MOS6502& _cpu;
	std::string _lastDebuggerCommand = "";
	bool _showCPUStatusAtDebugPrompt = false;
	bool _debugModeOnException = false;
	typedef bool (Debugger::*debugFn_t)(std::string&);

	// Debugger commands
	struct debugCommand {
		const char *command;
		const char *shortcut;
		const debugFn_t func;
		const bool doFileCompletion;
		const std::string helpMsg;
	};
	const std::vector<debugCommand> _debugCommands;

	bool executeDebuggerCmd(std::string);
	void dumpStack();
	void parseMemCommand(std::string);

    static std::vector<debugCommand> setupDebugCommands();
	bool matchCommand(const std::string &, debugFn_t &);

	// Command handling functions
	bool helpCmd(std::string&);
	bool listCmd(std::string&);
	bool loadCmd(std::string&);
	bool stackCmd(std::string&);
	bool breakpointCmd(std::string&);
	bool cpustateCmd(std::string&);
	bool autostateCmd(std::string&);
	bool resetListPCCmd(std::string&);
	bool memdumpCmd(std::string&);
	bool memmapCmd(std::string&);
	bool setCmd(std::string&);
	bool resetCmd(std::string&);
	bool continueCmd(std::string&);
	bool loopdetectCmd(std::string&);
	bool backtraceCmd(std::string&);
	bool labelCmd(std::string&);
	bool whereCmd(std::string&);
	bool watchCmd(std::string&);
	bool quitCmd(std::string&);
	bool findCmd(std::string&);
	bool clockCmd(std::string&);
	bool loadScriptCmd(std::string&);
	bool savememCmd(std::string&);
	bool loadhexCmd(std::string&);

	// Hex file
	bool loadHexFile(const std::string&);
	bool saveToHexFile(const std::string&, const std::vector<std::pair<Word, Word>>&);
	bool saveToHexFile(const std::string&, Word startAddress, Word);

	// Breakpoints
	std::set<Word> breakpoints;

	// Backtrace
	std::vector<std::string> backtrace;  // Use ::vector so we can easily iterate

	// Labels
	std::unordered_map<Word, std::string> addrToLabel;
	std::unordered_map<std::string, Word> labelToAddr;

	// GNU readline command completion, used by the debugger, needs access to the _debugCommands vector.
    char **readlineCompletionCallback(const char*, int, int);
	friend char *readlineCommandGenerator(const char*, int);
	friend char **readlineCompletionCallback(const char*, int, int);
	void setupConsoleInput();
};