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

#include <memory>
#include <algorithm>
#include <fstream>
#include <fmt/core.h>

//
// Memory is a simple vector of Elements, where each element is a
// storage container for a Cell.  Elements are of four different
// types: Unmapped, RAM, ROM and MIO (memory-mapped I/O).  Unmapped
// elements are represented by the base Element class.  RAM, ROM and
// MIO are represented by their own derived class.
//
// Each element is organized and accessed via the Memory class.
// 

/////////
// Memory element base class, also indicates an unmapped memory cell.
template<class Cell>
class Element {
public:
	enum Type {RAM, ROM, MIO, UNMAPPED};

	virtual ~Element() { }

	virtual Cell Read() const {
		return 0;
	}

	virtual void Write([[maybe_unused]] const Cell b) { };

	virtual std::string type() const {
		return "Unmapped";
	}

	virtual Type getType() const {
		return UNMAPPED;
	}

	Element<Cell>& operator=(const Cell b) {
		this->Write(b);
		return *this;
	}

	bool operator==(const Element<Cell> &e) const {
		return Read() == e.Read();
	}

	bool operator==(const int i) const {
		return this->Read() == (Cell) i;
	}

private:

protected:
};

// RAM element
template<class Cell>
class RAM : public Element<Cell> {
public:
	RAM () { }

	RAM<Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	Cell Read() const override {
		return _cell;
	}

	void Write(const Cell b) override {
		_cell = b;
	}

	typename Element<Cell>::Type getType() const override {
		return Element<Cell>::RAM;
	}

	std::string type() const override{
		return "RAM";
	}

private:
	Cell _cell;
};

// ROM element
template<class Cell>
class ROM : public Element<Cell> {
public:
	
	ROM(Cell value) {
		_cell = value;
	}

	Cell Read() const override {
		return _cell;
	}

	void Write([[maybe_unused]] const Cell  b) override {
		;
	}

	typename Element<Cell>::Type getType() const override {
		return Element<Cell>::ROM;
	}

	std::string type() const override {
		return "ROM";
	}

private:
	Cell _cell;
};

// Memory mapped devices, ie. a keyboard and terminal.
template<class Cell>
class MIO : public Element<Cell> {
public:
	using readfn_t  = Cell (*)(void);
	using writefn_t = void (*)(Cell);

//	MIO() {	}

	MIO(readfn_t readfn = nullptr, writefn_t writefn = nullptr) :
		_readfn(readfn), _writefn(writefn) { }
	
	void setMIO(readfn_t readfn = nullptr, writefn_t writefn = nullptr) {
		_readfn = readfn;
		_writefn = writefn;
	}

	void Write(const Cell b) override {
		if (_writefn) 
			_writefn(b);
	}

	Cell Read() const override {
		if (_readfn) 
			return _readfn();
		return 0;
	}

	typename Element<Cell>::Type getType() const override {
		return Element<Cell>::MIO;
	}

	std::string type() const override {
		return "MIO";
	}

private:
	readfn_t _readfn;
	writefn_t _writefn;
};
	
/////////
// memory class
template<class Address = unsigned long, class Cell = unsigned char>
class Memory {
public:

	Memory(const Address endAddress) : _endAddress(endAddress) {
		uint64_t _size = _endAddress + 1;

		if (_size > _mem.max_size()) {
			auto s = fmt::format("End address {:#04x} exceeds host "
					     "system memory limits",
					     endAddress);
			exception(s);
		}
		_mem.reserve(_size);
		_watch.reserve(_size);

		_unmapped = std::make_shared<::Element<Cell>>();
		_mem.assign(_size, _unmapped);
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
			fmt::print("# mem[{:04x}] {:02x} -> {:02x}\n",
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
			(*it) = std::make_shared<::RAM<Cell>>();
		}

		return true;
	}

	bool mapROM(const Address start,
		    const std::vector<Cell> &rom,
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

		for (auto it = startIdx; it != endIdx; it++, i++) {
			(*it) = std::make_shared<::ROM<Cell>>(rom[i]);
		}

		return true;
	}
	
	bool mapMIO(const Address address,
		    const typename ::MIO<Cell>::readfn_t readfn,
		    const typename ::MIO<Cell>::writefn_t writefn,
		    const bool overwriteExistingElements = false) {

		boundsCheck(address);
		if (!overwriteExistingElements &&
		    addressRangeOverlapsExistingMap(address, address)) {
			auto s = fmt::format("Address {:x} overlaps with "
					     "existing map", address);
			exception(s);
			return false;
		}

		_mem[address] = std::make_shared<::MIO<Cell>>(readfn, writefn);
		return true;
	}

	bool isAddressMapped(const Address address) {
		return isAddressMapped(_mem[address]);
	}

	bool isAddressMapped(std::shared_ptr<Element<Cell>> e) {
		return e != _unmapped;
	}

	void hexdump(const Address start, Address end) {

		if (start > end || end > _endAddress) {
			fmt::print("# Invalid memory range\n");
			return;
		}

		fmt::print("# Memory Dump {:#04x}:{:#04x}\n", start, end);

		auto begin = _mem.begin() + start;
		auto stop = _mem.begin() + end; 
		int cnt = 0;
		std::string hexdump, ascii;
        for (auto it = begin; it <= stop; it++) {
			if (cnt == 0)
				hexdump += fmt::format("{:04x}  ",
					       std::distance(_mem.begin(), it));
			
			Cell c = (*it)->Read();
			hexdump += fmt::format("{:02x} ", c);
			if (isprint(c))
				ascii += c;
			else
				ascii += '.';

			cnt++;
			if (cnt == 16 || it == stop) {
				cnt = 0;
				fmt::print("{:58.58}{}\n", hexdump, ascii);
				hexdump = "";
				ascii = "";
			}
		}
	}

	void printMap() const {

		auto it = _mem.begin();
		auto range_start = it;
		auto range_end = it;

		fmt::print("Memory map:\n");

		while (it != _mem.end()) {
			auto next_it = std::next(it);
			if ((next_it == _mem.end()) ||
			  ((*next_it)->getType() != (*range_start)->getType())){

				range_end = it;
				Address bytes = 1 + (Address) 
					std::distance(range_start, range_end);
				Address start = (Address)
				       std::distance(_mem.begin(), range_start);
				Address end = (Address)
					std::distance(_mem.begin(), range_end);
				auto type = (*range_end)->type();

				fmt::print("{:#06x} - {:#04x} {} ({} bytes)\n",
					   start, end, type, bytes);

				range_start = next_it;
			}

			it = next_it;
		}
		auto mappedBytes = 
			std::count_if(_mem.begin(), _mem.end(),
				      [](std::shared_ptr<Element<Cell>> e) {
			    return e->getType() != Element<Cell>::UNMAPPED;
			});
				
		fmt::print("Total bytes mapped: {} bytes\n", mappedBytes);
		fmt::print("Total memory size : {} bytes\n", _mem.size());

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
			auto s = fmt::format("File {} not found", filename);
			exception(s);
		}
		
		// reserve capacity
		std::vector<Cell> vec;
		vec.reserve(fileSize);
		
		// read the data
		vec.insert(vec.begin(),
			   std::istream_iterator<unsigned char>(file),
			   std::istream_iterator<unsigned char>());
		loadData(vec, start);
	}

	void loadDataFromFile(std::string& filename, Address start) {
		loadDataFromFile(filename.c_str(), start);
	}
	
	void loadData(std::vector<Cell> &data, Address startAddress) {

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

		auto a = _mem.begin() + startAddress;
		for (auto i = data.begin(); i != data.end(); a++, i++)  {
			if ((*a)->getType() != Element<Cell>::RAM) {
				auto s = fmt::format("Data attempted to load "
						     "on non-RAM memory "
						     "location at address {:x}",
						     std::distance(_mem.begin(),
								   a));
				exception(s);
			}
			auto d = *i;
			(*a)->Write(d);
		}
	}

	// watch memory address
	
	void enableWatch(const Address address) {
		boundsCheck(address);
		
		_watch[address] = true;
	}

	bool watching(const Address address) const {
		boundsCheck(address);
		return _watch[address];
	}

	void listWatch() {
		fmt::print("# Watch list\n");

		for(Address addr = 0; addr <= _watch.size(); addr++) 
			if (_watch[addr])
				fmt::print("# {:#04x}\n", addr);
	}

	void clearWatch(Address address) {
		boundsCheck(address);
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
	std::shared_ptr<Element<Cell>> _unmapped;	   // Default memory element 

	std::vector<std::shared_ptr<Element<Cell>>> _mem;
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
		std::string error = "Memory Exception: " + message; 
		throw Exception(error);
	}
};
