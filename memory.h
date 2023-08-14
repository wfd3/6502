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

	// XXX TODO:  This is gross.
	// the _m array should be an array of structs that contain:
	// {
	//  Byte element;
	//  Type type (RAM, ROM, IO, etc.);
	//  bool watching;
	// }
	// and probably function pointers to read and write methods for each
	// memory type.
	//
	// But for now, this works. 
	void enableWatch(Address_t address) {
		_watch[address] = true;
	}

	bool watching(Address_t address) {
		return _watch[address];
	}

	void listWatch() {
		printf("# Watch list\n");
		for(int i = 0; i < 64*1024; i++)
			if (_watch[i])
				printf("# %04x\n", i);
	}

	void clearWatch(Address_t address) {
		_watch[address] = false;
	}

private:
	Byte *_m;
	size_t _size;
	bool _watch[64*1024];
};
