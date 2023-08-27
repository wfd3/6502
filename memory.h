//
// Memory and memory element classes for emulated CPUs
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

# pragma once

#include <vector>
#include <algorithm>
#include <stdarg.h>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <stdexcept>

/////////
// Memory element class
class Element {
public:
	enum Type {RAM, ROM, MIO, UNMAPPED};

	virtual ~Element() { }

	virtual Element& operator=(const char b) {
		this->Write(b);
		return *this;
	}	

	virtual unsigned char Read() const = 0;

	virtual void Write(const unsigned char b) = 0;

	virtual Element& operator=(const unsigned char b) {
		this->Write(b);
		return *this;
	}

	virtual Element& operator=(const int i) {
		this->Write(i & 0xff);
		return *this;
	}

	virtual Element& operator=(const long unsigned int i) {
		this->Write(i & 0xff);
		return *this;
	}

	virtual bool operator==(const Element &e) const {
		return Read() == e.Read();
	}

	virtual bool operator==(const int i) const {
		return this->Read() == i;
	}

	virtual bool operator==(const unsigned int i) const {
		return this->Read() == i;
	}

	virtual bool operator==(const unsigned char i) const {
		return this->Read() == i;
	}

	operator char() const {
		return this->Read();
	}

	operator unsigned char() const {
		return this->Read();
	}

	operator int() const {
		return this->Read();
	}

	operator long() const {
		return this->Read();
	}
	
	operator unsigned long() const {
		return this->Read();
	}

	std::string type() const {
		switch (this->_type) {
		case RAM:      return "RAM";
		case ROM:      return "ROM";
		case MIO:      return "Memory Mapped I/O";
		case UNMAPPED: return "Unmapped";
		}
		return "Error - bad memory type";
	}

	Type getType() const {
		return this->_type;
	}

private:

protected:
	Type _type;
};


class RAM : public Element {
public:
	RAM () {
		_type = Element::RAM;
	}
	RAM& operator=(const int i) {
		Write(i);
		return *this;
	}

	unsigned char Read() const {
		return _byte;
	}

	void Write(const unsigned char b) {
		_byte = b;
	}

private:
	unsigned char _byte;
};

class ROM : public Element {
public:
	
	ROM(unsigned char value) {
		_byte = value;
		_type = Element::ROM;
	}

	unsigned char Read() const {
		return _byte;
	}

	void Write(const unsigned char  b) {
		(void) b;	// Suppress -Wall warning
		;
	}

private:
	unsigned char _byte;
};

// Memory mapped devices, ie. a keyboard and termial.
class MIO : public Element {
public:
	using readfn_t  = unsigned char (*)(void);
	using writefn_t = void (*)(unsigned char);

	MIO() { _type = Element::MIO; }

	MIO(readfn_t readfn, writefn_t writefn) {
		setMIO(readfn, writefn);
		_type = Element::MIO;
	}
	
	void setMIO(readfn_t readfn, writefn_t writefn) {
		_readfn = readfn;
		_writefn = writefn;
	}

	void Write(const unsigned char b) {
		if (_writefn) 
			_writefn(b);
	}

	unsigned char Read() const {
		if (_readfn) 
			return _readfn();
		return 0;
	}

	MIO& operator=(const int i) {
		this->Write(i & 0xff);
		return *this;
	}

private:
	readfn_t _readfn;
	writefn_t _writefn;
};

class Unmapped : public Element {
public:
	Unmapped() { _type = Element::UNMAPPED; }

	void Write(const unsigned char b) {
		(void) b;	// Suppress -Wall warnings
		;
	}

	unsigned char Read() const {
		return 0;
	}

	Unmapped& operator=(const int i) {
		(void) i;	// Suppress -Wall warning
		return *this;
	}

private:
};

	
/////////
// memory class
class Memory {
public:

	Memory(const size_t endAddress) {
		_endAddress = endAddress;
		auto _size = _endAddress + 1;

		_mem.reserve(_size);
		_watch.reserve(_size);

		_mem.assign(_size, &_unmapped);
		_watch.assign(_size, false);
	}

	unsigned long size() {
		return _mem.size();
	}

	unsigned char Read(const unsigned long address) {
		boundsCheck(address);
		return _mem.at(address)->Read();
	}

	void Write(const unsigned long address, const unsigned char byte) {
		boundsCheck(address);
		if (_watch[address]) {
			printf("# mem[%04lx] %02x -> %02x\n",
			       address, _mem[address]->Read(), byte);
		}
		
		_mem.at(address)->Write(byte);
	}
	
	Element& operator[](const size_t address) {
		boundsCheck(address);
		auto &e = _mem.at(address);
		return *e;
	}

	bool mapRAM(const unsigned long start, const unsigned long end) {
		boundsCheck(end);
		if (addressRangeOverlapsExistingMap(start, end)) {
			exception("Address range %lx:%lx overlaps with "
				  "existing map", start, end);
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + end;
		for (auto it = startIdx; it <= endIdx; it++) {
			(*it) = new ::RAM;
		}

		return true;
	}

	bool mapROM(const unsigned long start,
		    std::vector<unsigned char> &rom) {
		boundsCheck(start + rom.size());
		if (addressRangeOverlapsExistingMap(start, start+rom.size())) {
			exception("Address range %lx:%lx overlaps with "
				  " existing map", start, start + rom.size());
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + rom.size();
		unsigned long i = 0;

		for (auto it = startIdx; it <= endIdx; it++, i++)
			(*it) = new ::ROM(rom[i]);

		return true;
	}
	
	bool mapMIO(const unsigned long address,
		    const ::MIO::readfn_t readfn,
		    const ::MIO::writefn_t writefn) {

		boundsCheck(address);
		if (addressRangeOverlapsExistingMap(address, address)) {
			exception("Address %lx overlaps with existing map",
				  address);
			return false;
		}

		_mem[address] = new ::MIO(readfn, writefn);
		return true;
	}

	bool isAddressMapped(unsigned long address) {
		return isAddressMapped(_mem[address]);
	}

	bool isAddressMapped(Element *e) {
		return e != &_unmapped;
	}

	void hexdump(unsigned long start, size_t end) {

		printf("# Memory Dump 0x%04lx:0x%04lx\n", start, end);

		if (start > end || end > _endAddress) {
			printf("# -- Invalid memory range\n");
			return;
		}

		for (unsigned long i = start; i <= end; i += 16) {
			std::string ascii;
			std::string hexdump;
			
			hexdump += vformat("%04x  ", i);
			
			for (unsigned long j = 0; j < 16; j++) {
				if (j + i > end) {
					hexdump += "   ";
					ascii += ' ';
					continue;
				}
				unsigned char c = _mem[i + j]->Read();
				hexdump += vformat("%02x ", c);
				if (isprint(c))
					ascii += c;
				else
					ascii += '.';
			}
			printf("%s  %s\n", hexdump.c_str(), ascii.c_str());
		}
	}

	void printMap() {

		printf("Memory size: %ld bytes\n", _mem.size());

		auto it = _mem.begin();
		auto range_start = it;
		auto range_end = it;

		printf("Memory map:\n");
		unsigned long mappedBytes = 0;

		while (it != _mem.end()) {
			auto next_it = std::next(it);
			if ((next_it == _mem.end()) ||
			  ((*next_it)->getType() != (*range_start)->getType())){

				range_end = it;
				unsigned long bytes = 1 +
					std::distance(range_start, range_end);
				unsigned long start =
				       std::distance(_mem.begin(), range_start);
				unsigned long end =
					std::distance(_mem.begin(), range_end);

				printf("%04lx - %04lx %s (%ld bytes)\n",
				       start, end, (*range_end)->type().c_str(),
				       bytes)
					;

				range_start = next_it;
			}

			it = next_it;
		}
		mappedBytes =
			std::count_if(_mem.begin(), _mem.end(), [](Element *e) {
				return e->getType() != Element::UNMAPPED;
			});
				
		printf("Total bytes mapped: %ld\n", mappedBytes); 
	}



	// Loading data into memory

	void loadDataFromFile(const char *filename, unsigned long start) {

		std::ifstream file(filename, std::ios::binary);

		file.unsetf(std::ios::skipws);
		
		std::streampos fileSize;
		
		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (fileSize == -1) {
			exception("Can't load file '%s': not found\n",
				  filename);
		}
		
		// reserve capacity
		std::vector<unsigned char> vec;
		vec.reserve(fileSize);
		
		// read the data:
		vec.insert(vec.begin(),
			   std::istream_iterator<unsigned char>(file),
			   std::istream_iterator<unsigned char>());
		loadData(vec, start);
	}
	
	void loadData(std::vector<unsigned char> &data,
		      unsigned long startAddress) {
		
		if (startAddress > _endAddress)
			exception("Data load address is not a valid "
				  "address: 0x%04x", startAddress);
		if (startAddress + data.size() - 1 > _endAddress)
			exception("Data will not fit into memory at start "
				  "address 0x%04x (data length %d bytes)",
				  startAddress, data.size());

		for (size_t i = 0; i < data.size(); i++) 
			_mem[startAddress + i]->Write(data[i]);
	}

	// watch memory address
	
	void enableWatch(unsigned long address) {
		_watch[address] = true;
	}

	bool watching(unsigned long address) {
		return _watch[address];
	}

	void listWatch() {
		printf("# Watch list\n");

		for(unsigned long addr = 0; addr <= _watch.size(); addr++) 
			if (_watch[addr])
				printf("# e%04lx\n", addr);
	}

	void clearWatch(unsigned long address) {
		_watch[address] = false;
	}

	class Exception : public std::exception {
	public:
		Exception(const std::string& msg) : message(msg) {}
		
		const char* what() const noexcept override {
			return message.c_str();
		}
		
	private:
		std::string message;
	};

private:

	unsigned long _endAddress; // Last address
	::Unmapped _unmapped;	   // Default memory element 

	std::vector<Element *> _mem;
	std::vector<bool> _watch; // Vector of watched addresses.

	void boundsCheck(unsigned long address) {
		if (address > _endAddress) 
			exception("Address 0x%04x out of range", address);
	}

	bool boundsCheckNoThrow(unsigned long address) {
		return (address <= _endAddress);
	}

	bool addressRangeOverlapsExistingMap(unsigned long start,
					     unsigned long end) {
		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + end;
		for (auto it = startIdx; it <= endIdx; it++) {
			if (isAddressMapped((*it)))
				return true;
		}
		return false;
	}

	static const std::string vformat(const char * const zcFormat, ...) {
		va_list vaArgs;
		va_start(vaArgs, zcFormat);
		
		va_list vaArgsCopy;
		va_copy(vaArgsCopy, vaArgs);
		const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
		va_end(vaArgsCopy);
		
		std::vector<char> zc(iLen + 1);
		std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
		va_end(vaArgs);

		return std::string(zc.data(), iLen);
	}

	void exception(const char *zcFormat, ...) {
		va_list vaArgs;
		va_start(vaArgs, zcFormat);
		
		va_list vaArgsCopy;
		va_copy(vaArgsCopy, vaArgs);
		const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
		va_end(vaArgsCopy);

		std::vector<char> zc(iLen + 1);
		std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
		va_end(vaArgs);

		std::string formatted(zc.data(), iLen);
		std::string error = "Memory Exception: " + formatted + "\n" +
			"Halting\n";
		throw Exception(error);
	}
};
