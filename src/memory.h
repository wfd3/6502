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

#pragma once

#include <vector>
#include <algorithm>
#include <stdarg.h>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <stdexcept>

/////////
// Memory element base class
template<class Cell>
class Element {
public:
	enum Type {RAM, ROM, MIO, UNMAPPED};

	virtual ~Element() { }

	virtual Cell Read() const = 0;

	virtual void Write(const Cell b) = 0;

	virtual Element<Cell>& operator=(const Cell b) {
		this->Write(b);
		return *this;
	}	

	virtual Element<Cell>& operator=(const int i) {
		this->Write(i & 0xff);
		return *this;
	}

	virtual Element<Cell>& operator=(const long unsigned int i) {
		this->Write(i & 0xff);
		return *this;
	}

	virtual bool operator==(const Element<Cell> &e) const {
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

// RAM element
template<class Cell>
class RAM : public Element<Cell> {
public:
	RAM () {
		this->_type = Element<Cell>::RAM;
	}

	RAM<Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	RAM<Cell>& operator=(const long unsigned int i) {
		Write(i);
		return *this;
	}

	RAM<Cell>& operator=(const int i) {
		Write(i);
		return *this;
	}

	Cell Read() const {
		return _cell;
	}

	void Write(const Cell b) {
		_cell = b;
	}

private:
	Cell _cell;
};

// ROM elemenet
template<class Cell>
class ROM : public Element<Cell> {
public:
	
	ROM(Cell value) {
		_cell = value;
		this->_type = Element<Cell>::ROM;
	}

	Cell Read() const {
		return _cell;
	}

	void Write(const Cell  b) {
		(void) b;	// Suppress -Wall warning
		;
	}

	ROM<Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	ROM<Cell>& operator=(const long unsigned int i) {
		Write(i);
		return *this;
	}

	ROM<Cell>& operator=(const int i) {
		Write(i);
		return *this;
	}

private:
	Cell _cell;
};

// Memory mapped devices, ie. a keyboard and termial.
template<class Cell>
class MIO : public Element<Cell> {
public:
	using readfn_t  = Cell (*)(void);
	using writefn_t = void (*)(Cell);

	MIO() {
		this-> _type = Element<Cell>::MIO;
	}

	MIO(readfn_t readfn, writefn_t writefn) {
		setMIO(readfn, writefn);
		this->_type = Element<Cell>::MIO;
	}
	
	void setMIO(readfn_t readfn = NULL, writefn_t writefn = NULL) {
		_readfn = readfn;
		_writefn = writefn;
	}

	void Write(const Cell b) {
		if (_writefn) 
			_writefn(b);
	}

	Cell Read() const {
		if (_readfn) 
			return _readfn();
		return 0;
	}

	MIO<Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	MIO<Cell>& operator=(const long unsigned int i) {
		Write(i);
		return *this;
	}

	MIO<Cell>& operator=(const int i) {
		Write(i);
		return *this;
	}

private:
	readfn_t _readfn;
	writefn_t _writefn;
};

// An unmapped address; there should only be one of these.
template<class Cell>
class Unmapped : public Element<Cell> {
public:
	Unmapped() {
		this->_type = Element<Cell>::UNMAPPED;
	}

	void Write(const Cell b) {
		(void) b;	// Suppress -Wall warnings
		;
	}

	Cell Read() const {
		return 0;
	}

	Unmapped<Cell>& operator=(const int i) {
		(void) i;	// Suppress -Wall warning
		return *this;
	}

	Unmapped<Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	Unmapped<Cell>& operator=(const long unsigned int i) {
		Write(i);
		return *this;
	}

	
private:
};

	
/////////
// memory class
template<class Address = unsigned long, class Cell = unsigned char>
class Memory {
public:

	Memory(const Address endAddress) {
		_endAddress = endAddress;
		auto _size = _endAddress + 1;

		if (_size > _mem.max_size()) {
			exception("End address %04lx exceeds host system "
				  "memory limits", endAddress);
		}
		_mem.reserve(_size);
		_watch.reserve(_size);

		_mem.assign(_size, &_unmapped);
		_watch.assign(_size, false);
	}

	size_t size() {
		return _mem.size();
	}

	Cell Read(const Address address) {
		boundsCheck(address);
		return _mem.at(address)->Read();
	}

	void Write(const Address address, const Cell l) {
		boundsCheck(address);
		if (_watch[address]) {
			printf("# mem[%04x] %02x -> %02x\n",
			       address, _mem[address]->Read(), l);
		}
		
		_mem.at(address)->Write(l);
	}
	
	Element<Cell>& operator[](Address address) {
		boundsCheck(address);
		auto &e = _mem.at(address);
		return *e;
	}

	bool mapRAM(const Address start, const Address end) {
		boundsCheck(end);
		if (addressRangeOverlapsExistingMap(start, end)) {
			exception("Address range %lx:%lx overlaps with "
				  "existing map", start, end);
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + end;
		for (auto it = startIdx; it <= endIdx; it++) {
			(*it) = new ::RAM<Cell>;
		}

		return true;
	}

	bool mapROM(const Address start,
		    const std::vector<unsigned char> &rom,
		    const bool overwriteExistingElements = false) {
		boundsCheck(start + rom.size());
		if (!overwriteExistingElements &&
		    addressRangeOverlapsExistingMap(start, start+rom.size())) {
			exception("Address range %lx:%lx overlaps with "
				  " existing map", start, start + rom.size());
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + rom.size();
		unsigned long i = 0;

		for (auto it = startIdx; it <= endIdx; it++, i++) {
			if ((*it) != &_unmapped)
				delete (*it);
			(*it) = new ::ROM<Cell>(rom[i]);
		}

		return true;
	}
	
	bool mapMIO(const Address address,
		    const ::MIO<Cell>::readfn_t readfn,
		    const ::MIO<Cell>::writefn_t writefn,
		    const bool overwriteExistingElements = false) {

		boundsCheck(address);
		if (!overwriteExistingElements &&
		    addressRangeOverlapsExistingMap(address, address)) {
			exception("Address %lx overlaps with existing map",
				  address);
			return false;
		}

		if (_mem[address] != &_unmapped)
			delete _mem[address];
		_mem[address] = new ::MIO<Cell>(readfn, writefn);
		return true;
	}

	bool isAddressMapped(const Address address) {
		return isAddressMapped(_mem[address]);
	}

	bool isAddressMapped(Element<Cell> *e) {
		return e != &_unmapped;
	}

	// TODO: Remove for() loops by address
	void hexdump(const Address start, Address end) {

		printf("# Memory Dump 0x%04x:0x%04x\n", start, end);

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
				Cell c = _mem[i + j]->Read();
				hexdump += vformat("%02x ", c);
				if (isprint(c))
					ascii += c;
				else
					ascii += '.';
			}
			printf("%s  %s\n", hexdump.c_str(), ascii.c_str());
		}
	}

	void printMap() const {

		printf("Memory size: %ld bytes\n", _mem.size());

		auto it = _mem.begin();
		auto range_start = it;
		auto range_end = it;

		printf("Memory map:\n");

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
		unsigned long mappedBytes = 
			std::count_if(_mem.begin(), _mem.end(),
				      [](Element<Cell> *e) {
			    return e->getType() != Element<Cell>::UNMAPPED;
			});
				
		printf("Total bytes mapped: %ld\n", mappedBytes); 
	}


	// Loading data into memory

	void loadDataFromFile(const char *filename, Address start) {

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
		std::vector<Cell> vec;
		vec.reserve(fileSize);
		
		// read the data:
		vec.insert(vec.begin(),
			   std::istream_iterator<unsigned char>(file),
			   std::istream_iterator<unsigned char>());
		loadData(vec, start);
	}
	
	void loadData(std::vector<Cell> &data,
		      Address startAddress) {
		
		if (startAddress > _endAddress)
			exception("Data load address is not a valid "
				  "address: 0x%04x", startAddress);
		if (startAddress + data.size() - 1 > _endAddress)
			exception("Data will not fit into memory at start "
				  "address 0x%04x (data length %d bytes)",
				  startAddress, data.size());

		// TODO: iterator
		// TODO: Make sure we're only loading data into RAM
		for (Address a = startAddress, i = 0; i < data.size();
		     a++, i++)  {
			
			if (_mem[a]->getType() != Element<Cell>::RAM) {
					exception("Data attempted to load on non-RAM memory location at address %x\n", startAddress + i);
			}

			_mem[a]->Write(data[i]);
		}
	}

	// watch memory address
	
	void enableWatch(const Address address) {
		_watch[address] = true;
	}

	bool watching(const Address address) const {
		return _watch[address];
	}

	void listWatch() {
		printf("# Watch list\n");

		for(Address addr = 0; addr <= _watch.size(); addr++) 
			if (_watch[addr])
				printf("# e%04x\n", addr);
	}

	void clearWatch(Address address) {
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

	Address _endAddress; // Last address
	::Unmapped<Cell> _unmapped;	   // Default memory element 

	std::vector<Element<Cell> *> _mem;
	std::vector<bool> _watch; // Vector of watched addresses.

	void boundsCheck(Address address) {
		if (!boundsCheckNoThrow(address))
			exception("Address 0x%04x out of range", address);
	}

	bool boundsCheckNoThrow(Address address) {
		return (address <= _endAddress);
	}

	bool addressRangeOverlapsExistingMap(Address start, Address end) {
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
