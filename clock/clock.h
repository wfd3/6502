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
#include <iostream>

using namespace std::chrono_literals;
using freq_t = uint16_t;

class BusClock_t {
public:
	BusClock_t(freq_t MHz) : _emulateTiming(true) {
		
		_MHz = _boundMHz(MHz);
		_calibrate();

		_nsPerCycle = _nsInCycleAt1MHz / _MHz;
		if (_nsPerCycle < _resolutionFloor) 
			_nsPerCycle = _resolutionFloor;
	 } 

	void enableTimingEmulation() {
		_emulateTiming = true;
	}
	
	void disableTimingEmulation() {
		_emulateTiming = false;
	}

	void delay(uint64_t cycles = 1) {
		if (_emulateTiming) {

			auto start = std::chrono::high_resolution_clock::now();
			auto end = start + (_nsPerCycle * cycles) - _calibration;

			while (std::chrono::high_resolution_clock::now() < end) 
				;
		}
	}

	freq_t getFrequencyMHz() { return _MHz; }

private:
	bool _emulateTiming;
	freq_t _MHz;
	std::chrono::duration<uint64_t, std::nano> _calibration;
	static constexpr std::chrono::nanoseconds _nsInCycleAt1MHz = 1000ns;
	static constexpr std::chrono::nanoseconds _resolutionFloor = 250ns;
	std::chrono::duration<uint64_t, std::nano> _nsPerCycle;

	freq_t _boundMHz(freq_t MHz) {
		if (MHz < 1)
			MHz = 1;
		else if (MHz >= 1000) 
			MHz = 1000;

		return MHz;
	}

	void _calibrate() {
		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		_calibration = end - start;
	}
};
