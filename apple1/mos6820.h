#pragma once

#if defined(__linux__) 
# include <termio.h>
# include <unistd.h>
# include <cstdio>
#elif defined(_WIN64)
# include <conio.h>
#else 
# error "No platform-specific implementation of getch()"
#endif

#include "memory.h"
#include "6502.h"
#include "apple1.h" 

extern CPU cpu;

class MOS6820 : public MemMappedDevice<Address, Cell> {
public:	
	static constexpr Word KEYBOARD   = 0xd010;
	static constexpr Word KEYBOARDCR = 0xd011;
	static constexpr Word DISPLAY    = 0xd012;
	static constexpr Word DISPLAYCR  = 0xd013;

	MOS6820() {
		addAddress(DISPLAY);
		addAddress(KEYBOARD);
		addAddress(DISPLAYCR);
		addAddress(KEYBOARDCR);
	}

	bool housekeeping() {
		auto d = displayHousekeeping();
		auto k = keyboardHousekeeping();
	
		return d && k;
	}

	std::string type() { 
        return "MOS6820"; 
    }

	Cell Read(Address address) override {
		switch (address) {
			case DISPLAY:    return displayRead(address);
			case DISPLAYCR:  return displayRead(address);
			case KEYBOARD:   return keyboardRead(address);
			case KEYBOARDCR: return keyboardRead(address);
		}
		return 0;
	}

	void Write(Address address, Cell c) override { 
		switch (address) {
			case DISPLAY:    
				displayWrite(address, c);
				break;
			case DISPLAYCR:  
				displayWrite(address, c);
				break;
			case KEYBOARD:   
				keyboardWrite(address, c);
				break;
			case KEYBOARDCR: 
				keyboardWrite(address, c);
				break;
		}
	}	

#if defined(__linux__)

    // Make the terminal non-blocking
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
    }

    static void teardown()
    {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= ICANON | ECHO | ISIG;
        tcsetattr(0, TCSANOW, &term);
    }

#elif defined(_WIN64)
    
    static void setup() {}
    static void teardown() {}

#endif

private:
    // Apple 1 keycodes
    static constexpr char CTRL_LBRACKET = 0x1b; // ascii 27
    static constexpr char CTRL_RBRACKET = 0x1d; // ascii 29
    static constexpr char CTRL_MINUS    = 0x1f; // ascii 31
    static constexpr char CR            = 0x0d;
    static constexpr char BELL          = 0x0a;
    #ifdef _WIN64
    static constexpr char DEL           = '\b';
    #else
    static constexpr char DEL           = 0x7f;
    #endif

	// Display
	unsigned char _data = 0;
	bool _haveData = false;
	// Keyboard
	bool kbdCRRead = false;
	std::queue<Cell> queue;

#if defined(__linux__)
    
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

#elif defined(_WIN64) 
  
     bool getch(char& kbdCh) {
        if (_kbhit()) {
            kbdCh = _getch();
            return true;
        }
        return false;
    }

#endif

	bool displayHousekeeping() {
		if (!_haveData) 
			return true;

		auto c = _data & 0x7f;		// clear hi bit
		switch (c) {
		case CR:	// \r
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
		return true;
	}

	bool keyboardHousekeeping() {
		char ch;	
		if (getch(ch) == false) 
			return true;
		
		// Map modern ascii to Apple 1 keycodes
		switch (ch) {

		// Control characters
		case CTRL_RBRACKET:
			fmt::print("\n");
			cpu.Reset();
			if (cpu.inReset())
				cpu.Reset();	
			return true;
		
		case CTRL_LBRACKET:
			cpu.setDebug(true);
			return true;
		
		case CTRL_MINUS:
			fmt::print("\nExiting emulator\n");
			std::exit(0);

		// Regular characters
		case '\n':
			ch = CR;
			break;
		
		case DEL:
			ch = '_';
			break;
		
		default:
			ch = std::toupper(ch);
			break;
		}

		ch |= 0x80;
		queue.push(ch);
		return true;
	}

	void displayWrite(Address address, Cell c) {
		switch (address) {
		case DISPLAY:
			_data = c;
			_haveData = true;
			break;
		case DISPLAYCR:
			break;
		}
	}

	Cell displayRead(Address address) {
		switch (address) {
		case DISPLAY:
			return 0x7f;
		case DISPLAYCR:
			return 0;
		}
		return 0;
	}

	Cell keyboardRead(Address address) {
		Cell ch; 

		switch(address) {
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
				fmt::print("Unknown address in keyboard: {:x}\n", address);
		}	
		return 0;
	}

	void keyboardWrite([[maybe_unused]] Address a, [[maybe_unused]] Cell c) { }
};