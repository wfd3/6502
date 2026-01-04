// Headless test harness for running 6502 and 65C02 functional tests.
// This version has no keyboard/terminal interaction and is designed for
// automated testing.
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

#include <iostream>
#include <sstream>
#include <fmt/core.h>

#include <65C02.h>
#include <memory.h>

using Address = uint16_t;
using Byte    = uint8_t;

bool loopDetection = true;
bool startInDebugger = false;
std::string testFile;
Address loadAddress;
Address haltAddress = 0;
Address startAddress = 0;
bool haveHaltAddress = false;

void help() {
    fmt::print("Usage: tester2 [options] <testfile> <loadAddress> <startAddress>\n"
            	"Options:\n"
              	"  --help           Show this help message\n"
              	"  --debug          Start in debugger (enters debugger on loop detection)\n"
              	"  --noloop         Disable loop detection\n"
              	"  --halt=<address> Set halt address (optional)\n"
              	"\n"
              	"Headless test harness with optional debugger support.\n"
              	"Addresses must be in hexadecimal.\n");
}

bool parseCommandLine(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Error: Not enough arguments\n";
        help();
        return false;
    }

	bool haveLoadAddress = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            help();
            return false;
        } else if (arg == "--debug") {
            startInDebugger = true;
		} else if (arg == "--noloop") {
			loopDetection = false;
        } else if (arg.rfind("--halt=", 0) == 0) {
            std::string haltValueStr = arg.substr(7);
            std::istringstream iss(haltValueStr);
            if (!(iss >> std::hex >> haltAddress)) {
                std::cerr << "Error: Invalid halt address\n";
                return false;
            }
			haveHaltAddress = true;
        } else if (testFile.empty()) {
            testFile = arg;
        } else if (!haveLoadAddress) {
            std::istringstream iss(arg);
            if (!(iss >> std::hex >> loadAddress)) {
                std::cerr << "Error: Invalid load address\n";
                return false;
            }
			haveLoadAddress = true;
		} else {
            std::istringstream iss(arg);
            if (!(iss >> std::hex >> startAddress)) {
                std::cerr << "Error: Invalid start address\n";
                return false;
            }
        }
    }

    if (testFile.empty()) {
        std::cerr << "Error: Test file is required\n";
        return false;
    }

    if (!haveLoadAddress) {
        std::cerr << "Error: Load address is required\n";
        return false;
    }

    return true;
}


int main(int argc, char* argv[]) {
	if (!parseCommandLine(argc, argv))
		return 1;

	// Create memory and CPU - no devices needed for headless testing
	Memory<Address, Byte> mem(MOS65C02::LAST_ADDRESS);
	MOS65C02 cpu(mem);

	mem.Reset();
	mem.mapRAM(0x0000, 0xffff);

	fmt::print("Loading {} at {:04x}, start address {:04x}", testFile, loadAddress, startAddress);
	if (haveHaltAddress) {
		cpu.setHaltAddress(haltAddress);
		fmt::print(", halt at {:04x}", haltAddress);
	}
	fmt::print("\n");
	fmt::print("Loop detection: {}\n", loopDetection ? "On" : "Off");
	fmt::print("Start in debugger: {}\n", startInDebugger ? "Yes" : "No");

	mem.loadDataFromFile(testFile, loadAddress);
	cpu.setResetVector(startAddress);
	cpu.enableLoopDetection(loopDetection);
	cpu.Reset();

	// If --debug flag is set, enable debug mode and enter debugger on exceptions (like loop detection)
	if (startInDebugger) {
		cpu.setDebugMode(true);
		cpu.setDebugModeOnException(true);
	}

	// Simple execution loop - just run until halt address or loop detected
	while (!cpu.isPCAtHaltAddress()) {
		cpu.execute();
	}

	fmt::print("Test passed\n");

	return 0;
}
