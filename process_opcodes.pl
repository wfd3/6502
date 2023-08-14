#!/usr/bin/perl

$DEBUG = 0;

use feature qw/switch/;;
no warnings 'experimental::smartmatch';

#%i = {};

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
	($amode, $opcode, $bytes, $cycles, $cycle_flags)
	    = split(/ /);
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
	    default                 {
		die("Unknown amode $amode on instruction $ins\n");
	    }
	}

	$opcode = substr($opcode, 1); # Strip $
	$op = hex($opcode);
	
	if (exists($i{$op})) {
	    printf("Opcode 0x%x already in hash\n", $op);
	    printf("Inserting: ins %s, opcode 0x%x, amode %s\n",
		   $ins, $op, $amode);
	    printf("Found    : ins %s, opcode 0x%x, amode %s\n",
		   $i{$op}{"ins"}, $op, $i{$op}{"amode"});
	    die("exiting");
	}	
	
	$opcodes_stored++;
	$i{$op}{"ins"} = $ins;
	$i{$op}{"amode"} = $amode;
	$i{$op}{"bytes"} = $bytes;
	$i{$op}{"cycles"} = $cycles;
	$cycle_flags = "NONE" if ($cycle_flags eq "");
	$i{$op}{"cycle_flags"} = $cycle_flags;
	printf("  -- opcode 0x%x amode $amode cycles $cycles flags $cycle_flags\n", $op) if ($DEBUG);
    }
}

# INS_XXX_MMM definitions
printf("// --------------------------------------------------------------\n");
foreach my $opcode (sort keys(%i)) {
    $ins   = $i{$opcode}{"ins"};
    $amode = $i{$opcode}{"amode"};
    $fn    = "ins_" . lc($ins) . "()";         # ins_xxx() 
    $const = "INS_" . uc($ins) . "_" . $amode; # INS_XXX_MMM 
    printf("const Byte %s = 0x%x;\n", $const, $opcode);
}	

# Prototypes
printf("// --------------------------------------------------------------\n");
foreach my $opcode (sort keys(%i)) {
    $ins   = $i{$opcode}{"ins"};
    $amode = $i{$opcode}{"amode"};
    $fn    = "ins_" . lc($ins) . "()";         # ins_xxx() 
    $const = "INS_" . uc($ins) . "_" . $amode; # INS_XXX_MMM 
    printf("void %s(Byte addrmode) {}\n", $fn);
}

# i[INS_XXX] = makeIns(...);
printf("// --------------------------------------------------------------\n");
foreach my $opcode (sort keys(%i)) {
    $ins    = lc($i{$opcode}{"ins"});
    $amode  = $i{$opcode}{"amode"};
    $fn     = "&CPU::ins_" . lc($ins);		# ins_xxx -- note no '()' 
    $const  = "INS_" . uc($ins) . "_" . $amode; # INS_XXX_MMM
    $cycles = $i{$opcode}{"cycles"};
    $bytes  = $i{$opcode}{"bytes"};
    $flags  = $i{$opcode}{"cycle_flags"};
    
			  
    printf("\tinstructions[%s] = \n\t\tmakeIns(\"%s\", ADDR_MODE_%s, %d, %d, %s,\n\t\t\t %s);\n\n",
	   $const, $ins, $amode, $bytes, $cycles, $flags, $fn);
}


