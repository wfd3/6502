
CXX=g++

# Flags passed to the preprocessor.
CPPFLAGS += 

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pedantic -Wuninitialized -Wshadow -Wconversion -std=gnu++23 -I. # -Weffc++

all: lib6502.a

6502_HEADERS = memory.h clock.h 6502.h
6502_FILES = 6502.cc debug.cc instructions.cc opcode_map.cc 
6502_OBJS  = 6502.o debug.o instructions.o opcode_map.o

clean :
	rm -f $(TESTS) $(BINS) gtest.a gtest_main.a *.o *~ 
	rm -f $(TESTS_DIR)/*.o $(TESTS_DIR)/*~
	rm -f *.a


%.o: %.cc $(6502_HEADERS) Makefile
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $*.o $<

lib6502.a: $(6502_OBJS) Makefile
	$(AR) -rcs $@ $(6502_OBJS)
