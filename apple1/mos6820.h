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
#include <unistd.h>
# ifdef __linux__
#  include <termio.h>
# endif
# ifdef __MACH__
#  include <termios.h>
#  include <sys/ioctl.h>
# endif
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

	MOS6820() {
		this->_ioPorts = {KEYBOARD, KEYBOARDCR, DISPLAY, DISPLAYCR};
	}

  	Device::BusSignals housekeeping() override {
       Device::BusSignals signals;
		
		auto d = displayHousekeeping();
        signals.insert(d);
		auto k = keyboardHousekeeping();
        signals.insert(k);

		return signals;
	}

	std::string type() const override { 
		return fmt::format("MOS6820");
    }

	Cell Read(Address address) override {
		auto port = this->decodeAddress(address);

		switch (port) {
		case DISPLAY:
		case DISPLAYCR:
			return displayRead(port);
		case KEYBOARD:
		case KEYBOARDCR:
			return keyboardRead(port);
		}

		return 0;
	}

	void Write(Address address, Cell c) override {
		uint8_t port = this->decodeAddress(address);

		switch (port) {
		case DISPLAY:
		case DISPLAYCR:
			displayWrite(port, c);
			break;
		case KEYBOARD:
		case KEYBOARDCR:
			keyboardWrite(port, c);
			break;
		}
	}	

#if defined(__linux__) || defined(__MACH__)

    // Make the terminal non-blocking and allow Control-C, Control-\ to
	// pass through to the read() call.
    void setTermNonblocking()
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

		_terminalBlocking = true;
    }

    void setTermBlocking()
    {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= ICANON | ECHO | ISIG;
        tcsetattr(0, TCSANOW, &term);

		// Restore ^C and ^-Backslash 
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);

		_terminalBlocking = false;
	}

#elif defined(_WIN64)

	void setTermBlocking() {
		_CtrlC_Pressed = false;
		SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
		_terminalBlocking = true;
	}
	void setTermNonblocking() {
		SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);
		_terminalBlocking = false;
	}

#else 
	
	void setTermBlocking() {
		_terminalBlocking = true;
	}
	void setTermNonblocking() {
		_terminalBlocking = false;
	}

#endif

	void changeTerminalState() {
		if (_terminalBlocking) 
			setTermNonblocking();
		else 
			setTermBlocking();		
	}

private:
	bool _terminalBlocking = true;

	// Offsets from MemMappedDevice::_baseAddress for these memory-mapped I/O ports.  These are in order
	// and cannot change.
	static constexpr Word KEYBOARD   = 0;
	static constexpr Word KEYBOARDCR = 1;
	static constexpr Word DISPLAY    = 2;
	static constexpr Word DISPLAYCR  = 3;

    // Apple 1 keycodes
	static constexpr char NEWLINE        = 0x0a;
	static constexpr char CARRAGE_RETURN = 0x0d;
	static constexpr char BELL			 = 0x0a;
#ifdef _WIN64
	static constexpr char CTRL_C		 = 0x03;
	static constexpr char DEL			 = '\b';
	static constexpr char CTRL_BACKSPACE = 0x7f; // Quit emulator
#else
	static constexpr char DEL			 = 0x7f; // Backspace on unix/linux
	static constexpr char CTRL_BACKSPACE = 0x08; 
#endif
	static constexpr char CTRL_LBRACKET  = 0x1b;  // Clear screen
	static constexpr char CTRL_BACKSLASH = 0x1c; // Reset/Jump to Wozmon
	static constexpr char CTRL_RBRACKET  = 0x1d;  // Enter debugger

	// Platform agnostic remapping of control keycodes.  These are encoded in getch() and returned 
	// as a signed char, and should be non-printable ASCII characters.
	static constexpr char CLEARSCR_CHAR  = 0x00;
	static constexpr char RESET_CHAR     = 0x01;
	static constexpr char DEBUGGER_CHAR  = 0x02; // 0x03 is Control-C
	static constexpr char EXIT_CHAR      = 0x04;

	// Display
	bool _haveDspData = false;
	unsigned char _dspData = 0;
	
	// Keyboard
	bool _kbdCRRead = false;
	std::queue<Cell> _charQueue;

#if defined(__linux__) || defined(__MACH__)
    
     bool getch(char &kbdCh) const {
        int byteswaiting;
        char ch;

        ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
        if (byteswaiting == 0)
            return false;

        read(STDIN_FILENO, &ch, 1);
		
		switch (ch) {
		case CTRL_BACKSPACE:
			kbdCh = EXIT_CHAR;
			break;
		case CTRL_BACKSLASH:
			kbdCh = RESET_CHAR;
			break;
		case CTRL_RBRACKET:
			kbdCh = DEBUGGER_CHAR;
			break;
		case CTRL_LBRACKET:
			kbdCh = CLEARSCR_CHAR;
			break;
		default:
	        kbdCh = ch;
			break;
		}
        return true;
    }

	void clearScreen() const {
		const std::string CLS = "\033[2J\033[H"; 
		fmt::print("{}", CLS);
	 }

#elif defined(_WIN64) 

	static bool _CtrlC_Pressed;

	bool wasCtrlCPressed() {
		auto r = _CtrlC_Pressed;
		_CtrlC_Pressed = false;
		return r;
	}
	
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

	void clearScreen() const {
		system("cls");
	}

	bool getch(char& kbdCh) const {
		
		if (wasCtrlCPressed()) {
			kbdCh = CTRL_C;
			return true;
		}

		if (!_kbhit()) 
			return false;
		
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

#else

	// Clear the Apple 1 display
	void clearScreen() const {}

	// Check and get a key from the keyboard, returning true and setting kbdCh if a key was ready
	// for us.  Map Control keys to platform agnostic values (see above)
	bool getch(char& kbdCh) {
		return false;
	}

#endif

	Device::Lines displayHousekeeping() {
		if (!_haveDspData) 
			return Device::None;

		auto c = _dspData & 0x7f;	// clear hi bit
		switch (c) {
		case CARRAGE_RETURN:	// \r
			fmt::print("\n");
			break;
		case '_':				// Backspace
			fmt::print("\b");
			break;
		case BELL:				// Bell
			fmt::print("\a");
			break;
		default:
			if (c >= 0x20 && c <= 0x7e)
				fmt::print("{:c}", toupper(c));
		}

		_haveDspData = false;
		return Device::None;
	}

	Device::Lines keyboardHousekeeping() {
		char ch;	
        auto retval = Device::None;

        auto charsPending = getch(ch);
		if (!charsPending)
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
		_charQueue.push(ch);

        return retval;
	}

	void displayWrite(uint8_t port, Cell c) {
		switch (port) {
		case DISPLAY:
			_dspData = c;
			_haveDspData = true;
			break;
		}
	}

	Cell displayRead(uint8_t port) const {
		switch (port) {
		case DISPLAY:
			return 0x7f;
		case DISPLAYCR: 
			return 0;
		}
	
		return 0;
	}

	Cell keyboardRead(uint8_t port) {
		Cell ch;

		switch (port) {
		case KEYBOARDCR:
			// Check if characters are pending, return key code if so
			_kbdCRRead = true;
			if (_charQueue.empty())
				return 0;

			return _charQueue.front();

		case KEYBOARD:
			if (_charQueue.empty())
				return 0;

			ch = _charQueue.front();

			// Applesoft Basic Lite does a blind, unchecked read on the keyboard port
			// looking for a ^C.  If it sees one, it then does a read on the keyboard
			// control register followed by a read of the keyboard port, expecting to
			// get the same ^C.  This logic forces a keyboard control register read 
			// before removing the character from the queue, thus preventing an 
			// infinite loop.
			if (_kbdCRRead) {
				_charQueue.pop();
				_kbdCRRead = false;
			}
			return ch;

		default:
			return 0;
		}
	}

	void keyboardWrite([[maybe_unused]] uint8_t port, [[maybe_unused]] Cell c) { }
};

#ifdef _WIN64
template <typename Address, typename Cell>
bool MOS6820<Address, Cell>::_CtrlC_Pressed = false;
#endif
