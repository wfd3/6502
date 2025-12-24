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
			_t[a] = static_cast<Cell>('Z');
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

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
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

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address8Cell) {
	Memory<uint16_t, uint8_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address16Cell) {
	Memory<uint16_t, uint16_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address32Cell) {
	Memory<uint16_t, uint32_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump16Address64Cell) {
	Memory<uint16_t, uint64_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump32Address32Cell) {
	Memory<uint32_t, uint32_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryHexDump32Address64Cell) {
	Memory<uint32_t, uint64_t> mem(0x100);	
	mem.mapRAM(0, 0x100);

	MemoryDebugger debug(mem);
	debug.hexdump(0, 0x100);
}

TEST_F(MemoryTests, MemoryPrintMap) {
	Memory<Address, Cell> mem(0x100);	
	mem.mapRAM(0xf0, 0x100);

	MemoryDebugger debug(mem);
	debug.printMap();
}

//////////
// MemoryDebugger tests

TEST_F(MemoryTests, MemoryDebuggerWatch) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Test watch functionality
	debug.enableWatch(0x50);
	EXPECT_TRUE(debug.watching(0x50));
	EXPECT_FALSE(debug.watching(0x51));

	// Test clear watch
	debug.clearWatch(0x50);
	EXPECT_FALSE(debug.watching(0x50));

	// Test multiple watches
	debug.enableWatch(0x10);
	debug.enableWatch(0x20);
	debug.enableWatch(0x30);
	
	EXPECT_TRUE(debug.watching(0x10));
	EXPECT_TRUE(debug.watching(0x20));
	EXPECT_TRUE(debug.watching(0x30));

	// Test clear all watches
	debug.clearAllWatches();
	EXPECT_FALSE(debug.watching(0x10));
	EXPECT_FALSE(debug.watching(0x20));
	EXPECT_FALSE(debug.watching(0x30));
}

TEST_F(MemoryTests, MemoryDebuggerWatchedWrite) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Set up watch on address
	debug.enableWatch(0x42);
	
	// Write through debugger should work and show watch output
	debug.watchedWrite(0x42, 0xAB);
	EXPECT_EQ(mem.Read(0x42), 0xAB);
	
	// Write to unwatched address should work normally
	debug.watchedWrite(0x43, 0xCD);
	EXPECT_EQ(mem.Read(0x43), 0xCD);
}

TEST_F(MemoryTests, MemoryDebuggerFind) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Set up test pattern
	std::string pattern = "HELLO";
	for (size_t i = 0; i < pattern.length(); i++) {
		mem.Write(0x20 + i, pattern[i]);
	}
	
	// Another copy at different location
	for (size_t i = 0; i < pattern.length(); i++) {
		mem.Write(0x80 + i, pattern[i]);
	}

	// Find the pattern
	auto positions = debug.find(pattern);
	EXPECT_EQ(positions.size(), 2);
	EXPECT_TRUE(std::find(positions.begin(), positions.end(), 0x20) != positions.end());
	EXPECT_TRUE(std::find(positions.begin(), positions.end(), 0x80) != positions.end());

	// Test pattern not found
	auto notFound = debug.find("WORLD");
	EXPECT_EQ(notFound.size(), 0);
}

TEST_F(MemoryTests, MemoryDebuggerFindWithFilter) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Set up test data with some bits masked
	mem.Write(0x10, 0b10101010);  // 0xAA
	mem.Write(0x11, 0b11101110);  // 0xEE
	mem.Write(0x12, 0b10101010);  // 0xAA

	// Search for pattern using lower 4 bits only
	std::string pattern;
	pattern.push_back(0b00001010);  // Lower 4 bits of 0xAA
	pattern.push_back(0b00001110);  // Lower 4 bits of 0xEE
	pattern.push_back(0b00001010);  // Lower 4 bits of 0xAA

	auto positions = debug.find(pattern, 0x0F);  // Mask to lower 4 bits
	EXPECT_EQ(positions.size(), 1);
	EXPECT_EQ(positions[0], 0x10);
}

TEST_F(MemoryTests, MemoryDebuggerHexdumpBasic) {
	Memory<uint16_t, uint8_t> mem(0x20);
	mem.mapRAM(0, 0x20);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Fill with test pattern
	for (uint16_t i = 0; i < 0x20; i++) {
		mem.Write(i, i);
	}

	// Test should not crash and produce output
	debug.hexdump(0x00, 0x1F);
}

TEST_F(MemoryTests, MemoryDebuggerHexdumpWithExpression) {
	Memory<uint16_t, uint8_t> mem(0x20);
	mem.mapRAM(0, 0x20);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Fill with test pattern
	for (uint16_t i = 0; i < 0x20; i++) {
		mem.Write(i, 0x10);  // All values are 0x10
	}

	// Test with expression (should add 5 to each value: 0x10 + 0x05 = 0x15)
	debug.hexdump(0x00, 0x1F, "+ 05");
}

TEST_F(MemoryTests, MemoryDebuggerInvalidRange) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Test invalid range (should not crash, just print error message)
	debug.hexdump(0x50, 0x40);  // start > end
	debug.hexdump(0x00, 0x200); // end > memory size
}

TEST_F(MemoryTests, MemoryDebuggerWithMemoryMappedDevice) {
	Memory<uint16_t, uint8_t> mem(0x100);
	auto device = std::make_shared<testdev<uint16_t, uint8_t>>();
	
	mem.mapRAM(0, 0x100);
	mem.mapDevice(device, 0x10);
	
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Should show memory map with device
	debug.printMap();
	
	// Should show device data in hexdump
	debug.hexdump(0x10, 0x20);
}

TEST_F(MemoryTests, MemoryDebuggerDifferentTemplateTypes) {
	// Test with 32-bit addresses and 16-bit cells
	Memory<uint32_t, uint16_t> mem(0x1000);
	mem.mapRAM(0, 0x1000);
	MemoryDebugger<uint32_t, uint16_t> debug(mem);

	// Fill with test pattern
	for (uint32_t i = 0; i < 16; i++) {
		mem.Write(i, i * 0x1111);
	}

	// Test operations
	debug.hexdump(0, 15);
	debug.printMap();

	// Test watch
	debug.enableWatch(0x500);
	EXPECT_TRUE(debug.watching(0x500));
}

TEST_F(MemoryTests, MemoryDebuggerListWatch) {
	Memory<uint16_t, uint8_t> mem(0x100);
	mem.mapRAM(0, 0x100);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Test empty watch list
	debug.listWatch();  // Should show "--none--"

	// Add some watches
	debug.enableWatch(0x10);
	debug.enableWatch(0x20);
	debug.enableWatch(0x30);

	// Test non-empty watch list
	debug.listWatch();  // Should show addresses
}

TEST_F(MemoryTests, MemoryDebuggerHexdumpDivideByZeroProtection) {
	Memory<uint16_t, uint8_t> mem(0x20);
	mem.mapRAM(0, 0x20);
	MemoryDebugger<uint16_t, uint8_t> debug(mem);

	// Fill with test pattern (0x0a = 10)
	for (uint16_t i = 0; i < 0x20; i++) {
		mem.Write(i, 0x0a);
	}

	// Test the calculateValue function directly first
	auto result = debug.calculateValue("/ 0", 10);
	EXPECT_TRUE(result.hasError());
	EXPECT_EQ(result.error, decltype(debug)::CalculateError::DivisionByZero);
	EXPECT_EQ(result.value, 10); // Should return original value
	EXPECT_STREQ(result.errorMessage(), "Division by zero in expression");

	result = debug.calculateValue("% 0", 10);
	EXPECT_TRUE(result.hasError());
	EXPECT_EQ(result.error, decltype(debug)::CalculateError::ModuloByZero);
	EXPECT_EQ(result.value, 10); // Should return original value
	EXPECT_STREQ(result.errorMessage(), "Modulo by zero in expression");

	// Test normal operations work
	result = debug.calculateValue("/ 2", 10);
	EXPECT_FALSE(result.hasError());
	EXPECT_EQ(result.value, 5);

	result = debug.calculateValue("% 3", 10);
	EXPECT_FALSE(result.hasError());
	EXPECT_EQ(result.value, 1);

	// Test hexdump with divide by zero (should print error and exit early)
	testing::internal::CaptureStdout();
	debug.hexdump(0x00, 0x01, "/ 0");
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Error: Division by zero") != std::string::npos);
	EXPECT_FALSE(output.find("Memory") != std::string::npos); // Should NOT show memory dump

	// Test hexdump with mod by zero
	testing::internal::CaptureStdout();
	debug.hexdump(0x00, 0x01, "% 0");
	output = testing::internal::GetCapturedStdout();
	EXPECT_TRUE(output.find("Error: Modulo by zero") != std::string::npos);
	EXPECT_FALSE(output.find("Memory") != std::string::npos); // Should NOT show memory dump

	// Test the specific issue: + 2 should add 2, not show 0x32
	result = debug.calculateValue("+ 2", 10);
	EXPECT_FALSE(result.hasError());
	EXPECT_EQ(result.value, 12); // 10 + 2 = 12, not 50

	result = debug.calculateValue("+2", 10);  // Without space
	EXPECT_FALSE(result.hasError());
	EXPECT_EQ(result.value, 12); // Should also work
}