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
#ifdef _WIN64
#include <windows.h>
#endif

class Cycles_t {
public:
Cycles_t() : _c(0), _emulateTiming(false),
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

	uint64_t get() {
		return _c;
	}

	Cycles_t& operator=(const uint64_t c) {
		_c = c;
		return *this;
	}
	
	bool operator==(const Cycles_t &rhs) {
		return _c == rhs._c;
	}
	
	Cycles_t operator++(int) {
		_c++;
		clockDelay();
		return *this;
	}

	Cycles_t& operator+=(uint64_t i) {
		_c += i;
		while (i--)
			clockDelay();
		return *this;
	}

	uint64_t operator-(Cycles_t const& c) {
		if (c._c > _c)
			return 0;
		return (unsigned char) (_c - c._c);
	}

	Cycles_t operator+(Cycles_t const& c) {
		Cycles_t res;
		res = _c + c._c;
		return res;
	}

	bool operator>(const Cycles_t &c)  {
		return _c > c._c;
	}

	bool operator>(uint64_t c) const {
		return _c > c;
	}

	bool operator<(const Cycles_t &c) {
		return _c < c._c;
	}

private:

#ifdef _WIN64
	void windowsDelay(unt64_t delayNs) {
		// Use the Windows high-performance counter to busy-wait a
		//certain number of nanoseconds.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		LARGE_INTEGER start;
		QueryPerformanceCounter(&start);
		LONGLONG target_counts = delayNs * 
			frequency.QuadPart / 1000000000LL;

		LARGE_INTEGER current;
		do {
			QueryPerformanceCounter(&current);
		} while (current.QuadPart - start.QuadPart < target_counts);
	}
#endif

#ifdef __linux__
	void linuxDelay(uint64_t delayNs) {
		// Use Linux clock_gettime() to busy-wait a certain number of 
		// nanoseconds.
		struct timespec start, current;
		clock_gettime(CLOCK_MONOTONIC, &start);

		int64_t target_ns = start.tv_nsec + delayNs;
		int64_t target_s = start.tv_sec + target_ns / 1000000000LL;
		target_ns = target_ns % 1000000000LL;

		do {
			clock_gettime(CLOCK_MONOTONIC, &current);
		} while (current.tv_sec < target_s || 
				(current.tv_sec == target_s && current.tv_nsec < target_ns));

	}
#endif

	void clockDelay() {
		if (!_emulateTiming)
			return;

#if defined(_WIN64)
		windowsDelay(_cycleDelay.count());		
#elif defined(__linux__)
		linuxDelay(_cycleDelay.count());
#else 

		// The minimum sleep_for() resolution (and clock_nanosleep())
		// seems to be about 1000 nanoseconds, or 1 microsecond.  That's
		// unfortunate as it means we're only ever going to be able to
		// emulate a 1 MHz 6502 using this method.  But it works as a 
		// fallback method.  
		std::this_thread::sleep_for(_cycleDelay);
#endif
	}

	static constexpr int nsInMHz = 1000;

	uint64_t _c;
	bool _emulateTiming;
	std::chrono::nanoseconds _cycleDelay;
};
