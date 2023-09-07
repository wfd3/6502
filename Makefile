# This is just a wrapper around CMake.
# Any changes to the build system should be done in CMake, not here.
#
# Copyright (C) 2023 Walt Drummond
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

# The build directory (where CMake generates the build files)
BUILDDIR := build

# The source directory (where your CMakeLists.txt file is)
SRCDIR := ..

# The default target (invoked when you run `make` with no arguments)
# This target just invokes CMake to configure your project
.PHONY: all
all: configure lib apple1

# The `configure` target invokes CMake to configure your project
.PHONY: configure
configure:
	mkdir -p $(BUILDDIR)
	cd $(BUILDDIR) && cmake $(SRCDIR)

# The `build` target builds your project using the generated build files
.PHONY: build
build: configure
	cd $(BUILDDIR) && make

# The `clean` target deletes the build directory
.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

# The `rebuild` target cleans the build directory and then rebuilds the project
.PHONY: rebuild
rebuild: clean build

# Target to build the 'tests' target
.PHONY: tests
tests: configure
	cd $(BUILDDIR) && make tests

# Target to run the 'runtests' target
.PHONY: runtests
runtests: configure
	cd $(BUILDDIR) && make runtests

# Target to run the 'run_memory_tests' target
.PHONY: run_memory_tests
run_memory_tests: configure
	cd $(BUILDDIR) && make run_memory_tests

# Target to run the 'run_memory_tests' target
.PHONY: run_6502_tests
run_6502_tests: configure
	cd $(BUILDDIR) && make run_6502_tests

# Target to build the 'lib' target
.PHONY: lib
lib: configure
	cd $(BUILDDIR) && make lib

# Target to build the 'apple1' target
.PHONY: apple1
apple1: configure
	cd $(BUILDDIR) && make apple1
