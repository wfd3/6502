//
// Debugger functions for emulated 6502.
//   Breakpoints, backtrace, address labels, etc.
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
#include <iostream>
#include <vector>
#include <list>

#include <fmt/format.h>
#include <fmt/core.h>

#include <memory>
#include <6502.h>
#include <utils.h>


//////////
// Called by MOS6502::CPU() to initialize the debugger
void MOS6502::initDebugger() {
	setupConsoleInput();
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
	fmt::print("  | Pending: IRQ - {}, NMI - {}, Reset - {}; inReset? - {}\n",
		   yesno(pendingIRQ()), yesno(pendingNMI()), yesno(_pendingReset), yesno(_inReset));
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

