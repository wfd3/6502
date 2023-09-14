//
// Windows OS specific functions
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

#include <windows.h>
#include <consoleapi.h>
#include <wincontypes.h>
#include <cctype>
#include <conio.h>

#include <6502.h>

extern CPU cpu;

void enable_raw_mode() { }
void disable_raw_mode() { }

std::string commandKeyBanner = "^C is reset, ^Break is debugger";

BOOL WINAPI consoleHandler(DWORD signal) {
	switch (signal) {
	case CTRL_C_EVENT:
		fmt::print("\n");
		cpu.setPendingReset();
		break;

	case CTRL_BREAK_EVENT:
		cpu.setDebug(true);
		break;

	case CTRL_CLOSE_EVENT:
		fmt::print("\nExiting emulator\n");
		exit(0);
		break;
	}

	return true;
}

void setupSignals() {
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		fmt::print("Could not set control handler\n");
	}
}

bool getch(char& kbdCh) {
	if (_kbhit()) {
		kbdCh = _getch();
		return true;
	}
	return false;
}
