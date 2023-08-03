#include <stdio.h>
#include "memory.h"

Memory::Memory(size_t sz) {
	map(0, sz);
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

void Memory::exception() {
	printf("Memory exception!\n");
	throw -1;
}

Byte& Memory::operator[](Address_t address) {
	if (m == NULL) 
		throw ("Memory is null");
	if (address > memory_size) {
		printf("Memory out of range (indexed 0x%lx\n", address);
		exception();
	}
	return m[address];
}

void Memory::Init() {
	unsigned long i;
	if (m == NULL) 
		throw ("Memory is null");
	for (i = 0; i < memory_size; i++)
		m[i] = 0;
}

Byte Memory::ReadByte(Address_t address) {
	if (m == NULL) 
		throw ("Memory is null");
	if (address > memory_size)
		exception();
	return m[address];
}

void Memory::WriteByte(Address_t address, Byte byte) {
	if (m == NULL) 
		throw ("Memory is null");
	if (address > memory_size)
		exception();
	m[address] = byte;
}
