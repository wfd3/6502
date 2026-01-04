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

#ifdef HAVE_SDL2

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

// SDL may define CHAR_WIDTH/CHAR_HEIGHT macros - undefine them before including apple1_font.h
#ifdef CHAR_WIDTH
#undef CHAR_WIDTH
#endif
#ifdef CHAR_HEIGHT
#undef CHAR_HEIGHT
#endif

#include "display_interface.h"
#include "keyboard_interface.h"
#include "apple1_font.h"
#include <queue>

// SDL2-based graphical display for Apple 1 emulator
// Renders 40x24 character display with authentic Apple 1 font
// Also handles keyboard input from the SDL window
class SDL2Display : public IDisplay, public IKeyboard {
public:
    // Display dimensions
    static constexpr int COLS = 40;
    static constexpr int ROWS = 24;

    // Character cell size (font is 5x8, but we use 7x8 for spacing)
    static constexpr int CELL_WIDTH = 7;
    static constexpr int CELL_HEIGHT = 8;

    // Pixel scale factor (makes characters larger and more visible)
    static constexpr int SCALE = 3;

    // Window dimensions
    static constexpr int WINDOW_WIDTH = COLS * CELL_WIDTH * SCALE;
    static constexpr int WINDOW_HEIGHT = ROWS * CELL_HEIGHT * SCALE;

    SDL2Display(bool blinkCursor = false);
    ~SDL2Display() override;

    // IDisplay interface implementation
    void writeChar(uint8_t c) override;
    void clear() override;
    void refresh() override;
    bool isReady() const override;

    // IKeyboard interface implementation
    bool getChar(uint8_t& ch) override;

    // SDL2-specific methods
    void handleEvents();  // Process SDL2 events (close window, keyboard, etc.)
    bool shouldQuit() const { return _quit; }

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _texture;

    // Screen buffer: 40x24 characters
    std::array<std::array<uint8_t, COLS>, ROWS> _screen;

    // Cursor position
    int _cursorX;
    int _cursorY;

    // Dirty flag for refresh optimization
    bool _dirty;

    // Quit flag
    bool _quit;

    // Keyboard input queue
    std::queue<uint8_t> _keyQueue;

    // Cursor blink state
    bool _blinkEnabled;
    uint32_t _lastBlink;
    bool _cursorVisible;

    // Internal helper methods
    void initSDL();
    void cleanupSDL();
    void renderScreen();
    void renderChar(uint8_t ch, int x, int y);
    void scrollUp();
    void advanceCursor();
    void handleCarriageReturn();
    void handleBackspace();
    void handlePaste();
};

#endif // HAVE_SDL2
