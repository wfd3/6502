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

#include <cstdint>

// Platform-agnostic control key codes
// These are special non-printable codes used to signal control operations
// to the emulator. They are encoded by keyboard handlers and decoded by MOS6820.
namespace ControlCodes {
    constexpr uint8_t CLEAR_SCREEN  = 0xff;
    constexpr uint8_t RESET         = 0xfe;
    constexpr uint8_t DEBUGGER      = 0xfd;
    constexpr uint8_t EXIT          = 0xfc;
    constexpr uint8_t STATUS        = 0xfb;
}
