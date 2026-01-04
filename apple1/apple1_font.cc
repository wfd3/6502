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

#include "apple1_font.h"
#include <fstream>
#include <fmt/core.h>

// Define the character ROM storage
std::array<std::array<uint8_t, CHAR_HEIGHT>, NUM_CHARS> CHAR_ROM;

bool loadCharROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        fmt::print(stderr, "Error: Could not open character ROM file: {}\n", filename);
        return false;
    }

    // Read the entire ROM directly into CHAR_ROM in native order
    // The 2513 ROM organization (using lower 6 bits of ASCII for addressing):
    // - ROM indices 0x00-0x1F: ASCII 0x40-0x5F (@ through _)
    // - ROM indices 0x20-0x3F: ASCII 0x20-0x3F (space through ?)
    // This matches the hardware addressing: romIndex = asciiChar & 0x3F

    file.read(reinterpret_cast<char*>(CHAR_ROM.data()), ROM_SIZE);

    if (!file || file.gcount() != ROM_SIZE) {
        fmt::print(stderr, "Error: Expected {} bytes, read {} bytes from {}\n",
                   ROM_SIZE, file.gcount(), filename);
        return false;
    }

    return true;
}
