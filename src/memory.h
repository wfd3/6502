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

#include <set>
#include <algorithm>
#include <fstream>
#include <memory>
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
template<class Address, class Cell>
class Element {
public:
	virtual ~Element() { }

	virtual Cell Read([[maybe_unused]] const Address address) { 
		return 0; 
	}
	
	virtual void Write([[maybe_unused]] const Address address,
		[[maybe_unused]] const Cell b) { 
			return;
	};

	virtual std::string type() const {
		return "Unmapped";
	}
};

// RAM element
template<class Address, class Cell>
class RAM : public Element<Address, Cell> {
public:
	RAM() { 
		_cell = 0;
	}

	RAM<Address, Cell>& operator=(const Cell i) {
		Write(i);
		return *this;
	}

	Cell Read([[maybe_unused]] const Address address) override {
		return _cell;
	}

	void Write([[maybe_unused]] const Address address,
		const Cell b) override {
		_cell = b;
	}

	std::string type() const override{
		return "RAM";
	}

private:
	Cell _cell;
};

// ROM element
template<class Address, class Cell>
class ROM : public Element<Address, Cell> {
public:
	
	ROM(Cell value) {
		_cell = value;
	}

	Cell Read([[maybe_unused]] const Address address) override {
		return _cell;
	}

	void Write([[maybe_unused]] const Address address,
		[[maybe_unused]] const Cell  b) override {
		;
	}

	std::string type() const override {
		return "ROM";
	}

private:
	Cell _cell;
};

// Memory mapped devices, ie. a keyboard and terminal.  This 
// class is intended to be used with regular function pointers
// to implement the device logic.
template<class Address, class Cell>
class MIO : public Element<Address, Cell> {
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

	void Write([[maybe_unused]] const Address address,
		const Cell b) override {
		if (_writefn) 
			_writefn(b);
	}

	Cell Read([[maybe_unused]] const Address address) override {
		if (_readfn) 
			return _readfn();
		return 0;
	}

	std::string type() const override {
		return "MIO";
	}

private:
	readfn_t _readfn;
	writefn_t _writefn;
};

class Device {
public:
	// These should be in order of precedence 
	enum Lines {
		Exit,
		Reset,
		Debug,
		None
	};

	using BusSignals = std::vector<Lines>;

	virtual BusSignals housekeeping() { return {}; }
	virtual void enable()       { _active = true;  }
	virtual void disable()      { _active = false; }
	virtual bool isActive()     { return _active ; }
	
	#if 0
	// TODO: Fix this.
	// When the Bus class runs these by directly calling b->setup(), make them virtual functions.
	// Until then, just leave these ifdef'd out and let MemMappedDevice handle it.

	// Run before and after emulation commences.  Use for things like setting up and 
	// resetting terminal settings, etc. 
	virtual void setup() { }
	virtual void teardown() { };
	#endif

private:
	bool _active = false;
};

// Memory mapped Device class.  A more idiomatic version of the MIO 
// Class above.
template<class Address, class Cell>
class MemMappedDevice : public Device, public Element<Address, Cell> {
public:

	MemMappedDevice() {}
	MemMappedDevice(std::vector<Address>& addresses) {
		for (auto& a : addresses) {
			_addresses.insert(a);
		}
	}
	MemMappedDevice(std::initializer_list<Address> a) : _addresses(a.begin(), a.end()) {}
	virtual ~MemMappedDevice() {}

	void addAddress(Address a) { 
		_addresses.insert(a);
	}

	bool validAddress(Address address) const {
		return _addresses.find(address) != _addresses.end();

	}

	const std::set<Address>& getAddressSet() const {
		return _addresses;
	}

protected:
	std::set<Address> _addresses;
};

/////////
// memory class
template<class Address = unsigned long, class Cell = unsigned char>
class Memory {
public:
	class MemoryProxy {
	private:
		Memory<Address, Cell>& mem;
		Address addr;

	public:
		MemoryProxy(Memory<Address, Cell>& m, Address a) : mem(m), addr(a) {}

		// For assignment operations like mem[0x10] = value
		MemoryProxy& operator=(const Cell& value) {
			mem.Write(addr, value);
			return *this;
		}

		// For reading operations like value = mem[0x10]
		operator Cell() const {
			return mem.Read(addr);
		}
	};
	
	class iterator {
		private:
			Memory* mem;
			Address addr;
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = Cell;
			using pointer = Cell*;
			using reference = MemoryProxy;
			using iterator_category = std::random_access_iterator_tag;

			iterator(Memory* m, Address a) : mem(m), addr(a) {}

			reference operator*() {
				return (*mem)[addr];
			}

			iterator& operator++() {
				++addr;
				return *this;
			}

			iterator operator++(int) {
				iterator tmp(*this);
				++addr;
				return tmp;
			}

			bool operator==(const iterator& other) const {
				return mem == other.mem && addr == other.addr;
			}

			bool operator!=(const iterator& other) const {
				return !(*this == other);
			}
	}; // class Memory::iterator

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, _mem.size());
    }

	Memory(const Address endAddress) : _endAddress(endAddress) {
		uint64_t _size = _endAddress + 1;

		if (_size > _mem.max_size()) {
			auto s = fmt::format("End address {:{}x} exceeds host "
					     "system memory limits",
					     endAddress, AddressWidth);
			exception(s);
		}
		_mem.reserve(_size);
		_watch.reserve(_size);

		_unmapped = std::make_shared<::Element<Address, Cell>>();
		_mem.assign(_size, _unmapped);
		_watch.assign(_size, false);
	}

	size_t size() {
		return _mem.size();
	}

	std::vector<Address> find(std::string sequence, Cell filter = -1 ) {
		std::vector<Address> positions;

        if(sequence.size() > _mem.size())
            return positions; 
        
       for (size_t i = 0; i <= _mem.size() - sequence.size(); ++i) {
            bool matches = true;
            for (size_t j = 0; j < sequence.size(); ++j) {
                if (sequence[j] != (_mem[i + j]->Read(i + j) & filter)) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                positions.push_back(i);
            }
        }

        return positions;
	}

	Cell Read(const Address address) {
		boundsCheck(address);
		return _mem.at(address)->Read(address);
	}

	void Write(const Address address, const Cell l) {
		boundsCheck(address);
		if (_watch[address]) {
			fmt::print("mem[{:{}x}] {:{}x} -> {:{}x}\n",
				   address, AddressWidth, _mem.at(address)->Read(address), 
				   CellWidth, l, CellWidth);
		}

		_mem.at(address)->Write(address, l);
	}

	void assign(const Address a1, const Address a2, const Cell value) {
			for (uint32_t a = a1; a <= a2; a++) {
				_mem.at(a)->Write(a, value);
			}
	}

	MemoryProxy operator[](Address address) {
        return MemoryProxy(*this, address);
    }

	bool mapRAM(const Address start, const Address end) {
		boundsCheck(end);
		if (addressRangeOverlapsExistingMap(start, end)) {
			auto s = fmt::format("Address range {:{}x}:{:{}x} overlaps "
					     "with existing map",
					     start, AddressWidth, end, AddressWidth);
			exception(s);
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + end;
		for (auto it = startIdx; it <= endIdx; it++) {
			(*it) = std::make_shared<::RAM<Address,Cell>>();
		}

		return true;
	}

	bool mapROM(const Address start,
		    const std::vector<Cell> &rom,
		    const bool overwriteExistingElements = true) {
		
		boundsCheck(start + rom.size());
		if (!overwriteExistingElements &&
		    addressRangeOverlapsExistingMap(start, start+rom.size())) {
			auto s = fmt::format("Address range {:{}x}:{:{}x} overlaps "
					     "with existing map",
					     start, AddressWidth, start + rom.size(), AddressWidth);
			exception(s);
			return false;
		}

		auto startIdx = _mem.begin() + start;
		auto endIdx   = _mem.begin() + rom.size();
		unsigned long i = 0;

		for (auto it = startIdx; it != endIdx; it++, i++) {
			(*it) = std::make_shared<::ROM<Address,Cell>>(rom[i]);
		}

		return true;
	}
	
	bool mapMIO(const Address address,
		    const typename ::MIO<Address,Cell>::readfn_t readfn,
		    const typename ::MIO<Address,Cell>::writefn_t writefn,
		    const bool overwriteExistingElements = true) {

		boundsCheck(address);
		if (!overwriteExistingElements &&
		    addressRangeOverlapsExistingMap(address, address)) {
			auto s = fmt::format("Address {:{}x} overlaps with "
					     "existing map", address, AddressWidth);
			exception(s);
			return false;
		}

		_mem[address] = std::make_shared<::MIO<Address,Cell>>(readfn, writefn);
		return true;
	}

	bool mapDevice(std::shared_ptr<MemMappedDevice<Address,Cell>> device, 
		const bool overwriteExistingElements = true) {

		for (const auto& a : device->getAddressSet()) {
			boundsCheck(a);
			if (!overwriteExistingElements && isAddressMapped(a)) {
				auto s = fmt::format("Address {:{}x} overlaps with existing map", 
					a, AddressWidth); 
				exception(s);
				return false;
			}
			_mem[a] = device;
		}
		return true;
	}

	bool isAddressMapped(const Address address) const {
		return isAddressMapped(_mem[address]);
	}

	bool isAddressMapped(const std::shared_ptr<Element<Address,Cell>> e) const {
		return e != _unmapped;
	}

	bool addressUnmapped(const std::shared_ptr<Element<Address,Cell>> e) const {
		return e == _unmapped;
	}

	void hexdump(const Address start, Address end, Cell filter = 0xff) {

		if (start > end || end > _endAddress) {
			fmt::print("Invalid memory range\n");
			return;
		}

		fmt::print("Memory {:#{}x}:{:#{}x} with filter {:0>{}x}\n", 
			start, AddressWidth, end, AddressWidth, filter, CellWidth); 

		int lineEnd = 16 / sizeof(Cell);
		while (AddressWidth + 2 + (CellWidth + 1) * lineEnd + lineEnd > 80) {
			lineEnd /= 2;
		}
		int hexwidth = AddressWidth + 2 + (CellWidth + 1) * lineEnd;

		int cnt = 0;
		std::string hexdump, ascii;
        
		auto begin = _mem.begin() + start;
		auto stop = _mem.begin() + end; 
		for (auto it = begin; it <= stop; it++) {

			// Start a new line with the memory address
			if (cnt == 0) {
				auto addr = std::distance(_mem.begin(), it);
				hexdump += fmt::format("{:0>{}x}  ", addr, AddressWidth);
			}
			Cell c;
			Address address = std::distance(_mem.begin(), it);
			c = (*it)->Read(address);

			// Append hex and then ascii representation
			hexdump += fmt::format("{:0>{}x} ", c, CellWidth);

			for (size_t byte_idx = 0; byte_idx < sizeof(Cell); ++byte_idx) {
				uint8_t byte_value = (c >> (byte_idx * 8)) & filter;
				if (isascii(byte_value) && isprint(byte_value))
					ascii += byte_value;
				else
					ascii += '.';
			}
			
			
			// Print the accumulated line if we're at the end of the line or
			// the end of the memory range.
			cnt++;
			if (cnt == lineEnd || it == stop) {
				cnt = 0;
				hexdump = fmt::format("{:{}}", hexdump, hexwidth);
				fmt::print("{}{}\n", hexdump, ascii);
				hexdump = "";
				ascii = "";
			}
		}
	}

	void printMap() const {

		auto it = _mem.begin();
		auto range_start = it;
		unsigned long mappedBytes = 0;

		fmt::print("Memory map:\n");

		while (it != _mem.end()) {
			if (!addressUnmapped(*it))
				mappedBytes++;

			auto next_it = std::next(it);
			bool endOfRange = (next_it == _mem.end()) ||
			  ((*next_it)->type() != (*range_start)->type());
			
			if (endOfRange) {
				Address bytes = 1 + (Address) std::distance(range_start, it);
				Address start = (Address) std::distance(_mem.begin(), 
						range_start);
				Address end = (Address) std::distance(_mem.begin(), it);
				auto type = (*it)->type();

				fmt::print("{:#{}x} - {:#x} {} ({} bytes)\n",
					   start, AddressWidth, end, AddressWidth, type, bytes);

				range_start = next_it;
			}

			it = next_it;
		}
		
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
	
	void loadData(const std::vector<Cell> &data, const Address startAddress) {

		if (startAddress > _endAddress) {
			auto s = fmt::format("Data load address is not a valid "
					     "address: {:#{}x}", startAddress, AddressWidth);
			exception(s);
		}
		if (startAddress + data.size() - 1 > _endAddress) {
			auto s = fmt::format("Data will not fit into memory at "
					     "start address {:#{}x} (data "
					     "length {} bytes)",
					     startAddress, AddressWidth, data.size());
			exception(s);
		}

		auto a = _mem.begin() + startAddress;
		Address addr = startAddress;
		for (auto i = data.begin(); i != data.end(); a++, i++, addr++)  {
			(*a)->Write(addr, *i);
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
		fmt::print("Watch list\n");
		
		if (std::none_of(_watch.begin(), _watch.end(), [](bool v) { return v; })) {
			fmt::print("--none--\n");
			return;
		}

		Address addr = 0;
		for(const auto& watching : _watch) {  
			if (watching)
				fmt::print("{:{}x}\n", addr, AddressWidth);
			addr++;
		}
	}

	void clearWatch(Address address) {
		boundsCheck(address);
		_watch[address] = false;
	}

	void clearAllWatches() {
		_watch.clear();
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
	// Field widths for fmt::
	static constexpr int AddressWidth = sizeof(Address) * 2;
	static constexpr int CellWidth = sizeof(Cell) * 2;

	Address _endAddress; // Last address
	std::shared_ptr<Element<Address,Cell>> _unmapped;	   // Default memory element 

	std::vector<std::shared_ptr<Element<Address,Cell>>> _mem;
	std::vector<bool> _watch; // Vector of watched addresses.

	void boundsCheck(const Address address) {
		if (!boundsCheckNoThrow(address)) {
			auto s = fmt::format("Address {:{}x} out of range",
					     address, AddressWidth);
			exception(s);
		}
	}

	bool boundsCheckNoThrow(const Address address) const {
		return (address <= _endAddress);
	}

	bool addressRangeOverlapsExistingMap(const Address start, const Address end) {
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
