// Copyright (C) 2025 Walt Drummond
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <fmt/core.h>
#include <cstdio>

// Template function to print CPU status for any CPU type
// Works with both MOS6502 and MOS65C02 since they share the same register interface
template<typename CPUType>
void printCPUStatus(CPUType& cpu) {
	fmt::print("\r\033[K");  // Clear line
	fmt::print("PC:{:04X} A:{:02X} X:{:02X} Y:{:02X} SP:{:02X} [{}{}{}{}{}{}{}] ",
		cpu.getPC(),
		cpu.getA(),
		cpu.getX(),
		cpu.getY(),
		cpu.getSP(),
		cpu.getFlagN() ? 'N' : '-',
		cpu.getFlagV() ? 'V' : '-',
		cpu.getFlagB() ? 'B' : '-',
		cpu.getFlagD() ? 'D' : '-',
		cpu.getFlagI() ? 'I' : '-',
		cpu.getFlagZ() ? 'Z' : '-',
		cpu.getFlagC() ? 'C' : '-'
	);
	if (cpu.pendingNMI())
		fmt::print("NMI ");
	if (cpu.pendingIRQ())
		fmt::print("IRQ ");
	if (cpu.inReset())
		fmt::print("RESET ");
	fflush(stdout);
}
