// Emulated Apple 1
//
// Copyright (C) 2023 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

// A platform specific message about what keys do what
extern std::string commandKeyBanner;

extern bool getch(char&);
extern void setupSignals(void);
extern void disable_raw_mode();
extern void enable_raw_mode();

// Uncomment to load Apple INTEGER BASIC
//#define APPLE_INTEGER_BASIC

//Uncomment to load Applesoft Basic Lite
#define APPLESOFT_BASIC_LITE

#if defined(APPLE_INTEGER_BASIC) && defined(APPLESOFT_BASIC_LITE)
# error "Can't have both Apple Integer Basic and Applesoft Basic loaded "\
	"at the same time"
#endif

// Load addresses and data locations for various built in programs
constexpr Address_t wozmonAddress = 0xff00;
static const char* WOZMON_FILE =
BINFILE_PATH "/wozmon.bin";

#ifdef APPLE_INTEGER_BASIC
constexpr Address_t appleBasicAddress = 0xe000;
static const char* APPLESOFT_BASIC_FILE =
BINFILE_PATH "Apple-1_Integer_BASIC.bin";
#endif

#ifdef APPLESOFT_BASIC_LITE
constexpr Address_t applesoftBasicLiteAddress = 0x6000;
static const char* APPLE_INTEGER_BASIC_FILE =
BINFILE_PATH "/applesoft-lite-0.4-ram.bin";
#endif

// bytecode for the sample program from the Apple 1 Manual
constexpr Address_t apple1SampleAddress = 0x0000;
std::vector<unsigned char> apple1SampleProg =
{ 0xa9, 0x00, 0xaa, 0x20, 0xef,0xff, 0xe8, 0x8a, 0x4c, 0x02, 0x00 };

// Memory-mapped IO address for Keyboard and display
constexpr Word KEYBOARD = 0xd010;
constexpr Word KEYBOARDCR = 0xd011;
constexpr Word DISPLAY = 0xd012;
constexpr Word DISPLAYCR = 0xd013;

// Apple 1 keycodes
constexpr char CR = 0x0d;
constexpr char BELL = 0x0a;
#ifdef _WIN64
constexpr char DEL = '\b';
#else
constexpr char DEL = 0x7f;
#endif
constexpr char CTRLA = 0x01;
constexpr char CTRLB = 0x02;
constexpr char CTRLC = 0x03;
constexpr char CTRLD = 0x04;
