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

// Abstract interface for display output
// Allows multiple implementations (terminal, SDL2 window, etc.)
class IDisplay {
public:
    virtual ~IDisplay() = default;

    // Write a single character to the display
    // Character should already be in Apple 1 format (high bit set, uppercase)
    virtual void writeChar(uint8_t c) = 0;

    // Clear the entire display
    virtual void clear() = 0;

    // Refresh/update the display (for buffered implementations)
    // Called once per frame or when updates are ready
    virtual void refresh() = 0;

    // Query if display is ready for input (optional, for flow control)
    virtual bool isReady() const { return true; }
};

// Terminal-based display implementation using current fmt::print approach
class TerminalDisplay : public IDisplay {
public:
    void writeChar(uint8_t c) override;
    void clear() override;
    void refresh() override;
};
