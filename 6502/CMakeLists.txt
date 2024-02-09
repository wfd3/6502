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

cmake_minimum_required(VERSION 3.7)

project( lib6502 )

set (SOURCES
  "6502.h"
  "6502.cc"
  "opcode_map.cc"
  "instructions.cc"
  "disassembler.cc"
  "debugger.cc"
  "debugger_commands.cc"
  "utils.cc"
)
		
source_group("src" FILES ${SOURCES})
include_directories(AFTER ${fmt_INCLUDE_DIRS})
add_library( 6502 ${SOURCES} )
target_compile_definitions(6502 PRIVATE FMT_HEADER_ONLY)

add_library( 6502-test ${SOURCES})
target_compile_definitions(6502-test PRIVATE FMT_HEADER_ONLY TEST_BUILD)