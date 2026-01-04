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

#pragma once

#include "keyboard_interface.h"
#include <cstdint>

#if defined(__linux__) || defined(__MACH__)
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#elif defined(_WIN64)
#include <conio.h>
#include <windows.h>
#endif

// Terminal-based keyboard input (original implementation)
class TerminalKeyboard : public IKeyboard {
public:
    bool getChar(uint8_t& ch) override;
    void setNonBlocking() override;
    void setBlocking() override;

private:
#if defined(__linux__) || defined(__MACH__)
    // Linux/Mac implementation
#elif defined(_WIN64)
    static bool _CtrlC_Pressed;
    static BOOL WINAPI ConsoleCtrlHandler(DWORD CtrlType);
#endif
};
