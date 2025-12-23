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

cmake_minimum_required(VERSION 3.10...3.28)

project( apple1 )

# source for the 6502 executable
set (APPLE1_SOURCES "apple1.cc" "display_interface.cc" "terminal_keyboard.cc")

# Add SDL2 display source if available
if(HAVE_SDL2)
  list(APPEND APPLE1_SOURCES "sdl2_display.cc")
endif()

source_group("src" FILE ${APPLE1_SOURCES})

add_executable(apple1 ${APPLE1_SOURCES})
target_link_libraries(apple1 6502)
if(LINUX)
  target_link_libraries(apple1 readline fmt)
elseif(APPLE)
	target_link_libraries(apple1 readline fmt::fmt-header-only)
elseif(MSVC)
	target_link_libraries(apple1 fmt::fmt-header-only)
endif()

# Add SDL2 support if available
if(HAVE_SDL2)
  target_link_libraries(apple1 SDL2::SDL2)
  if(LINUX)
    target_link_libraries(apple1 SDL2::SDL2main)
  endif()
endif()

# source for the 65C02 executable
set (APPLE1_65C02_SOURCES "65C02-apple1.cc" "display_interface.cc" "terminal_keyboard.cc")

# Add SDL2 display source if available
if(HAVE_SDL2)
  list(APPEND APPLE1_65C02_SOURCES "sdl2_display.cc")
endif()

source_group("src" FILE ${APPLE1_65C02_SOURCES})

add_executable(65C02-apple1 ${APPLE1_65C02_SOURCES})
target_link_libraries(65C02-apple1 65C02 6502)
if(LINUX)
  target_link_libraries(65C02-apple1 readline fmt)
elseif(APPLE)
	target_link_libraries(65C02-apple1 readline fmt::fmt-header-only)
elseif(MSVC)
	target_link_libraries(65C02-apple1 fmt::fmt-header-only)
endif()

# Add SDL2 support if available
if(HAVE_SDL2)
  target_link_libraries(65C02-apple1 SDL2::SDL2)
  if(LINUX)
    target_link_libraries(65C02-apple1 SDL2::SDL2main)
  endif()
endif()

# Copy apple1 to {BUILDDIR}/bin
add_custom_command(TARGET apple1 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:apple1>" "${BINDIR}")
# Copy 65C02-apple1 to {BUILDDIR}/bin
add_custom_command(TARGET 65C02-apple1 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:65C02-apple1>" "${BINDIR}")
  