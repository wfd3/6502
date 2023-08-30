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

#include <fmt/core.h>

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
			auto s = fmt::format("End address {:#04x} exceeds host "
					     "system memory limits",
					     endAddress);
			exception(s);
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
			std::cout <<
				fmt::format("# mem[{:04x}] {:02x} -> {:02x}\n",
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
			auto s = fmt::format("Address range {:x}:{:x} overlaps "
					     "with existing map",
					     start, end);
			exception(s);
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
			auto s = fmt::format("Address range {:x}:{:x} overlaps "
					     "with existing map",
					     start, start + rom.size());
			exception(s);
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
			auto s = fmt::format("Address {:x} overlaps with "
					     "existing map", address);
			exception(s);
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

		std::cout << "# Memory Dump "
			  << fmt::format("{:#04x}:{:#04x}", start, end)
			  << std::endl;

		if (start > end || end > _endAddress) {
			std::cout << "# -- Invalid memory range" << std::endl;
			return;
		}

		for (unsigned long i = start; i <= end; i += 16) {
			std::string ascii;
			std::string hexdump;
			
			hexdump += fmt::format("{:04x}  ", i);
			
			for (unsigned long j = 0; j < 16; j++) {
				if (j + i > end) {
					hexdump += "   ";
					ascii += ' ';
					continue;
				}
				Cell c = _mem[i + j]->Read();
				hexdump += fmt::format("{:02x} ", c);
				if (isprint(c))
					ascii += c;
				else
					ascii += '.';
			}
			std::cout << hexdump << "  " << ascii << std::endl;
		}
	}

	void printMap() const {

		std::cout
			<< fmt::format("Memory size: {} bytes\n", _mem.size())
			<< std::endl;

		auto it = _mem.begin();
		auto range_start = it;
		auto range_end = it;

		std::cout << "Memory map:" << std::endl;

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

				std::cout << fmt::format("{:#04x}", start)
					  << " - "
					  << fmt::format("{:#04x}", end)
					  << " "
					  << (*range_end)->type().c_str()
					  << "( " << bytes << " bytes)"
					  << std::endl;
				range_start = next_it;
			}

			it = next_it;
		}
		unsigned long mappedBytes = 
			std::count_if(_mem.begin(), _mem.end(),
				      [](Element<Cell> *e) {
			    return e->getType() != Element<Cell>::UNMAPPED;
			});
				
		std::cout << "Total bytes mapped: " << mappedBytes << std::endl;
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
			auto s = fmt::format("Can't load file '{}': not found",
					     filename);
			exception(s);
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
		
		if (startAddress > _endAddress) {
			auto s = fmt::format("Data load address is not a valid "
					     "address: {:#04x}", startAddress);
			exception(s);
		}
		if (startAddress + data.size() - 1 > _endAddress) {
			auto s = fmt::format("Data will not fit into memory at "
					     "start address {:#04x} (data "
					     "length {} bytes)",
					     startAddress, data.size());
			exception(s);
		}

		// TODO: iterator
		// TODO: Make sure we're only loading data into RAM
		for (Address a = startAddress, i = 0; i < data.size();
		     a++, i++)  {
			
			if (_mem[a]->getType() != Element<Cell>::RAM) {
				auto s = fmt::format("Data attempted to load "
						     "on non-RAM memory "
						     "location at address {:x}",
						     startAddress + i);
				exception(s);
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
		std::cout << "# Watch list" << std::endl;

		for(Address addr = 0; addr <= _watch.size(); addr++) 
			if (_watch[addr])
				std::cout << fmt::format("# {:#04x}", addr)
					  << std::endl;
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
		if (!boundsCheckNoThrow(address)) {
			auto s = fmt::format("Address {:#04x} out of range",
					     address);
			exception(s);
		}
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

	void exception(const std::string &message) {
		std::string error = "Memory Exception: " + message + 
			"; Halting\n";
		throw Exception(error);
	}
};
