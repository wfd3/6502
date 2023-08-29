
CXX=g++

# Flags passed to the preprocessor.
CPPFLAGS += -I../src

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -std=gnu++23 

all: apple1

clean :
	rm -f *.o *~ \#* apple1

%.o: %.cc $(6502_HEADERS)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $*.o $<

apple1: apple1.o $(6502_OBJS) 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< -L../src -l6502
