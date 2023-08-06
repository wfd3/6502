#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <memory>
#include <cstdarg>

#include "memory.h"

Memory::Memory(size_t sz) {
	map(0, sz);
	Init();
}

Memory::~Memory() {
	delete [] m;

}

void Memory::map(Address_t start, size_t sz) {
	if (start != 0) {
		printf("INVALID start address %lx\n", start);
		throw -1;
	}
	memory_size = sz;
	m = new Byte[memory_size];
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

// Write to memory
Byte& Memory::operator[](Address_t address) {
	if (m == NULL) 
		Exception("Memory has not been initalized");
	if (address > memory_size) {
		Exception("Memory address out of range: 0x%04x ", address);
	}

	return m[address];
}

void Memory::Init() {
	unsigned long i;
	if (m == NULL) 
		Exception("Memory has not be initalized");

	for (i = 0; i < memory_size; i++)
		m[i] = 0;
}

Byte Memory::ReadByte(Address_t address) {
	if (m == NULL) 
		Exception("Memory has not be initalized");
	if (address > memory_size)
		Exception("Memory address out of range: 0x%04x ", address);

	return m[address];
}

void Memory::WriteByte(Address_t address, Byte byte) {
	if (m == NULL) 
		Exception("Memory has not be initalized");
	if (address > memory_size)
		Exception("Memory address out of range: 0x%04x ", address);

	m[address] = byte;
}

void Memory::LoadProgramFromFile(const char *file, Address_t startAddress) {

	int f;
	struct stat stat;

	if ((f = open(file, O_RDONLY)) < 0) 
		Exception("Can't open file '%s': %s", file, strerror(errno));
	
	if (fstat(f, &stat) < 0)
		Exception("File error on '%s': %s", file, strerror(errno));

	if ((size_t) stat.st_size > memory_size)
		Exception("File '%s' cannot fit into memory (size=%d)",
			  file, stat.st_size);
	
	if ((size_t) stat.st_size + startAddress > memory_size)
		Exception("File '%s' cannot fit into memory at start address "
			  "0x%04x (size=%d)",
			  file, startAddress, stat.st_size);
	
	size_t r = read(f, m + startAddress, stat.st_size);
	if (r != (size_t) stat.st_size) 
		Exception("Can't read file '%s': %s", file, strerror(errno));

	close(f);
}

void Memory::LoadProgram(const Byte *program, Address_t startAddress,
			 size_t programLength) {

	if (startAddress > memory_size)
		Exception("Program load address is not a valid address: 0x%04x",
			  startAddress);
	if (startAddress + programLength > memory_size)
		Exception("Program will not fit into memory at start address "
			  "0x%04x (program length %d bytes)",
			  startAddress, programLength);

	for (size_t i = 0; i < programLength; i++) 
		m[startAddress + i] = program[i];
}
