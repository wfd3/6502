// Stand alone program to run the 6502 and 65C02 functional tests. 
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
#include <csignal>
#include <sstream>
#include <queue>
#include <fmt/core.h>

#include <65C02.h>
#include <memory.h>

#include "mos6820.h"

using Address = uint16_t;
using Byte    = uint8_t;

bool startInDebugger = false;
bool loopDetection = true;
std::string testFile;
Address loadAddress;
Address haltAddress = 0; // Default value
Address startAddress = 0;
bool haveHaltAddress = false;

void help() {
    fmt::print("Usage: program [options] <testfile> <loadAddress> <startAddress>\n"
            	"Options:\n"
              	"  --help           Show this help message\n"
              	"  --debug          Start in debugger\n"
              	"  --halt=<address> Set halt address (optional)\n");
}

bool parseCommandLine(int argc, char* argv[]) {
    if (argc < 3) {
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

    return true;
}


int main(int argc, char* argv[]) {
	if (!parseCommandLine(argc, argv)) 
		return 1;

	constexpr Address PIA_BASE_ADDRESS = 0xd010;

	// Create the memory, CPU, PIA and bus clock
	Memory<Address, Byte> mem(MOS65C02::MAX_MEM);
	MOS65C02 cpu(mem);
	auto pia = std::make_shared<MOS6820<Address, Byte>>();

	fmt::print("  Reset        = Control-\\\n");
	fmt::print("  Clear screen = Control-[\n");
	fmt::print("  Debugger     = Control-]\n");
	fmt::print("  Quit         = Control-Backspace\n");

	mem.Reset();
	mem.mapRAM(0x0000, 0xffff);
    mem.mapDevice(pia, PIA_BASE_ADDRESS);
	pia->setTermNonblocking();	

	fmt::print("Loading {} at {:04x}, start address {:04x}", testFile, loadAddress, startAddress);
	if (haveHaltAddress) {
		cpu.setHaltAddress(haltAddress);
		fmt::print(", halt at {:04x}", haltAddress);
	}
	fmt::print("\n");
	fmt::print(" Loop detection is: {}\n", loopDetection ? "On" : "Off");
	fmt::print(" Start in debugger: {}\n", startInDebugger ? "Yes" : "No");

	mem.loadDataFromFile(testFile, loadAddress);
	cpu.setResetVector(startAddress);
	cpu.loopDetection(loopDetection);
	cpu.Reset();

	cpu.setDebugMode(startInDebugger);

	 while (!cpu.isPCAtHaltAddress()) {
		// If we're in debug mode we have to toggle the terminal out of and in to non-blocking mode
		// so the CPU debugger (implemented in the CPU class) can access the terminal in non-blocking 
		// mode.
		auto debug = cpu.isInDebugMode();
		if (debug) 
				pia->setTermBlocking();
		
		cpu.execute();
		
		if (debug) 
				pia->setTermNonblocking();

		auto signals = pia->housekeeping();

		for (const auto& signal : signals) {
				switch(signal) {
				case Device::None:
						break;
				case Device::Reset:
						cpu.Reset();
						if (cpu.inReset()) 
								cpu.Reset();
						break;
				case Device::Debug:
						cpu.setDebugMode(true);
						break;
				case Device::Exit:
						fmt::print("\nExiting emulator\n");
						std::exit(0);
				}
		}
	}

	pia->setTermNonblocking();	
	fmt::print("Test passed\n");

	return 0;
}
