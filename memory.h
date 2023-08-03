#pragma once

using Byte = unsigned char;
using Address_t = unsigned long;

class Memory {
public:
	Memory(size_t);
	~Memory();
	void map(Address_t, size_t);
	Byte& operator[](Address_t);
	void Init();
	Byte ReadByte(Address_t);
	void WriteByte(Address_t, Byte byte);
	void exception();

private:
	Byte *m;
	size_t memory_size;
};
