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

project( 6502 )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
  add_compile_options(/MP)	
  add_compile_options(/W4 /wd4201 /WX)	
else()
  add_compile_options(-W -Wall -pedantic -Wuninitialized -Wshadow)
#  add_compile_options(-Weffc++)
  add_compile_options(-Werror)	#Comment this out if -Weffc++ is enabled
  add_compile_options(-std=gnu++23) 
endif()

add_definitions(-DBINFILE_PATH=\"${CMAKE_SOURCE_DIR}/binfiles\")
include_directories(${CMAKE_SOURCE_DIR}/src)

set(BINDIR "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(apple1)


