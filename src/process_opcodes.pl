#!/bin/perl
# Process 6502 opcode descriptions into various C++ code snippets.
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

$DEBUG = 0;

use feature qw/switch/;;
no warnings 'experimental::smartmatch';

$AddressingMode{"IDX"} = "AddressingMode::IndirectX";
$AddressingMode{"ZPX"} = "AddressingMode::ZeroPageX";
$AddressingMode{"ZPY"} = "AddressingMode::ZeroPageY";
$AddressingMode{"ZP"}  = "AddressingMode::ZeroPage";
$AddressingMode{"IMM"} = "AddressingMode::Immediate";
$AddressingMode{"IDY"} = "AddressingMode::IndirectY";
$AddressingMode{"ABY"} = "AddressingMode::AbsoluteY";
$AddressingMode{"ABX"} = "AddressingMode::AbsoluteX";
$AddressingMode{"ABS"} = "AddressingMode::Absolute";
$AddressingMode{"IMP"} = "AddressingMode::Implied";
$AddressingMode{"ACC"} = "AddressingMode::Accumulator";
$AddressingMode{"REL"} = "AddressingMode::Relative";
$AddressingMode{"IND"} = "AddressingMode::Indirect";

$opcodes_stored = 0;
while (<>) {
    chomp();
    s/^\s+|\s+$//;    
    next if (/^#/ || /^$/);
    
    if (/^!/) { # New instruction
		$ins = substr($_, 1);
		printf("-- Found instruction: $ins\n") if ($DEBUG);
		next;
    } else {	# Addressing modes for previous instruction
	($amode, $opcode, $bytes, $cycles, $cycle_flags) = split(/ /);
	given ($amode) {
	    when(/\(Indirect\,X\)/) { $amode = "IDX"; } # (Indirect,X)
	    when(/ZeroPage\,X/)     { $amode = "ZPX"; } # ZeroPage,X
	    when(/ZeroPage\,Y/)     { $amode = "ZPY"; } # ZeroPage,Y
	    when(/ZeroPage/)        { $amode = "ZP"; }  # ZeroPage
	    when(/Immediate/)       { $amode = "IMM"; } # Immediate
	    when(/\(Indirect\)\,Y/) { $amode = "IDY"; } # (Indirect),Y
	    when(/Absolute\,Y/)     { $amode = "ABY"; } # Absolute,Y
	    when(/Absolute\,X/)     { $amode = "ABX"; } # Absolute,X
	    when(/Absolute/)        { $amode = "ABS"; } # Absolute
	    when(/Implied/)         { $amode = "IMP"; } # Implied
	    when(/Accumulator/)     { $amode = "ACC"; } # Accumulator
	    when(/Relative/)        { $amode = "REL"; } # Relative
	    when(/Indirect/)        { $amode = "IND"; } # Indirect
	    default                 { die("Unknown amode $amode on instruction $ins\n"); }
	}

	$opcode = substr($opcode, 1); # Strip $
	$op = hex($opcode);
	
	if (exists($i{$op})) {
	    printf("Opcode 0x%02x already in hash\n", $op);
	    printf("Inserting: ins %s, opcode 0x%02x, amode %s\n",
		   $ins, $op, $amode);
	    printf("Found    : ins %s, opcode 0x%02x, amode %s\n",
		   $i{$op}{"ins"}, $op, $i{$op}{"amode"});
	    die("exiting");
	}	
	
	$opcodes_stored++;
	$i{$op}{"ins"} = $ins;
	$i{$op}{"amode"} = $amode;
	$i{$op}{"bytes"} = $bytes;
	$i{$op}{"cycles"} = $cycles;
	$cycle_flags = "InstructionFlags::NONE" if ($cycle_flags eq "");
	$cycle_flags = "InstructionFlags:CyclePage" if ($cycle_flags eq "CYCLE_CROSS_PAGE");
	$cycle_flags = "InstructionFlags:Branch" if ($cycle_flags eq "CYCLE_BRANCH");
	
	$i{$op}{"cycle_flags"} = $cycle_flags;
	printf("  -- opcode 0x%x amode $amode cycles $cycles flags $cycle_flags\n", $op) if ($DEBUG);
    }
}

# Numerically sort the opcodes
@sorted_ops = sort { $a <=> $b } keys %i;

# XXX_MMM definitions
printf("// --------------------------------------------------------------\n");
printf("class Opcodes {\n");
printf("public:\n");
foreach my $opcode (@sorted_ops) {
    $ins   = $i{$opcode}{"ins"};
    $amode = $i{$opcode}{"amode"};
    $const = uc($ins) . "_" . $amode;   # XXX_MMM 
    printf("\tconstexpr Byte %-7s = 0x%02x;\n", $const, $opcode);
}	
printf("};\n");

# Function shells
printf("// --------------------------------------------------------------\n");
foreach my $opcode (@sorted_ops) {
    $ins   = $i{$opcode}{"ins"};
    $amode = $i{$opcode}{"amode"};
    $fn    = "ins_" . lc($ins) . "()"; # ins_xxx() 
    $const = uc($ins) . "_" . $amode;  # XXX_MMM 
    printf("void %s([[maybe__unused]] Byte addrmode,\n\t[[maybe_unused]] Cycles_t& expectedCyclesToUse) {}\n", $fn);
}

# Function prototypes
printf("// --------------------------------------------------------------\n");
foreach my $opcode (@sorted_ops) {
    $ins   = $i{$opcode}{"ins"};
    $amode = $i{$opcode}{"amode"};
    $fn    = "ins_" . lc($ins) . "()"; # ins_xxx() 
    $const = uc($ins) . "_" . $amode;  # XXX_MMM 
    printf("void %s(Byte, Cycles_t&);\n", $fn);
}

# i[XXX] = makeIns(...);
printf("// --------------------------------------------------------------\n");
printf("std::map<Byte, MOS6502::instruction> MOS6502::setupInstructionMap() {\n");
printf("\treturn  {\n");
printf("\t\t// The table below is formatted as follows:\n");
printf("\t\t// { Opcode,\n");
printf("\t\t//   {\"name\", AddressingMode, ByteLength, CyclesUsed, Flags, Function pointer for instruction}}\n");

foreach my $opcode (@sorted_ops) {
    $ins    = lc($i{$opcode}{"ins"});
    $amode  = $i{$opcode}{"amode"};
	$addrm  = $AddressingMode{$amode};
    $fn     = "&MOS6502::" . lc($ins) . "_" . lc($amode);		# ins_xxx -- note no '()' 
    $const  = uc($ins) . "_" . $amode; # XXX_MMM
    $cycles = $i{$opcode}{"cycles"};
    $bytes  = $i{$opcode}{"bytes"};
    $flags  = $i{$opcode}{"cycle_flags"};
    
	printf("\t\t{ Opcodes::%s,\n", $const);
	printf("\t\t  { \"%s\", %s, %d, %d, %s, %s }},\n",
		$ins, $addrm, $bytes, $cyces, $flags, $fn);
}
printf("\t};\n");
printf("}\n");



