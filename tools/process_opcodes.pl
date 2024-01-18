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

$DEBUG = 1;
$STOP = 0;

use feature qw/switch/;;
no warnings 'experimental::smartmatch';
use Getopt::Long;

my $wantcpu;

GetOptions(
		'cpu=s'	=> \$wantcpu
);

sub mycpu {
	my (%cpulist) = @_;
	return 1 if ($wantcpu eq "");

	foreach my $c (keys %cpulist) {
		return 1 if ($c eq $wantcpu);
	}
	return 0;
}

sub addFlags {
	my ($flags_ref, %flg) = @_;
	$added = 0;
	foreach my $key (keys %flg) {
		$$flags_ref .= " & " if ($$flags_ref ne "");
		$$flags_ref .= $key;
		$added++;
	}
	$$flags_ref = "InstructionFlags::None" if ($added == 0);
}

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
$AddressingMode{"ZPI"} = "AddressingMode::ZeroPageIndirect";
$AddressingMode{"AII"} = "AddressingMode::AbsoluteIndexedIndirect";

$opcodes_stored = 0;
$line = 0;

while (<>) {
	$line++;
    chomp();
    s/^\s+|\s+$//;

	printf("Line $line: $_\n") if ($DEBUG);

    next if (/^#/ || /^$/);


	if (/^\$/) { # New CPU
		$cpu = substr($_, 1);
		printf("-- Found CPU: $cpu\n") if ($DEBUG);
		next;
	}

	die "CPU not set\n" if ($cpu eq "");

    if (/^!/) { # New instruction
		$ins = substr($_, 1);
		printf("-- Found instruction: $ins\n") if ($DEBUG);
		next;
    }

	# Addressing modes for previous instruction
	($amode, $opcode, $bytes, $cycles, @flags) = split(/ /);
	given ($amode) {
	    when(/\(Indirect\,X\)/) { $amode = "IDX"; } # (Indirect,X)
		when(/ZeroPageIndirect/){ $amode = "ZPI"; } # ZeroPage Indirect
	    when(/ZeroPage\,X/)     { $amode = "ZPX"; } # ZeroPage,X
	    when(/ZeroPage\,Y/)     { $amode = "ZPY"; } # ZeroPage,Y
	    when(/ZeroPage/)        { $amode = "ZP"; }  # ZeroPage
	    when(/Immediate/)       { $amode = "IMM"; } # Immediate
	    when(/\(Indirect\)\,Y/) { $amode = "IDY"; } # (Indirect),Y
	    when(/AbsoluteIndexedIndirect/){ $amode = "AII"; } # Absolute Indexed Indirect
		when(/Absolute\,Y/)     { $amode = "ABY"; } # Absolute,Y
	    when(/Absolute\,X/)     { $amode = "ABX"; } # Absolute,X
	    when(/Absolute/)        { $amode = "ABS"; } # Absolute
	    when(/Implied/)         { $amode = "IMP"; } # Implied
	    when(/Accumulator/)     { $amode = "ACC"; } # Accumulator
	    when(/Relative/)        { $amode = "REL"; } # Relative
	    when(/Indirect/)        { $amode = "IND"; } # Indirect
	    default                 { die("Unknown amode $amode on instruction $ins\n"); }
	}
	die "Bad amode on instruction" if (length($amode) == 0);

	$opcode = substr($opcode, 1); # Strip $
	$op = hex($opcode);
	$key = $op;

	if (!exists($i{$key})) {
		printf("  -- New instruction\n") if ($DEBUG);
	    $opcodes_stored++;
		$i{$key}{"ins"} = $ins;
		$i{$key}{"op"} = $op;
		$i{$key}{"amode"} = $amode;
		$i{$key}{"bytes"} = $bytes;
		$i{$key}{"cycles"} = $cycles;
	}
	$i{$key}{"cpu"}{$cpu}++;

	$l = scalar @flags;
	if ($l > 0) {
		if ($DEBUG) {
			printf("-- Parsing flags : @flags\n");
			printf("-- Existing flags: ");
			print join(", ", keys %{$i{$key}{"cycle_flags"}}), "\n"
		}

		foreach my $flg (@flags) {
			printf("  -- Processing flg: '$flg'\n") if ($DEBUG);

			$i{$key}{"cycle_flags"}{"InstructionFlags::PageBoundary"}++ if ($flg eq "CYCLE_CROSS_PAGE");
			$i{$key}{"cycle_flags"}{"InstructionFlags::Branch"}++ if ($flg eq "CYCLE_BRANCH");
			$i{$key}{"cycle_flags"}{"InstructionFlags::BranchMinusCycle"}++ if ($flg eq "65C02_SUBTRACT_CYCLE");
		}
		if ($DEBUG) {
			printf("-- new flags = ");
			print join(", ", keys %{$i{$key}{"cycle_flags"}}), "\n"
		}
	} else {
		printf("  -- No new flags\n");
	}

	if ($DEBUG) {
		$c = join(", ", keys %{$i{$key}{"cpu"}});
		printf("  -- opcode 0x%x CPUs $c amode $amode cycles $cycles, $op, flags ");
		print join(", ", keys %{$i{$key}{"cycle_flags"}}), "\n"
	}
}

exit(0) if ($STOP);

# Numerically sort the opcodes
@sorted_ops = sort { $a <=> $b } keys %i;

# XXX_MMM definitions
printf("// --------------------------------------------------------------\n");
printf("class Opcodes {\n");
printf("public:\n");
foreach my $key (@sorted_ops) {
	next if !mycpu(%{$i{$key}{"cpu"}});
	$op    = $i{$key}{"op"};
    $ins   = $i{$key}{"ins"};
    $amode = $i{$key}{"amode"};
    $const = uc($ins) . "_" . $amode;   # XXX_MMM
    printf("\tstatic constexpr Byte %-7s = 0x%02x;\n", $const, $op);
}
printf("};\n");

# Function shells
printf("// --------------------------------------------------------------\n");
%seen = ();
foreach my $key (@sorted_ops) {
	next if !mycpu(%{$i{$key}{"cpu"}});
    $ins   = $i{$key}{"ins"};
	next if (exists $seen{$ins});

	$op    = $i{$key}{"op"};
    $amode = $i{$key}{"amode"};
    $fn    = "ins_" . lc($ins) . "()"; # ins_xxx()
    $const = uc($ins) . "_" . $amode;  # XXX_MMM

	$seen{$ins} = 1;

	printf("void %s([[maybe__unused]] Byte addrmode, [[maybe_unused]] Cycles_t& expectedCyclesToUse) {}\n", $fn);
}

# Function prototypes
%seen = ();
printf("// --------------------------------------------------------------\n");
foreach my $key (@sorted_ops) {
	next if !mycpu(%{$i{$key}{"cpu"}});
    $ins   = $i{$key}{"ins"};
	next if (exists $seen{$ins});

	$op    = $i{$key}{"op"};
    $amode = $i{$key}{"amode"};
    $fn    = "ins_" . lc($ins); # ins_xxx()
    $const = uc($ins) . "_" . $amode;  # XXX_MMM

	$seen{$ins} = 1;

	printf("void %s(Byte, Cycles_t&);\n", $fn);
}

# i[XXX] = makeIns(...);
printf("// --------------------------------------------------------------\n");
printf("std::map<Byte, MOS6502::instruction> MOS6502::setupInstructionMap() {\n");
printf("\treturn  {\n");
printf("\t\t// The table below is formatted as follows:\n");
printf("\t\t// { Opcode,\n");
printf("\t\t//   {\"name\", AddressingMode, ByteLength, CyclesUsed, Flags,\n");
printf("\t\t//     std::bind(&MOS6502::ins_op, this, std::placeholders::_1, std::placeholders::_2)}}\n");

foreach my $key (@sorted_ops) {
	next if !mycpu(%{$i{$key}{"cpu"}});
	$op     = $i{$key}{"op"};
	$cpu    = $i{$key}{"cpu"};
    $ins    = lc($i{$key}{"ins"});
    $amode  = $i{$key}{"amode"};
	$addrm  = $AddressingMode{$amode};
	
	$class = "";
	for my $c (keys %{$i{$key}{"cpu"}}) {
		if ($c ne $wantcpu) {
			$class = $c;
		}
	}
	if ($class eq "") {
		$class = $wantcpu;
	}

	$class = "&MOS" . $class . "::";	
	$opfn = "ins_" . lc($ins);
    $fn     = "std::bind(" . $class . $opfn .", this, std::placeholders::_1, std::placeholders::_2)";
	$const  = uc($ins) . "_" . $amode; # XXX_MMM
    $cycles = $i{$key}{"cycles"};
    $bytes  = $i{$key}{"bytes"};
	$flags  = "";
	addFlags(\$flags, %{$i{$key}{"cycle_flags"}});
	if ($wantcpu eq "65C02") {
		$addrm = "convertAddressingMode(" . $addrm . ")";
	}

	printf("\t\t{ " . $Class . "Opcodes.%s,\n", $const);
	printf("\t\t  { \"%s\", %s, %d, %d, %s,\n",	$ins, $addrm, $bytes, $cycles, $flags);
	printf("\t\t    %s}},\n", $fn);
}
printf("\t};\n");
printf("}\n");



