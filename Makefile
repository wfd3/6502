# A sample Makefile for building Google Test and using it in user
# tests.  Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.

# Please tweak the following variable definitions as needed by your
# project, except GTEST_HEADERS, which you can use in your own targets
# but shouldn't modify.

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR=../googletest/googletest-1.13.0/googletest

# Where to find user code.
USER_DIR=.

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = 6502_tests

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

all: runtests

6502.o: 6502.cc 6502.h
	g++ -c 6502.cc

6502: 6502.o
	g++ -o 6502 6502.cc

tests : $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o *~
	rm -f $(TESTS_DIR)/*.o $(TESTS_DIR)/*~

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Build the tests

TESTS_DIR = ./tests
TESTS_FILES = 6502.cc \
	$(TESTS_DIR)/6502_tests_setup.cc \
	$(TESTS_DIR)/6502_tests_and.cc \
	$(TESTS_DIR)/6502_tests_ora.cc \
	$(TESTS_DIR)/6502_tests_eor.cc \
	$(TESTS_DIR)/6502_tests_nop.cc \
	$(TESTS_DIR)/6502_tests_pushpop.cc \
	$(TESTS_DIR)/6502_tests_lda_ldy_ldx.cc \
	$(TESTS_DIR)/6502_tests_flags.cc \
	$(TESTS_DIR)/6502_tests_bit.cc \
	$(TESTS_DIR)/6502_tests_cmp.cc \
	$(TESTS_DIR)/6502_tests_dec_dex_dey.cc \
	$(TESTS_DIR)/6502_tests_inc_inx_iny.cc \
	$(TESTS_DIR)/6502_tests_lsr.cc


TESTS_OBJS = 6502.o \
	$(TESTS_DIR)/6502_tests_setup.o \
	$(TESTS_DIR)/6502_tests_and.o \
	$(TESTS_DIR)/6502_tests_ora.o \
	$(TESTS_DIR)/6502_tests_eor.o \
	$(TESTS_DIR)/6502_tests_nop.o \
	$(TESTS_DIR)/6502_tests_pushpop.o \
	$(TESTS_DIR)/6502_tests_lda_ldy_ldx.o \
	$(TESTS_DIR)/6502_tests_flags.o \
	$(TESTS_DIR)/6502_tests_bit.o \
	$(TESTS_DIR)/6502_tests_cmp.o \
	$(TESTS_DIR)/6502_tests_dec_dex_dey.o \
	$(TESTS_DIR)/6502_tests_inc_inx_iny.o \
	$(TESTS_DIR)/6502_tests_lsr.o

%.o: %.cc 6502.h  $(GTEST_HEADERS) 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $*.o $<

6502_tests : $(TESTS_OBJS) gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

runtests: 6502_tests
	./6502_tests
