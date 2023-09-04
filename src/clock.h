#pragma once

class Cycles_t {
public:
	Cycles_t() {
		_c = 0;
		_emulateTiming = false;
		_cycleDelay = std::chrono::nanoseconds(nsInMHz); // 1us == 1MHz
	}

	unsigned long delayTimeNs() {
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

	unsigned long get() {
		return _c;
	}

	void operator=(const unsigned long c) {
		_c = c;
	}
	
	bool operator==(const Cycles_t &rhs) {
		return _c == rhs._c;
	}
	
	Cycles_t& operator++(int) {
		_c++;
		clockDelay();
		return *this;
	}

	Cycles_t& operator+=(int i) {
		_c += i;
		clockDelay();
		return *this;
	}

	unsigned char operator-(Cycles_t const& c) {
		if (c._c > _c)
			return 0;
		return _c - c._c;
	}

	Cycles_t operator+(Cycles_t const& c) {
		Cycles_t res;
		res = _c + c._c;
		return res;
	}

	bool operator>(const Cycles_t &c)  {
		return _c > c._c;
	}

	bool operator>(unsigned char c) const {
		return _c > c;
	}

	bool operator<(const Cycles_t &c) {
		return _c < c._c;
	}

private:
	void clockDelay() {
		// The minimum sleep_for() resolution (and clock_nanosleep())
		// seems to be about 1000 nanoseconds, or 1 microsecond.  That's
		// unfortnate as it means we're only ever going to be able to
		// emulate a 1 MHz 6502 using this method.
		if (_emulateTiming) {
			std::this_thread::sleep_for(_cycleDelay);
		}
	}

	static constexpr int nsInMHz = 1000;

	unsigned long _c;
	bool _emulateTiming;
	std::chrono::nanoseconds _cycleDelay;
};
