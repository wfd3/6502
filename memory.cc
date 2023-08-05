#include <stdio.h>
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

void Memory::LoadProgramFromFile(const char *file, Address_t start_address) { }

void Memory::LoadProgram(const Byte *program, Address_t start_address,
			 size_t program_length) {

	if (start_address > memory_size)
		Exception("Program load address is not a valid address: 0x%04x",
			  start_address);
	if (start_address + program_length > memory_size)
		Exception("Program will not fit into memory at start address "
			  "0x%04x (program length %d bytes)",
			  start_address, program_length);

	for (size_t i = 0; i < program_length; i++) 
		m[start_address + i] = program[i];
}
