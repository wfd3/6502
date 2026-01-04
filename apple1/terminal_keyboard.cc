// Copyright (C) 2025 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#include "terminal_keyboard.h"
#include "control_codes.h"
#include <csignal>
#include <cstdio>

// Apple 1 keycodes and control character mappings
static constexpr char CTRL_BACKSPACE  = 0x08;
static constexpr char CTRL_LBRACKET   = 0x1b; // Clear screen
static constexpr char CTRL_BACKSLASH  = 0x1c; // Reset/Jump to Wozmon
static constexpr char CTRL_RBRACKET   = 0x1d; // Enter debugger
static constexpr char CTRL_T          = 0x14; // Toggle status display

using namespace ControlCodes;

#if defined(__linux__) || defined(__MACH__)

bool TerminalKeyboard::getChar(uint8_t& ch) {
    int byteswaiting;
    char c;

    ioctl(STDIN_FILENO, FIONREAD, &byteswaiting);
    if (byteswaiting == 0)
        return false;

    read(STDIN_FILENO, &c, 1);

    switch (c) {
    case CTRL_BACKSPACE:
        ch = EXIT;
        break;
    case CTRL_BACKSLASH:
        ch = RESET;
        break;
    case CTRL_RBRACKET:
        ch = DEBUGGER;
        break;
    case CTRL_LBRACKET:
        ch = CLEAR_SCREEN;
        break;
    case CTRL_T:
        ch = STATUS;
        break;
    default:
        ch = c;
        break;
    }
    return true;
}

void TerminalKeyboard::setNonBlocking() {
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

void TerminalKeyboard::setBlocking() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO | ISIG;
    tcsetattr(0, TCSANOW, &term);

    // Restore ^C and ^-Backslash
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

#elif defined(_WIN64)

bool TerminalKeyboard::_CtrlC_Pressed = false;

BOOL WINAPI TerminalKeyboard::ConsoleCtrlHandler(DWORD CtrlType) {
    switch (CtrlType) {
    case CTRL_C_EVENT:
        _CtrlC_Pressed = true;
        return true;
    }
    return false;
}

bool TerminalKeyboard::getChar(uint8_t& ch) {
    static constexpr char DEL = '\b';
    static constexpr char CTRL_C = 0x03;

    if (_CtrlC_Pressed) {
        _CtrlC_Pressed = false;
        ch = CTRL_C;
        return true;
    }

    if (!_kbhit())
        return false;

    auto c = _getch();
    if (GetAsyncKeyState(VK_CONTROL) < 0) { // Control was held when key was pressed
        switch (c) {
        case DEL:  // Ctrl-Backspace
            ch = EXIT;
            return true;
        case CTRL_BACKSLASH:
            ch = RESET;
            return true;
        case CTRL_LBRACKET:
            ch = CLEAR_SCREEN;
            return true;
        case CTRL_RBRACKET:
            ch = DEBUGGER;
            return true;
        case CTRL_T:
            ch = STATUS;
            return true;
        }
    }

    ch = c;
    return true;
}

void TerminalKeyboard::setNonBlocking() {
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
}

void TerminalKeyboard::setBlocking() {
    SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);
}

#endif
