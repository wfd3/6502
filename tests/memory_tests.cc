#include <gtest/gtest.h>
#include "../memory.h"

class MemoryTests : public testing::Test {
public:

	virtual void SetUp() {
	}
	
	virtual void TearDown()	{
	}
};

using Address  = unsigned long;
using Cell = unsigned char;

TEST_F(MemoryTests, CanMapRAMAndReadWriteIt) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), 0x1000+1);
	mem[1] = 10;
	EXPECT_EQ(mem[1], 10);
}

TEST_F(MemoryTests, CantWriteUnmapedMemory) {
	Memory<Address, Cell> mem(0x2000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), 0x2000+1);
	mem[0x1001] = 10;
	EXPECT_EQ(mem[0x1001], 0);
}

TEST_F(MemoryTests, WriteInBoundsDoesntThrowException) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), 0x1000+1);
	EXPECT_NO_THROW({mem[0x1000] = 10; });
}

#if 0
TEST_F(MemoryTests, InsaneMemorySizeThrowsMemoryException) {
	std::vector<Address, Cell> v;
	EXPECT_THROW({Memory<Address,Cell> mem(v.max_size() + 100); },
		     Memory<Address,Cell>::Exception);
}

TEST_F(MemoryTests, MapBeyondEndAddressThrowsMemoryException) {
	Memory<Address, Cell> mem(0x10);

	EXPECT_THROW({mem.mapRAM(0, 0x1000); },
		     Memory<Address,Cell>::Exception);
}

TEST_F(MemoryTests, WriteOutOfBoundsThrowsOuMemoryException) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapRAM(0, 0x1000);
	EXPECT_EQ(mem.size(), 0x1000+1);
	EXPECT_THROW({mem[0x1001] = 10; },
		     Memory<Address, Cell>::Exception);
}
#endif
TEST_F(MemoryTests,CanLoadDataIntoMemory) {
	Memory<Address, Cell> mem(0x1000);
	std::vector<unsigned char> data;

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
	std::vector<unsigned char> rom;
	rom.reserve(ROMSZ);
	rom.assign(ROMSZ, 0x10);

	mem.mapROM(0, rom);
	EXPECT_EQ(mem[0], 0x10);
}

TEST_F(MemoryTests, ROMThrowsAwayReads) {
	Memory<Address, Cell> mem(0x1000);
	constexpr size_t ROMSZ = 100;
	std::vector<unsigned char> rom;
	rom.reserve(ROMSZ);
	rom.assign(ROMSZ, 0x10);

	mem.mapROM(0, rom);
	mem[0] = 0x100;
	EXPECT_EQ(mem[0], 0x10);
}

// MIO tests
TEST_F(MemoryTests, MIONullWriteThrowsAwayWrite) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapMIO(0x100, NULL, NULL);

	mem[0x100] = 0x42;
	EXPECT_EQ(mem[0x100], 0x0);
}

TEST_F(MemoryTests, MIONullReadReturnsZero) {
	Memory<Address, Cell> mem(0x1000);

	mem.mapMIO(0x100, NULL, NULL);

	EXPECT_EQ(mem[0x100], 0x0);
}

unsigned char mio_byte;
void miowrite(unsigned char b) {
	mio_byte = b;
}

unsigned char mioread() {
	return mio_byte;
}

TEST_F(MemoryTests, MIOWrite) {
	Memory<Address, Cell> mem(0x1000);

	mio_byte = 0;
	mem.mapMIO(0x100, NULL, miowrite);

	mem[0x100] = 0x42;
	EXPECT_EQ(mio_byte, 0x42);
}

TEST_F(MemoryTests, MIORead) {
	Memory<Address, Cell> mem(0x1000);

	mio_byte = 0x45;
	mem.mapMIO(0x100, NULL, miowrite);

	EXPECT_EQ(mio_byte, 0x45);
}

TEST_F(MemoryTests, MemoryClassWithDefaultTemplateTypes) {
	Memory<> mem(0x100);

	mem.mapRAM(0, 0x100);
	mem[0x42] = 0x42;
	EXPECT_EQ(mem[0x42], 0x42);
}	



