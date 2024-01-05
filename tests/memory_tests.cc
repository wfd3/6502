//
// Tests for memory class
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

#include <gtest/gtest.h>
#include <memory.h>
#include <cstdint>

class MemoryTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

using Address  = uint64_t;
using Cell = uint64_t;

TEST_F(MemoryTests, CanMapRAMAndReadWriteIt) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), (Address) 0x1000 + 1);
	mem[1] = 10;
	EXPECT_EQ(mem[1], 10);
}

TEST_F(MemoryTests, CantWriteUnmappedMemory) {
	Memory<Address, Cell> mem(0x2000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), (Address)  0x2000+1);
	mem[0x1001] = 10;
	EXPECT_EQ(mem[0x1001], 0);
}

TEST_F(MemoryTests, WriteInBoundsDoesntThrowException) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(),(Address) 0x1000+1);
	EXPECT_NO_THROW({mem[0x1000] = 10; });
}

TEST_F(MemoryTests, InsaneMemorySizeThrowsMemoryException) {
	std::vector<unsigned long> v;
	size_t bignum = v.max_size();
	bool caughtMemoryException = false;
	bool caughtOtherException = false;
	
	try {
		Memory<Address, Cell> mem(bignum + 100);
	}
	catch([[maybe_unused]] Memory<Address, Cell>::Exception &e) {
		caughtMemoryException = true;
	}
	catch(...) {
		caughtOtherException = true;
	}

	EXPECT_TRUE(caughtMemoryException);
	EXPECT_FALSE(caughtOtherException);
}

TEST_F(MemoryTests, MapBeyondEndAddressThrowsMemoryException) {
	Memory<Address, Cell> mem(0x10);
	bool caughtMemoryException = false;

	try {
		mem.mapRAM(0, 0x1000); 
	}
	catch ([[maybe_unused]] Memory<Address, Cell>::Exception &e) {
		caughtMemoryException = true;
	}
	catch (...) {
	}

	EXPECT_TRUE(caughtMemoryException);
}

TEST_F(MemoryTests, WriteOutOfBoundsThrowsOuMemoryException) {
	Memory<Address, Cell> mem(0x1000);
	bool caughtMemoryException = false;

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), (Address) 0x1000+1);

	try {
		mem[0x1001] = 10;
	}
	catch([[maybe_unused]] Memory<Address, Cell>::Exception &e) {
		caughtMemoryException = true;
	}
	catch(...) {
	}

	EXPECT_TRUE(caughtMemoryException);
}
TEST_F(MemoryTests,CanLoadDataIntoMemory) {
	Memory<Address, Cell> mem(0x1000);
	std::vector<Cell> data;

	data.reserve(100);
	data.assign(100, 0xef);

	mem.mapRAM(0, 0x1000);
	mem.loadData(data, 0);

	for (unsigned long i = 0; i < data.size(); i++) 
		EXPECT_EQ(mem[i], 0xef);
}

// ROM tests
TEST_F(MemoryTests, ROMRead) {
	Memory<Address, Cell> mem(0x1000);
	constexpr size_t ROMSZ = 100;
	std::vector<Cell> rom;
	rom.reserve(ROMSZ);
	rom.assign(ROMSZ, 0x10);

	mem.mapROM(0, rom);
	EXPECT_EQ(mem[0], 0x10);
}

TEST_F(MemoryTests, ROMThrowsAwayReads) {
	Memory<Address, Cell> mem(0x1000);
	constexpr size_t ROMSZ = 100;
	std::vector<Cell> rom;
	rom.reserve(ROMSZ);
	rom.assign(ROMSZ, 0x10);

	mem.mapROM(0, rom);
	mem[0] = 0x100;
	EXPECT_EQ(mem[0], 0x10);
}

// MIO tests
TEST_F(MemoryTests, MIONullWriteThrowsAwayWrite) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapMIO(0x100, nullptr, nullptr);

	mem[0x100] = 0x42;
	EXPECT_EQ(mem[0x100], 0x0);
}

TEST_F(MemoryTests, MIONullReadReturnsZero) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapMIO(0x100, nullptr, nullptr);

	EXPECT_EQ(mem[0x100], 0x0);
}

Cell mio_byte;
void miowrite(Cell b) {
	mio_byte = b;
}

Cell mioread() {
	return mio_byte;
}

TEST_F(MemoryTests, MIOWrite) {
	Memory<Address, Cell> mem(0x1000);

	mio_byte = 0;
	mem.mapMIO(0x100, nullptr, miowrite);

	mem[0x100] = 0x42;
	EXPECT_EQ(mio_byte, 0x42);
}

TEST_F(MemoryTests, MIORead) {
	Memory<Address, Cell> mem(0x1000);

	mio_byte = 0x45;
	mem.mapMIO(0x100, nullptr, miowrite);

	EXPECT_EQ(mio_byte, 0x45);
}

TEST_F(MemoryTests, MemoryClassWithDefaultTemplateTypes) {
	Memory<> mem(0x100);

	mem.mapRAM(0, 0x100);
	mem[0x42] = 0x42;
	EXPECT_EQ(mem[0x42], 0x42);
}	

// Memory mapped device
template<class Address, class Cell> 
class testdev : public MemMappedDevice<Address, Cell> {
public:
	std::map<uint8_t, Cell> _t;

	testdev(std::vector<Address>& addresses) : MemMappedDevice<Address, Cell>() {
		for (const auto& a : addresses) {
			this->_ioPorts.insert(a);
			_t[a] = 'Z';
		}
	}

	testdev(std::initializer_list<Address> addresses) : MemMappedDevice<Address, Cell>() {
		for (const auto& a : addresses) {
			this->_ioPorts.insert(a);
			_t[a] = 'Z';
		}
	}

	testdev() { }

	Cell Read([[maybe_unused]] const Address address) override { // TODO: bounds checking 
		auto port = this->decodeAddress(address);
		if (_t.find(port) == _t.end())
			return 0;
		return _t[port]; 
	}

	void Write([[maybe_unused]] const Address address, [[maybe_unused]] const Cell c) override { // TODO: Bounds checking
		auto port = this->decodeAddress(address);
		_t[port] = c; 
	}

	virtual std::string type() const override {
		return "testdev";
	}
};

TEST_F(MemoryTests, MemoryClassCanInsertAndWriteToCustomDevice) {
	Memory<Address, Cell> mem(0x100);
	auto d = std::make_shared<testdev<Address,Cell>>(std::initializer_list<Address>{0x10, 0x15});

	#define BASE 0
	mem.mapDevice(d, BASE);
	mem[0x10] = 'K';

	EXPECT_EQ(d->_t[0x10], 'K');
}

TEST_F(MemoryTests, MemoryClassCanInsertAndReadFromCustomDevice) {
	Memory<Address, Cell> mem(0x100);
	auto d = std::make_shared<testdev<Address,Cell>>(std::initializer_list<Address>{0x10,0x15});
	
	#define BASE 0
	mem.mapDevice(d, BASE);
	mem[0x10] = 'W';
	EXPECT_EQ(mem[0x10], 'W');
}

TEST_F(MemoryTests, MemoryHexDumpWithMappedDevice) {
	Memory<uint16_t, uint8_t> mem(0x100);	
	std::vector<uint16_t> addrs = {0x10, 0x13, 0x15};
	auto d = std::make_shared<testdev<uint16_t,uint8_t>>(addrs);
	
	#define BASE 0
	mem.mapRAM(0, 0x100);
	mem.mapDevice(d, BASE);
	EXPECT_EQ(mem[0x10], 'Z');

	mem[0x12] = 'z';

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDumpWithMappedDeviceExplicitAddressList) {
	Memory<uint16_t, uint8_t> mem(0x100);	
	auto addrs = std::initializer_list<uint16_t>{0x15, 0x16, 0x17, 0x18, 0x19, /* gap */ 0x20};
	auto d = std::make_shared<testdev<uint16_t,uint8_t>>(addrs);
	
	#define BASE 0
	mem.mapRAM(0, 0x100);
	mem.mapDevice(d, BASE);
	EXPECT_EQ(mem[0x15], 'Z');

	mem[0x18] = 'z';

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address8Cell) {
	Memory<uint16_t, uint8_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address16Cell) {
	Memory<uint16_t, uint16_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address32Cell) {
	Memory<uint16_t, uint32_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address64Cell) {
	Memory<uint16_t, uint64_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump32Address32Cell) {
	Memory<uint32_t, uint32_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump32Address64Cell) {
	Memory<uint32_t, uint64_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	mem.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryPrintMap) {
	Memory<Address, Cell> mem(0x100);	
	mem.mapRAM(0xf0, 0x100);

	mem.printMap();
}