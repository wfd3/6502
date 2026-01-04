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

#ifdef HAVE_SDL2

#include "sdl2_display.h"
#include "control_codes.h"
#include <fmt/core.h>
#include <stdexcept>
#include <cctype>

SDL2Display::SDL2Display(bool blinkCursor)
    : _window(nullptr), _renderer(nullptr), _texture(nullptr),
      _cursorX(0), _cursorY(0), _dirty(true), _quit(false),
      _blinkEnabled(blinkCursor), _lastBlink(0), _cursorVisible(true) {

    // Initialize screen buffer with spaces
    for (auto& row : _screen) {
        row.fill(' ');
    }

    initSDL();
}

SDL2Display::~SDL2Display() {
    cleanupSDL();
}

void SDL2Display::initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(fmt::format("SDL_Init failed: {}", SDL_GetError()));
    }

    _window = SDL_CreateWindow(
        "Apple 1 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!_window) {
        SDL_Quit();
        throw std::runtime_error(fmt::format("SDL_CreateWindow failed: {}", SDL_GetError()));
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        SDL_DestroyWindow(_window);
        SDL_Quit();
        throw std::runtime_error(fmt::format("SDL_CreateRenderer failed: {}", SDL_GetError()));
    }

    // Create texture for rendering
    _texture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        COLS * CELL_WIDTH,
        ROWS * CELL_HEIGHT
    );

    if (!_texture) {
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
        throw std::runtime_error(fmt::format("SDL_CreateTexture failed: {}", SDL_GetError()));
    }

    // Set initial render color (green on black, classic Apple 1)
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
}

void SDL2Display::cleanupSDL() {
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    SDL_Quit();
}

void SDL2Display::writeChar(uint8_t c) {
    // Clear high bit (Apple 1 format)
    c &= 0x7f;

    // Handle special characters
    if (c == 0x0d) {  // Carriage return
        handleCarriageReturn();
    } else if (c == 0x5f) {  // Underscore = backspace
        handleBackspace();
    } else if (c == 0x0a) {  // Line feed = bell (ignore for now)
        // Could add audio beep here
    } else if (c >= 0x20 && c <= 0x7e) {  // Printable characters
        // Convert to uppercase (Apple 1 style)
        c = std::toupper(c);

        // Write to screen buffer
        _screen[_cursorY][_cursorX] = c;
        advanceCursor();
        _dirty = true;
    }
}

void SDL2Display::clear() {
    // Clear screen buffer
    for (auto& row : _screen) {
        row.fill(' ');
    }

    // Reset cursor
    _cursorX = 0;
    _cursorY = 0;
    _dirty = true;
}

void SDL2Display::refresh() {
    // Check if cursor blink needs updating (only if blinking is enabled)
    if (_blinkEnabled) {
        uint32_t now = SDL_GetTicks();
        if (now - _lastBlink > 500) {  // Blink every 500ms
            _cursorVisible = !_cursorVisible;
            _lastBlink = now;
            _dirty = true;  // Force redraw for cursor blink
        }
    }

    if (!_dirty) {
        return;
    }

    renderScreen();
    _dirty = false;
}

bool SDL2Display::isReady() const {
    return !_quit;
}

void SDL2Display::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            _quit = true;
            break;

        case SDL_KEYDOWN: {
            SDL_Keycode key = event.key.keysym.sym;
            SDL_Keymod mod = SDL_GetModState();

            // Handle control key combinations
            if (mod & KMOD_CTRL) {
                switch (key) {
                case SDLK_BACKSPACE:  // Ctrl-Backspace = Exit
                    _keyQueue.push(ControlCodes::EXIT);
                    break;
                case SDLK_BACKSLASH:  // Ctrl-\ = Reset
                    _keyQueue.push(ControlCodes::RESET);
                    break;
                case SDLK_LEFTBRACKET:  // Ctrl-[ = Clear screen
                    _keyQueue.push(ControlCodes::CLEAR_SCREEN);
                    break;
                case SDLK_RIGHTBRACKET:  // Ctrl-] = Debugger
                    _keyQueue.push(ControlCodes::DEBUGGER);
                    break;
                case SDLK_t:  // Ctrl-T = Toggle status display
                    _keyQueue.push(ControlCodes::STATUS);
                    break;
                case SDLK_c:  // Ctrl-C
                    _keyQueue.push(0x03);
                    break;
                case SDLK_v:  // Ctrl-V = Paste
                    handlePaste();
                    break;
                }
            } else {
                // Handle special keys that don't generate SDL_TEXTINPUT
                if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                    uint8_t ch = 0x0d | 0x80;  // Carriage return with high bit
                    _keyQueue.push(ch);
                } else if (key == SDLK_BACKSPACE || key == SDLK_DELETE) {
                    uint8_t ch = '_' | 0x80;  // Apple 1 backspace with high bit
                    _keyQueue.push(ch);
                }
                // All other regular keys will be handled by SDL_TEXTINPUT
            }
            break;
        }

        case SDL_TEXTINPUT: {
            // Handle regular text input (letters, numbers, symbols)
            // SDL_TEXTINPUT gives us the actual character including shift state
            char inputChar = event.text.text[0];
            if (inputChar >= 0x20 && inputChar <= 0x7e) {  // Printable ASCII
                uint8_t ch = std::toupper(inputChar);  // Apple 1 is uppercase only
                ch |= 0x80;  // Set high bit
                _keyQueue.push(ch);
            }
            break;
        }
        }
    }
}

bool SDL2Display::getChar(uint8_t& ch) {
    if (_keyQueue.empty()) {
        return false;
    }

    ch = _keyQueue.front();
    _keyQueue.pop();
    return true;
}

void SDL2Display::renderScreen() {
    // Set texture as render target
    SDL_SetRenderTarget(_renderer, _texture);

    // Clear texture with black
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);

    // Render all characters
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            uint8_t ch = _screen[y][x];
            if (ch != ' ') {  // Skip spaces for efficiency
                renderChar(ch, x, y);
            }
        }
    }

    // Render cursor (first character in ROM, which is '@' in the 2513)
    // Hardware-accurate: cursor uses ROM index 0
    if (_cursorVisible) {
        const uint8_t* cursorData = CHAR_ROM[0].data();
        if (cursorData) {
            // Render the cursor character directly using ROM index 0
            SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255);
            for (int row = 0; row < 8; row++) {
                uint8_t scanLine = cursorData[row];
                for (int col = 0; col < 5; col++) {
                    if (scanLine & (1 << (4 - col))) {
                        SDL_Rect pixel;
                        pixel.x = _cursorX * CELL_WIDTH + col;
                        pixel.y = _cursorY * CELL_HEIGHT + row;
                        pixel.w = 1;
                        pixel.h = 1;
                        SDL_RenderFillRect(_renderer, &pixel);
                    }
                }
            }
        }
    }

    // Reset render target to window
    SDL_SetRenderTarget(_renderer, nullptr);

    // Copy texture to window with scaling
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);
}

void SDL2Display::renderChar(uint8_t ch, int x, int y) {
    const uint8_t* fontData = getCharData(ch);
    if (!fontData) {
        return;  // Character not in font
    }

    // Set draw color to green (classic Apple 1 phosphor)
    SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255);

    // Render each pixel of the character
    for (int row = 0; row < 8; row++) {  // Render all 8 scan lines from the ROM
        uint8_t scanLine = fontData[row];

        for (int col = 0; col < 5; col++) {  // Font is 5 pixels wide
            // Check if pixel is set (bit 4 = leftmost, bit 0 = rightmost)
            if (scanLine & (1 << (4 - col))) {
                SDL_Rect pixel;
                pixel.x = x * CELL_WIDTH + col;
                pixel.y = y * CELL_HEIGHT + row;
                pixel.w = 1;
                pixel.h = 1;
                SDL_RenderFillRect(_renderer, &pixel);
            }
        }
    }
}

void SDL2Display::scrollUp() {
    // Move all rows up by one
    for (int y = 0; y < ROWS - 1; y++) {
        _screen[y] = _screen[y + 1];
    }

    // Clear bottom row
    _screen[ROWS - 1].fill(' ');
}

void SDL2Display::advanceCursor() {
    _cursorX++;
    if (_cursorX >= COLS) {
        _cursorX = 0;
        _cursorY++;
        if (_cursorY >= ROWS) {
            _cursorY = ROWS - 1;
            scrollUp();
        }
    }
}

void SDL2Display::handleCarriageReturn() {
    _cursorX = 0;
    _cursorY++;
    if (_cursorY >= ROWS) {
        _cursorY = ROWS - 1;
        scrollUp();
    }
    _dirty = true;
}

void SDL2Display::handleBackspace() {
    if (_cursorX > 0) {
        _cursorX--;
        _screen[_cursorY][_cursorX] = ' ';
        _dirty = true;
    }
}

void SDL2Display::handlePaste() {
    if (!SDL_HasClipboardText()) {
        return;
    }

    char* clipboardText = SDL_GetClipboardText();
    if (clipboardText == nullptr) {
        return;
    }

    // Process each character in the clipboard text
    for (char* ptr = clipboardText; *ptr != '\0'; ptr++) {
        char ch = *ptr;
        
        // Skip non-printable characters except for CR and LF
        if (ch < 0x20 && ch != 0x0A && ch != 0x0D) {
            continue;
        }
        
        // Convert line feeds to carriage returns (Apple 1 style)
        if (ch == 0x0A) {
            ch = 0x0D;
        }
        
        // Convert to uppercase (Apple 1 style)
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - 'a' + 'A';
        }
        
        // Only accept printable ASCII and carriage return
        if ((ch >= 0x20 && ch <= 0x7E) || ch == 0x0D) {
            // Set high bit (Apple 1 format) and add to queue
            uint8_t apple1_ch = static_cast<uint8_t>(ch) | 0x80;
            _keyQueue.push(apple1_ch);
        }
    }

    SDL_free(clipboardText);
}

#endif // HAVE_SDL2
