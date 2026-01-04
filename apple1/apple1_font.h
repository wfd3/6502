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
#include <array>

// Undefine any system macros that might conflict with our constants
#ifdef CHAR_WIDTH
#undef CHAR_WIDTH
#endif
#ifdef CHAR_HEIGHT
#undef CHAR_HEIGHT
#endif

// Apple 1 Character ROM Data
// Based on the Signetics 2513 character generator (CM2140 variant)
//
// Specifications:
// - 64 characters (ASCII 0x20-0x5F)
// - 5 pixels wide x 8 pixels tall per character
// - Characters displayed in 7x8 cell (5 bits data + 2 blank columns for spacing)
//
// Font format:
// - Each character is 8 bytes (one per scan line, top to bottom)
// - Each byte contains 5 bits of pixel data in bits 4-0
// - Bit 4 = leftmost pixel, Bit 0 = rightmost pixel
//
// ROM Organization:
// - The 2513 ROM stores characters in a specific order:
//   - First 256 bytes: Characters 0x40-0x5F (@ through _)
//   - Next 256 bytes: Characters 0x20-0x3F (space through ?)
//
// References:
// - https://www.sbprojects.net/projects/apple1/terminal.php
// - https://github.com/bbenchoff/2513CharGenAdapter
// - https://p-l4b.github.io/2513/
// - https://www.thealmightyguru.com/Wiki/index.php?title=Signetics_2513

constexpr int CHAR_WIDTH = 5;   // Pixel width of character
constexpr int CHAR_HEIGHT = 8;  // Pixel height of character (including blank row)
constexpr int DISPLAY_WIDTH = 7; // Display cell width (5 + 2 blank columns)
constexpr int DISPLAY_HEIGHT = 8; // Display cell height (7 + 1 blank row)
constexpr int FONT_START = 0x20; // First character in font (space)
constexpr int FONT_END = 0x5F;   // Last character in font (underscore)
constexpr int NUM_CHARS = 64;    // Total characters in ROM
constexpr int ROM_SIZE = 512;    // Size of 2513 ROM dump (64 chars * 8 bytes)

// Character ROM data - loaded from file at initialization
// Indexed by character code (0x20-0x5F), one byte per scan line
extern std::array<std::array<uint8_t, CHAR_HEIGHT>, NUM_CHARS> CHAR_ROM;

// Load character ROM from file in native hardware order
// The 2513 ROM is addressed using the lower 6 bits of the ASCII character:
//   - ROM indices 0x00-0x1F: ASCII 0x40-0x5F (@ through _)
//   - ROM indices 0x20-0x3F: ASCII 0x20-0x3F (space through ?)
bool loadCharROM(const char* filename);

// Get bitmap data for a character
// Uses hardware-accurate addressing: lower 6 bits of ASCII character
// Returns pointer to 8 bytes of scan line data, or nullptr if out of range
inline const uint8_t* getCharData(uint8_t ch) {
    if (ch < FONT_START || ch > FONT_END) {
        return nullptr;
    }
    // Hardware addressing: use lower 6 bits (same as original Apple 1)
    return CHAR_ROM[ch & 0x3F].data();
}

// Check if a pixel is set in a character at given position
// x: 0-4 (left to right), y: 0-7 (top to bottom)
inline bool getPixel(uint8_t ch, int x, int y) {
    if (x < 0 || x >= CHAR_WIDTH || y < 0 || y >= CHAR_HEIGHT) {
        return false;
    }
    const uint8_t* data = getCharData(ch);
    if (!data) {
        return false;
    }
    // Bit 4 is leftmost pixel, bit 0 is rightmost
    return (data[y] & (1 << (4 - x))) != 0;
}
