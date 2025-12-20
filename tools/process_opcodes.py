#!/usr/bin/env python3
"""
Process 6502 opcode descriptions into various C++ code snippets.

This script reads opcode definition files and generates C++ code including:
- Opcode constant definitions
- Function prototypes and shells
- Instruction map initialization

Usage:
    # Generate code for 6502 CPU
    python3 process_opcodes.py --cpu=6502 < 6502/opcodes_6502.txt > output.cc

    # Generate code for 65C02 CPU
    python3 process_opcodes.py --cpu=65C02 < 65C02/opcodes_65C02.txt > output.cc

Input file format:
    $CPU_NAME          - Define which CPU (e.g., $6502 or $65C02)
    !INSTRUCTION       - Define new instruction (e.g., !ADC, !LDA)
    AddressingMode $opcode bytes cycles [flags...]
                       - Define addressing mode for current instruction

Example input:
    $6502
    !ADC
    Immediate $69 2 2
    ZeroPage $65 2 3
    Absolute,X $7D 3 4 CYCLE_CROSS_PAGE

Copyright (C) 2023 Walt Drummond

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
"""

import sys
import argparse
import re
from typing import Dict, List, Set

# Enable debug output (set to False for production use)
DEBUG = False

# Mapping from short addressing mode names to C++ enum values
ADDRESSING_MODES = {
    'IDX': 'AddressingMode::IndirectX',
    'ZPX': 'AddressingMode::ZeroPageX',
    'ZPY': 'AddressingMode::ZeroPageY',
    'ZP':  'AddressingMode::ZeroPage',
    'IMM': 'AddressingMode::Immediate',
    'IDY': 'AddressingMode::IndirectY',
    'ABY': 'AddressingMode::AbsoluteY',
    'ABX': 'AddressingMode::AbsoluteX',
    'ABS': 'AddressingMode::Absolute',
    'IMP': 'AddressingMode::Implied',
    'ACC': 'AddressingMode::Accumulator',
    'REL': 'AddressingMode::Relative',
    'IND': 'AddressingMode::Indirect',
    'ZPI': 'AddressingMode::ZeroPageIndirect',
    'AII': 'AddressingMode::AbsoluteIndexedIndirect',
}

# Patterns for matching addressing mode strings
# Order matters! More specific patterns must come before general ones
ADDRESSING_MODE_PATTERNS = [
    (r'\(Indirect,X\)',            'IDX'),  # (Indirect,X)
    (r'\(Indirect\),Y',            'IDY'),  # (Indirect),Y
    (r'ZeroPageIndirect',          'ZPI'),  # ZeroPageIndirect
    (r'ZeroPage,X',                'ZPX'),  # ZeroPage,X
    (r'ZeroPage,Y',                'ZPY'),  # ZeroPage,Y
    (r'ZeroPage',                  'ZP'),   # ZeroPage (must be after ZPX, ZPY)
    (r'AbsoluteIndexedIndirect',   'AII'),  # AbsoluteIndexedIndirect
    (r'Absolute,X',                'ABX'),  # Absolute,X
    (r'Absolute,Y',                'ABY'),  # Absolute,Y
    (r'Absolute',                  'ABS'),  # Absolute (must be after ABX, ABY)
    (r'Immediate',                 'IMM'),  # Immediate
    (r'Implied',                   'IMP'),  # Implied
    (r'Accumulator',               'ACC'),  # Accumulator
    (r'Relative',                  'REL'),  # Relative
    (r'Indirect',                  'IND'),  # Indirect
]

# Mapping from flag names in input file to C++ flag constants
FLAG_MAPPING = {
    'CYCLE_CROSS_PAGE':         'InstructionFlags::PageBoundary',
    'CYCLE_BRANCH':             'InstructionFlags::Branch',
    '65C02_SUBTRACT_CYCLE':     'InstructionFlags::NoBoundaryCrossed',
    '65C02_CYCLE_DECIMAL_MODE': 'InstructionFlags::DecimalMode',
}


def parse_addressing_mode(amode_str: str) -> str:
    """
    Convert an addressing mode string to its short code.

    Args:
        amode_str: The addressing mode string from the input file

    Returns:
        The short code (e.g., 'ZPX' for 'ZeroPage,X')

    Raises:
        ValueError: If the addressing mode is not recognized
    """
    for pattern, code in ADDRESSING_MODE_PATTERNS:
        if re.search(pattern, amode_str):
            return code

    raise ValueError(f"Unknown addressing mode: {amode_str}")


def add_flags(flag_dict: Dict[str, int]) -> str:
    """
    Convert a dictionary of flags to a C++ flag expression.

    Args:
        flag_dict: Dictionary of flag names (values are counts, ignored)

    Returns:
        A string like "InstructionFlags::PageBoundary | InstructionFlags::Branch"
        or "InstructionFlags::None" if no flags
    """
    if not flag_dict:
        return "InstructionFlags::None"

    # Join all flag names with " | "
    return " | ".join(sorted(flag_dict.keys()))


def is_cpu_match(cpu_list: Dict[str, int], wanted_cpu: str) -> bool:
    """
    Check if an instruction matches the wanted CPU.

    Args:
        cpu_list: Dictionary of CPUs this instruction applies to
        wanted_cpu: The CPU we're generating code for (or empty for all)

    Returns:
        True if this instruction should be included
    """
    if not wanted_cpu:
        return True

    return wanted_cpu in cpu_list


def calculate_cycle_range(base_cycles: int, flags: Dict[str, int]) -> tuple:
    """
    Calculate minimum and maximum cycles based on flags.

    Args:
        base_cycles: The base cycle count from the opcode definition
        flags: Dictionary of instruction flags

    Returns:
        Tuple of (min_cycles, max_cycles)
    """
    min_cycles = base_cycles
    max_cycles = base_cycles

    # PageBoundary flag: instruction may take one extra cycle
    if 'InstructionFlags::PageBoundary' in flags:
        max_cycles = base_cycles + 1

    # NoBoundaryCrossed flag: base cycles is maximum, subtract 1 for minimum
    # This is for 65C02 RMW (Read-Modify-Write) instructions with Absolute,X addressing
    if 'InstructionFlags::NoBoundaryCrossed' in flags:
        min_cycles = base_cycles - 1

    return min_cycles, max_cycles


def main():
    """Main entry point for the script."""
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description='Process 6502 opcode descriptions into C++ code'
    )
    parser.add_argument(
        '--cpu',
        type=str,
        default='',
        help='CPU type to generate code for (6502 or 65C02)'
    )
    args = parser.parse_args()

    wanted_cpu = args.cpu

    # Storage for all parsed instructions
    # Key: opcode number (0-255)
    # Value: dict with instruction properties
    instructions = {}

    # Current context while parsing
    current_cpu = None
    current_instruction = None
    line_number = 0

    # Read from stdin line by line
    for line in sys.stdin:
        line_number += 1
        line = line.strip()

        if DEBUG:
            print(f"Line {line_number}: {line}", file=sys.stderr)

        # Skip comments and empty lines
        if not line or line.startswith('#'):
            continue

        # CPU definition line: $6502 or $65C02
        if line.startswith('$'):
            current_cpu = line[1:]  # Remove the '$' prefix
            if DEBUG:
                print(f"-- Found CPU: {current_cpu}", file=sys.stderr)
            continue

        # Check that we have a CPU defined
        if current_cpu is None:
            print("Error: CPU not set", file=sys.stderr)
            sys.exit(1)

        # Instruction definition line: !ADC or !LDA
        if line.startswith('!'):
            current_instruction = line[1:]  # Remove the '!' prefix
            if DEBUG:
                print(f"-- Found instruction: {current_instruction}", file=sys.stderr)
            continue

        # Addressing mode line: parse the opcode details
        # Format: AddressingMode Opcode Bytes Cycles [Flags...]
        parts = line.split()
        if len(parts) < 4:
            continue  # Skip malformed lines

        amode_str = parts[0]
        opcode_str = parts[1]
        bytes_str = parts[2]
        cycles_str = parts[3]
        flag_strs = parts[4:] if len(parts) > 4 else []

        # Parse addressing mode
        try:
            amode = parse_addressing_mode(amode_str)
        except ValueError as e:
            print(f"Error on line {line_number}: {e}", file=sys.stderr)
            print(f"  Instruction: {current_instruction}", file=sys.stderr)
            sys.exit(1)

        if not amode:
            print(f"Error: Bad addressing mode on line {line_number}", file=sys.stderr)
            sys.exit(1)

        # Parse opcode (remove '$' prefix and convert hex to int)
        opcode_hex = opcode_str[1:] if opcode_str.startswith('$') else opcode_str
        opcode = int(opcode_hex, 16)

        # Parse bytes and cycles
        num_bytes = int(bytes_str)
        num_cycles = int(cycles_str)

        # If this opcode hasn't been seen before, initialize it
        if opcode not in instructions:
            if DEBUG:
                print("  -- New instruction", file=sys.stderr)

            instructions[opcode] = {
                'ins': current_instruction,
                'op': opcode,
                'amode': amode,
                'bytes': num_bytes,
                'cycles': num_cycles,
                'cpu': {},
                'cycle_flags': {}
            }

        # Mark that this CPU supports this opcode
        instructions[opcode]['cpu'][current_cpu] = 1

        # Parse flags
        if flag_strs:
            if DEBUG:
                print(f"-- Parsing flags: {flag_strs}", file=sys.stderr)
                print(f"-- Existing flags: {list(instructions[opcode]['cycle_flags'].keys())}",
                      file=sys.stderr)

            for flag in flag_strs:
                if DEBUG:
                    print(f"  -- Processing flag: '{flag}'", file=sys.stderr)

                # Convert flag name to C++ constant
                if flag in FLAG_MAPPING:
                    cpp_flag = FLAG_MAPPING[flag]
                    instructions[opcode]['cycle_flags'][cpp_flag] = 1

            if DEBUG:
                print(f"-- New flags: {list(instructions[opcode]['cycle_flags'].keys())}",
                      file=sys.stderr)
        else:
            if DEBUG:
                print("  -- No new flags", file=sys.stderr)

        if DEBUG:
            cpu_list = ', '.join(instructions[opcode]['cpu'].keys())
            flags_list = ', '.join(instructions[opcode]['cycle_flags'].keys())
            print(f"  -- opcode 0x{opcode:x} CPUs {cpu_list} amode {amode} "
                  f"cycles {num_cycles}, {opcode}, flags {flags_list}",
                  file=sys.stderr)

    # Sort opcodes numerically
    sorted_opcodes = sorted(instructions.keys())

    # Generate C++ code sections

    # Section 1: Opcode constant definitions
    print("// --------------------------------------------------------------")
    print("class Opcodes {")
    print("public:")

    for opcode in sorted_opcodes:
        inst = instructions[opcode]
        if not is_cpu_match(inst['cpu'], wanted_cpu):
            continue

        # Create constant name: ADC_ZPX, LDA_ABS, etc.
        const_name = f"{inst['ins']}_{inst['amode']}"
        print(f"\tstatic constexpr Byte {const_name:<7} = 0x{inst['op']:02x};")

    print("};")

    # Section 2: Function shells (empty implementations)
    print("// --------------------------------------------------------------")
    seen_instructions = set()

    for opcode in sorted_opcodes:
        inst = instructions[opcode]
        if not is_cpu_match(inst['cpu'], wanted_cpu):
            continue

        ins_name = inst['ins']
        if ins_name in seen_instructions:
            continue

        seen_instructions.add(ins_name)

        func_name = f"ins_{ins_name.lower()}"
        print(f"void {func_name}([[maybe_unused]] Byte addrmode, "
              f"[[maybe_unused]] Cycles_t& expectedCyclesToUse) {{}}")

    # Section 3: Function prototypes
    print("// --------------------------------------------------------------")
    seen_instructions = set()

    for opcode in sorted_opcodes:
        inst = instructions[opcode]
        if not is_cpu_match(inst['cpu'], wanted_cpu):
            continue

        ins_name = inst['ins']
        if ins_name in seen_instructions:
            continue

        seen_instructions.add(ins_name)

        func_name = f"ins_{ins_name.lower()}"
        print(f"void {func_name}(Byte, Cycles_t&);")

    # Section 4: Instruction map setup
    print("// --------------------------------------------------------------")
    print("std::map<Byte, MOS6502::instruction> MOS6502::setupInstructionMap() {")
    print("\treturn  {")
    print("\t\t// The table below is formatted as follows:")
    print("\t\t// { Opcode,")
    print("\t\t//   {\"name\", AddressingMode, ByteLength, MinCycles, MaxCycles, Flags,")
    print("\t\t//     std::bind(&MOS6502::ins_op, this, std::placeholders::_1, std::placeholders::_2)}}")

    for opcode in sorted_opcodes:
        inst = instructions[opcode]
        if not is_cpu_match(inst['cpu'], wanted_cpu):
            continue

        # Determine which class to use for the function binding
        # If instruction is in multiple CPUs, use the other one
        class_name = ""
        for cpu in inst['cpu'].keys():
            if cpu != wanted_cpu:
                class_name = cpu

        if not class_name:
            class_name = wanted_cpu

        class_prefix = f"&MOS{class_name}::"

        # Build the pieces
        ins_lower = inst['ins'].lower()
        const_name = f"{inst['ins']}_{inst['amode']}"
        amode_cpp = ADDRESSING_MODES[inst['amode']]

        # For 65C02, wrap addressing mode in converter
        if wanted_cpu == '65C02':
            amode_cpp = f"convertAddressingMode({amode_cpp})"

        # Calculate cycle range
        min_cycles, max_cycles = calculate_cycle_range(
            inst['cycles'],
            inst['cycle_flags']
        )

        # Build flags string
        flags_str = add_flags(inst['cycle_flags'])

        # Build function binding
        func_binding = (f"std::bind({class_prefix}ins_{ins_lower}, this, "
                       f"std::placeholders::_1, std::placeholders::_2)")

        # Output the instruction map entry
        # Note: Variable name should be lowercase for consistency with Perl output
        print(f"\t\t{{ Opcodes.{const_name},")
        print(f'\t\t  {{ "{ins_lower}", {amode_cpp}, {inst["bytes"]}, '
              f'{min_cycles}, {max_cycles}, {flags_str},')
        print(f"\t\t    {func_binding}}}}},")

    print("\t};")
    print("}")


if __name__ == '__main__':
    main()
