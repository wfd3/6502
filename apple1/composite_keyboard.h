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
#include <vector>

// Composite keyboard that checks multiple keyboard sources
// Useful for accepting input from both SDL2 window and terminal
class CompositeKeyboard : public IKeyboard {
public:
    CompositeKeyboard() = default;

    void addKeyboard(IKeyboard* keyboard) {
        _keyboards.push_back(keyboard);
    }

    bool getChar(uint8_t& ch) override {
        // Check all keyboard sources in order
        for (auto* keyboard : _keyboards) {
            if (keyboard->getChar(ch)) {
                return true;
            }
        }
        return false;
    }

    void setNonBlocking() override {
        for (auto* keyboard : _keyboards) {
            keyboard->setNonBlocking();
        }
    }

    void setBlocking() override {
        for (auto* keyboard : _keyboards) {
            keyboard->setBlocking();
        }
    }

private:
    std::vector<IKeyboard*> _keyboards;
};
