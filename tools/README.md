# Opcode Processing Tools

This directory contains scripts for generating C++ code from 6502/65C02 opcode definitions.

## Scripts

### process_opcodes.py (Recommended)

Modern Python 3 implementation. Requires only standard library (no external dependencies).

**Usage:**
```bash
# Generate 6502 opcode map
python3 process_opcodes.py --cpu=6502 < ../6502/opcodes_6502.txt > output.cc

# Generate 65C02 opcode map
python3 process_opcodes.py --cpu=65C02 < ../65C02/opcodes_65C02.txt > output.cc
```

**Features:**
- Clear, well-documented code for Python beginners
- Type hints for better IDE support
- No external dependencies
- Identical output to Perl version

### process_opcodes.pl (Legacy)

Original Perl implementation. Requires Perl 5 with Getopt::Long.

**Usage:**
```bash
# Generate 6502 opcode map
perl process_opcodes.pl --cpu=6502 < ../6502/opcodes_6502.txt > output.cc

# Generate 65C02 opcode map
perl process_opcodes.pl --cpu=65C02 < ../65C02/opcodes_65C02.txt > output.cc
```

**Note:** Uses deprecated `given/when` syntax (may break in future Perl versions)

## Input File Format

Opcode definition files use a simple text format:

```
$CPU_NAME              # Define CPU type (6502 or 65C02)
!INSTRUCTION           # Define instruction name (ADC, LDA, etc.)
AddressingMode $opcode bytes cycles [flags...]
```

### Example

```
$6502
!ADC
Immediate $69 2 2
ZeroPage $65 2 3
Absolute,X $7D 3 4 CYCLE_CROSS_PAGE
```

### Supported Flags

- `CYCLE_CROSS_PAGE` - Add 1 cycle if page boundary crossed
- `CYCLE_BRANCH` - Variable cycles for branch instructions
- `65C02_SUBTRACT_CYCLE` - RMW instructions (65C02 only)
- `65C02_CYCLE_DECIMAL_MODE` - Extra cycle in decimal mode (65C02 only)

## Output

The scripts generate three C++ code sections:

1. **Opcode Constants** - `static constexpr Byte XXX_YYY = 0x##;`
2. **Function Prototypes** - `void ins_xxx(Byte, Cycles_t&);`
3. **Instruction Map** - `std::map` initialization with all opcode details

## Development

To modify the Python script:
1. Read the inline documentation and docstrings
2. Set `DEBUG = True` (line 31) to see parsing details
3. Test against both 6502 and 65C02 opcode files
4. Compare output with Perl version to ensure compatibility

## Migration Note

Both scripts produce identical output. The Python version is recommended for new development due to:
- Better maintainability
- No dependency on deprecated language features
- Clearer code structure
- Better documentation
