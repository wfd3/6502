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
#include <algorithm>
#include <thread>

using namespace std::chrono_literals;
using freq_t = uint16_t;

class BusClock_t {
public:
	BusClock_t(freq_t MHz) : _emulateTiming(true) {
		_MHz = std::clamp(MHz, _MIN_MHz, _MAX_MHz);
		_cyclesInDelayTime = _delayMs.count() * _MHz * _cyclesPerMsAt1MHz;
		_accumulatedCycles = 0;
	 } 

	void enableTimingEmulation() {
		_emulateTiming = true;
		_accumulatedCycles = 0;
	}
	
	void disableTimingEmulation() {
		_emulateTiming = false;
		_accumulatedCycles = 0;
	}

	bool delay(const uint64_t cycles = 1) {
		_accumulatedCycles += cycles;

		if (_accumulatedCycles >= _cyclesInDelayTime) {
			_accumulatedCycles -= _cyclesInDelayTime;
			if (_emulateTiming) {
				std::this_thread::sleep_for(_delayMs);
				return true;
			}
		} 

		return false;
	}

	freq_t getFrequencyMHz() { return _MHz; }

	uint64_t getAccumulatedCycles() { return _accumulatedCycles; }

	std::chrono::milliseconds minimumDelayTime() { return _delayMs; }

	uint64_t getCyclesInDelayTime() { return _cyclesInDelayTime; }

private:
	bool _emulateTiming;
	freq_t _MHz;
	uint64_t _accumulatedCycles = 0;
	uint64_t _cyclesInDelayTime;

	static constexpr freq_t _MIN_MHz = 1;
	static constexpr freq_t _MAX_MHz = 1000;
	static constexpr uint64_t _cyclesPerMsAt1MHz = 1000;	
	static constexpr std::chrono::milliseconds _delayMs = 15ms; // needs to be large enough so that ::sleep_for() is reasonably precise 
};
