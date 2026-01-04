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

#include "display_interface.h"
#include <fmt/core.h>
#include <cctype>

#if defined(_WIN64)
#include <cstdlib>
#endif

void TerminalDisplay::writeChar(uint8_t c) {
    // Clear high bit (Apple 1 format)
    c &= 0x7f;

    // Handle special characters
    if (c == 0x0d) {  // Carriage return
        fmt::print("\n");
    } else if (c == 0x5f) {  // Underscore = backspace
        fmt::print("\b \b");
    } else if (c == 0x0a) {  // Line feed = bell
        fmt::print("\a");
    } else if (c >= 0x20 && c <= 0x7e) {  // Printable characters
        fmt::print("{}", static_cast<char>(std::toupper(c)));
    }
    // Ignore non-printable characters
}

void TerminalDisplay::clear() {
#if defined(_WIN64)
    std::system("cls");
#else
    fmt::print("\033[2J\033[H");  // ANSI clear screen and home cursor
#endif
}

void TerminalDisplay::refresh() {
    // For terminal output, flush is automatic with fmt::print
    // No explicit refresh needed
}
