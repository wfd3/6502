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
