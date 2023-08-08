#pragma once

using Byte = unsigned char;
using Word = unsigned int;
using Address_t = unsigned long;

class Memory {
public:
	
	Memory(size_t);
	~Memory();

	void Init();
	void map(Address_t, size_t);

	Byte& operator[](Address_t address);

	Byte ReadByte(Address_t);
	void WriteByte(Address_t, Byte);
	Word ReadWord(Address_t);
	void WriteWord(Address_t, Word);
	void Exception(const char *, ...);
	void Hexdump(Address_t, Address_t);

	void LoadProgramFromFile(const char *, Address_t);
	void LoadProgram(const Byte *, Address_t, size_t);

private:
	Byte *_m;
	size_t _size;
};
