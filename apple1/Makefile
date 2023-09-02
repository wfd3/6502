
CXX=g++

# Flags passed to the preprocessor.
CPPFLAGS += -I../src

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -std=gnu++23 

all: apple1

clean :
	rm -f *.o *~ \#* apple1 core

%.o: %.cc $(6502_HEADERS)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $*.o $<

apple1: apple1.o $(6502_OBJS) ../src/lib6502.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< -L../src -l6502 -lfmt -lreadline
