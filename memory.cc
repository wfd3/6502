#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <iostream>
#include <memory>
#include <cstdarg>
#include <vector>

#include "memory.h"

Memory::Memory(size_t sz) {
	map(0, sz);
	Init();
}

Memory::~Memory() {
	delete [] _m;
}

void Memory::map(Address_t start, size_t sz) {
	if (start != 0) 
		Exception("Invalid start address %lx\n", start);

	_size = sz;
	_m = new Byte[_size];

	for (size_t i = 0; i < sz; i++) {
		_m[i] = 0;
	}
}

void Memory::Exception(const char *fmt_str, ...) {
	va_list args;
	int final_n, n = 256;
	std::unique_ptr<char[]> formatted;

	va_start(args, fmt_str);
	while (1) {
		// Wrap the plain char array into the unique_ptr 
		formatted.reset(new char[n]); 
		strcpy(&formatted[0], fmt_str);

		final_n = vsnprintf(formatted.get(), n, fmt_str, args);

		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}

	va_end(args);

	printf("Memory Exception: %s\n", formatted.get());
	printf("Halting\n");
	exit(1);
}

void Memory::Init() {
	unsigned long i;
	if (_m == NULL) 
		Exception("Memory has not be initalized");

	for (i = 0; i < _size; i++) {
		_m[i] = 0;
		_watch[i] = false;
	}
}

Byte& Memory::operator[](Address_t address) {
	return _m[address];
}

Byte Memory::ReadByte(Address_t address) {
	if (_m == NULL) 
		Exception("Memory has not be initalized");
	if (address > _size)
		Exception("Memory address out of range: 0x%04x (max 0x%04x) ",
			  address, _size-1);

	return _m[address];
}

void Memory::WriteByte(Address_t address, Byte value) {
	if (_m == NULL) 
		Exception("Memory has not be initalized");
	if (address > _size)
		Exception("Memory address out of range: 0x%04x ", address);

	if (_watch[address]) {
		printf("# mem[%04lx] %02x -> %02x\n",
		       address, _m[address], value);
	}

	_m[address] = value;
}

Word Memory::ReadWord(Address_t address) {
	Word value = ReadByte(address) | (ReadByte(address + 1) << 8);
	return value;
}

void Memory::WriteWord(Address_t address, Word value) {
	WriteByte(address, value & 0xff);
	WriteByte(address + 1, value >> 8);
}

void Memory::LoadProgramFromFile(const char *file, Address_t startAddress) {
	int f;
	struct stat stat;
	Byte *buf;

	if ((f = open(file, O_RDONLY)) < 0) 
		Exception("Can't open file '%s': %s", file, strerror(errno));
	
	if (fstat(f, &stat) < 0)
		Exception("File error on '%s': %s", file, strerror(errno));

	if ((size_t) stat.st_size > _size)
		Exception("File '%s' cannot fit into memory (size=%d)",
			  file, stat.st_size);
	
	if ((size_t) stat.st_size + startAddress > _size)
		Exception("File '%s' cannot fit into memory at start address "
			  "0x%04x (size=%d)",
			  file, startAddress, stat.st_size);

	buf = (Byte *) new Byte[stat.st_size];
	size_t r = read(f, buf, stat.st_size);
	if (r != (size_t) stat.st_size) 
		Exception("Can't read file '%s': %s", file, strerror(errno));

	LoadProgram(buf, startAddress, stat.st_size);

	delete []buf;
	close(f);
}

void Memory::LoadProgram(const Byte *program, Address_t startAddress,
			 size_t programLength) {

	if (startAddress > _size)
		Exception("Program load address is not a valid address: 0x%04x",
			  startAddress);
	if (startAddress + programLength > _size)
		Exception("Program will not fit into memory at start address "
			  "0x%04x (program length %d bytes)",
			  startAddress, programLength);

	for (size_t i = 0; i < programLength; i++) 
		_m[startAddress + i] = program[i];
}

static const std::string vformat(const char * const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}

void Memory::Hexdump(Address_t start, Address_t end) {

	printf("# Memory Dump 0x%04lx:0x%04lx\n", start, end);

	if (start > end || end > _size) {
		printf("# -- Invalid memory range\n");
		return;
	}

	for (Address_t i = start; i <= end; i += 16) {
		std::string ascii = "................";
		std::string hexdmp;

		hexdmp += vformat("%04x  ", i);

		for (Address_t j = 0; j < 16; j++) {
			if ((unsigned long) j + i > end) {
				hexdmp += "   ";
				ascii += ' ';
			} else {
				Address_t a = j + i;
				hexdmp += vformat("%02x ", _m[a]);
				if (isprint(_m[a]))
					ascii.at(j) = _m[a];
			}
		}

		std::cout << hexdmp << "  " << ascii << std::endl;
	}
}
