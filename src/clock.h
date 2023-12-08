//
// Bus clock
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

#include <cstdint>
#include <chrono>

#ifdef _WIN64
#include <windows.h>
#endif

class BusClock_t {
public:
	BusClock_t() : _emulateTiming(false),
		_cycleDelay(std::chrono::nanoseconds(nsInMHz)) { } 

	int64_t delayTimeNs() {
		return _cycleDelay.count();
	}

	void enableTimingEmulation() {
		_emulateTiming = true;
	}
	
	void disableTimingEmulation() {
		_emulateTiming = false;
	}

	void toggleTimingEmulation() {
		_emulateTiming = !_emulateTiming;
	}

	bool timingEmulation() {
		return _emulateTiming;
	}

	void delay(uint64_t cycles) {
		if (!_emulateTiming)
			return;

		#if defined(_WIN64)
		windowsDelay(cycles);		
		#elif defined(__linux__)
		linuxDelay(cycles);
		#else 
		fallbackDelay(cycles);
		#endif
	}

private:
	static constexpr int nsInMHz = 1000;
	bool _emulateTiming;
	std::chrono::nanoseconds _cycleDelay;

#ifdef _WIN64
	void windowsDelay(uint64_t cycles) {
		// Use the Windows high-performance counter to busy-wait a
		//certain number of nanoseconds.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);
		LONGLONG target_counts = _cycleDelay.count() * 
			frequency.QuadPart / 1000000000LL;
		target_counts *= cycles;

		LARGE_INTEGER current;
		do {
			QueryPerformanceCounter(&current);
		} while (current.QuadPart - start.QuadPart < target_counts);
	}
#endif

#ifdef __linux__
	void linuxDelay(uint64_t cycles) {
		// Use Linux clock_gettime() to busy-wait a certain number of 
		// nanoseconds.
		struct timespec start, current;
		clock_gettime(CLOCK_MONOTONIC, &start);

		int64_t target_ns = start.tv_nsec + (_cycleDelay.count() * cycles);
		int64_t target_s = start.tv_sec + target_ns / 1000000000LL;
		target_ns = target_ns % 1000000000LL;

		do {
			clock_gettime(CLOCK_MONOTONIC, &current);
		} while (current.tv_sec < target_s || 
				(current.tv_sec == target_s && current.tv_nsec < target_ns));

	}
#endif

	void fallbackDelay(uint64_t cycles) {
		// The minimum sleep_for() resolution (and clock_nanosleep())
		// seems to be about 1000 nanoseconds, or 1 microsecond.  That's
		// unfortunate as it means we're only ever going to be able to
		// emulate a 1 MHz 6502 using this method.  But it works as a 
		// fallback method.  
		std::this_thread::sleep_for(_cycleDelay * cycles);
	}

};
