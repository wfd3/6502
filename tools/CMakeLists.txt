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

cmake_minimum_required(VERSION 3.0...3.7)

project( tools )

# bin2hex
set (BIN2HEX_SOURCES "bin2hex.cc")
source_group("src" FILE ${BIN2HEX_SOURCES})
add_executable(bin2hex ${BIN2HEX_SOURCES})
target_link_libraries(bin2hex fmt)

# Copy bin2hex to {BUILDDIR}/bin
add_custom_command(TARGET bin2hex POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:bin2hex>" "${BINDIR}")

# tester 
set (TESTER_SOURCES "tester.cc")
source_group("src" FILE ${TESTER_SOURCES})
include_directories(${CMAKE_SOURCE_DIR}/apple1)  
add_executable(tester ${TESTER_SOURCES})
target_link_libraries(tester 65C02 6502) 

if(LINUX)
  target_link_libraries(tester readline fmt)
elseif(APPLE)
	target_link_libraries(tester readline fmt::fmt-header-only)
elseif(MSVC)
	target_link_libraries(tester fmt::fmt-header-only)
endif()

add_custom_command(TARGET tester POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:tester>" "${BINDIR}")
