//
// MOS6820 Peripheral Interface Adaptor emulator, handling keyboard input and display output.  This also 
// handles control keys (reset, clear screen, debugger entry and emulator exit) input but not processing
// of these keys.
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

#pragma once
#include <cstdint>

#if defined(__linux__) || defined(__MACH__)
# ifdef __linux__
# include <termio.h>
# endif
# ifdef __MACH__
# include <termios.h>
# include <sys/ioctl.h>
# endif
# include <unistd.h>
# include <cstdio>
#elif defined(_WIN64)
# include <conio.h>
# include <windows.h>
#else 
# error "No platform-specific implementation of getch()"
#endif

#include "memory.h"

template<class Address = uint16_t, class Cell = uint8_t>
class MOS6820 : public MemMappedDevice<Address, Cell> {
public:	
	
	// Offsets from baseAddress for these memory-mapped I/O ports.  These are in order and
	// cannot change.
	static constexpr Word KEYBOARD   = 0;
	static constexpr Word KEYBOARDCR = 1;
	static constexpr Word DISPLAY    = 2;
	static constexpr Word DISPLAYCR  = 3;

	MOS6820(Address base) {
		Map(base);
	}
	MOS6820() {}

	void Map(Address base) {
		baseAddress = base;

		MemMappedDevice<Address, Cell>::addAddress(baseAddress + DISPLAY);
		MemMappedDevice<Address, Cell>::addAddress(baseAddress + KEYBOARD);
		MemMappedDevice<Address, Cell>::addAddress(baseAddress + DISPLAYCR);
		MemMappedDevice<Address, Cell>::addAddress(baseAddress + KEYBOARDCR);
	}

  	Device::BusSignals housekeeping() override {
        std::vector<Device::Lines> r;
		auto d = displayHousekeeping();
        r.push_back(d);
		auto k = keyboardHousekeeping();
        r.push_back(k);
		return r;
	}

	std::string type() const override { 
        return "MOS6820"; 
    }

	Cell Read(Address address) override {
		auto addr = address - baseAddress;

		switch (addr) {
		case DISPLAY:
		case DISPLAYCR:
			return displayRead(addr);
		case KEYBOARD:
		case KEYBOARDCR:
			return keyboardRead(addr);
		}

		return 0;
	}

	void Write(Address address, Cell c) override {
		uint8_t addr = address - baseAddress;
		switch (addr) {
		case DISPLAY:
		case DISPLAYCR:
			displayWrite(addr, c);
			break;
		case KEYBOARD:
		case KEYBOARDCR:
			keyboardWrite(addr, c);
			break;
		}
	}	

#if defined(__linux__) || defined(__MACH__)

    // Make the terminal non-blocking and allow Control-C, Control-\ to
	// pass through to the read() call.
    static void setup()
    {
        termios term;
        fflush(stdout);
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~(ICANON | ECHO | ISIG);
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        setbuf(stdin, nullptr);
        setbuf(stdout, nullptr);
        tcflush(0, TCIFLUSH);

		// Ignore ^C and ^-Backslash
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
    }

    static void teardown()
    {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= ICANON | ECHO | ISIG;
        tcsetattr(0, TCSANOW, &term);

		// Restore ^C and ^-Backslash 
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}

#elif defined(_WIN64)

	static void setup() {
		CtrlC_Pressed = false;
		SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	}
	static void teardown() {
		SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);
	}

	bool wasCtrlCPressed() {
		auto r = CtrlC_Pressed;
		CtrlC_Pressed = false;
		return r;
	}

#endif

private:

    // Apple 1 keycodes
	static constexpr char NEWLINE        = 0x0a;
	static constexpr char CARRAGE_RETURN = 0x0d;
	static constexpr char BELL			 = 0x0a;
#ifdef _WIN64
	static constexpr char CTRL_C		 = 0x03;
	static constexpr char DEL			 = '\b';
#else
	static constexpr char DEL			 = 0x7f; // Backspace on unix/linux
#endif

#ifdef _WIN64   
	static constexpr char CTRL_BACKSPACE = 0x7f; // Quit emulator
#else
	static constexpr char CTRL_BACKSPACE = 0x08; 
#endif
	static constexpr char CTRL_LBRACKET = 0x1b;  // Clear screen
	static constexpr char CTRL_BACKSLASH = 0x1c; // Reset/Jump to Wozmon
	static constexpr char CTRL_RBRACKET = 0x1d;  // Enter debugger

	// These are encoded in getch() and returned as signed char.  Should be 
	// non-printable ASCII characters.
	static constexpr char CLEARSCR_CHAR  = 0x00;
	static constexpr char RESET_CHAR     = 0x01;
	static constexpr char DEBUGGER_CHAR  = 0x02; // 0x03 is Control-C
	static constexpr char EXIT_CHAR      = 0x04;

	// Where this device is mapped in memory. 
	// TODO: this should go away.
	Address baseAddress            = 0;

	// Display
	unsigned char _data = 0;
	bool _haveData = false;
	// Keyboard
	bool kbdCRRead = false;
	std::queue<Cell> queue;

#if defined(__linux__) || defined(__MACH__)
    
     bool getch(char &kbdCh) {
        int byteswaiting;
        char ch;

        ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
        if (byteswaiting == 0)
            return false;

        read(STDIN_FILENO, &ch, 1);
        kbdCh = ch;
        return true;
    }

	 void clearScreen() {
		 fmt::print("{}", CLS)
	 }

#elif defined(_WIN64) 

	static bool CtrlC_Pressed;

	static BOOL WINAPI ConsoleCtrlHandler(DWORD CtrlType) {
		switch (CtrlType) {
		case CTRL_C_EVENT:
			CtrlC_Pressed = true;
			return true;
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			return false;
		}

		return false;
	}

	void clearScreen() {
		system("cls");
	}

	bool getch(char& kbdCh) {
		
		if (wasCtrlCPressed()) {
			fmt::print("^C\n");
			kbdCh = CTRL_C;
			return true;
		}

		if (_kbhit()) {
			auto c = _getch();
			if (GetAsyncKeyState(VK_CONTROL) < 0) { // Control was held when key was pressed
				switch (c) {
				case CTRL_BACKSPACE:
					kbdCh = EXIT_CHAR;
					return true;
				case CTRL_BACKSLASH:
					kbdCh = RESET_CHAR;
					return true;
				case CTRL_LBRACKET:
					kbdCh = CLEARSCR_CHAR;
					return true;
				case CTRL_RBRACKET:
					kbdCh = DEBUGGER_CHAR;
					return true;
				}
			}
		
			kbdCh = c;
			return true;
		}

		return false;
    }

#endif

	Device::Lines displayHousekeeping() {
		if (!_haveData) 
			return Device::None;

		auto c = _data & 0x7f;		// clear hi bit
		switch (c) {
		case CARRAGE_RETURN:	// \r
			fmt::print("\n");
			break;
		case '_':		// Backspace
			fmt::print("\b");
			break;
		case BELL:		// Bell
			fmt::print("\a");
			break;
		default:
			if (c >= 0x20 && c <= 0x7e)
				fmt::print("{:c}", toupper(c));
		}

		_haveData = false;
		return Device::None;
	}

	Device::Lines keyboardHousekeeping() {
		char ch;	
        auto retval = Device::None;
		
        if (getch(ch) == false) 
			return retval;
		
		// Handle control characters or map modern ascii to Apple 1 keycodes
		switch (ch) {

		// Control characters; don't queue these.
		case RESET_CHAR:
            return Device::Reset;

		case DEBUGGER_CHAR:
			return Device::Debug;

        case EXIT_CHAR: 
			return Device::Exit;
		
		case CLEARSCR_CHAR:
			clearScreen();
			return Device::None;

		// Regular characters; do queue these
		case NEWLINE:
			ch = CARRAGE_RETURN;
			break;
		
		case DEL:
			ch = '_';
			break;
		}

		ch = std::toupper(ch);
		ch |= 0x80;
		queue.push(ch);

        return retval;
	}

	void displayWrite(uint8_t addr, Cell c) {
		switch (addr) {
		case DISPLAY:
			_data = c;
			_haveData = true;
			break;
		}
	}

	Cell displayRead(uint8_t addr) {
		switch (addr) {
		case DISPLAY:
			return 0x7f;
		case DISPLAYCR: 
			return 0;
		}
	
		return 0;
	}

	Cell keyboardRead(uint8_t addr) {
		Cell ch;

		switch (addr) {
		case KEYBOARDCR:
			// Check if characters are pending, return key code if so
			kbdCRRead = true;
			if (queue.empty())
				return 0;

			return queue.front();

		case KEYBOARD:
			if (queue.empty())
				return 0;

			ch = queue.front();

			// Applesoft Basic Lite does a blind, unchecked read on the keyboard port
			// looking for a ^C.  If it sees one, it then does a read on the keyboard
			// control register followed by a read of the keyboard port, expecting to
			// get the same ^C.  This logic forces a keyboard control register read 
			// before removing the character from the queue, thus preventing an 
			// infinite loop.
			if (kbdCRRead) {
				queue.pop();
				kbdCRRead = false;
			}
			return ch;

		default:
			return 0;
		}
	}

	void keyboardWrite([[maybe_unused]] uint8_t a, [[maybe_unused]] Cell c) { }
};

#ifdef _WIN64
template <typename Address, typename Cell>
bool MOS6820<Address, Cell>::CtrlC_Pressed = false;
#endif
