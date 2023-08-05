#pragma once

using Byte = unsigned char;
using Address_t = unsigned long;

class Memory {
public:
	Memory(size_t);
	~Memory();

	void Init();
	void map(Address_t, size_t);

	Byte ReadByte(Address_t);
	void WriteByte(Address_t, Byte byte);
	void Exception(const char *, ...);

	void LoadProgramFromFile(const char *, Address_t);
	void LoadProgram(const Byte *, Address_t, size_t);

	Byte operator[](Address_t address) const;
	Byte& operator[](Address_t address);

private:
	Byte *m;
	size_t memory_size;
};
