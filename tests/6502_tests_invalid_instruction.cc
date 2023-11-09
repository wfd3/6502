#include <gtest/gtest.h>
#include <6502.h>

class MOS6502OpcodeTests : public testing::Test {
public:

        Memory<Address_t, Byte> mem{CPU::MAX_MEM};
        CPU cpu{mem};

        virtual void SetUp() {
                mem.mapRAM(0, CPU::MAX_MEM);
        }
        
        virtual void TearDown() {
        }
};

TEST_F(MOS6502OpcodeTests, InvalidOpcodeThrowsException) {
    bool caughtRuntimeException = false;
    bool caughtOtherException = false;
    uint64_t UsedCycles, ExpectedCycles;
    Byte ins = 0xff;  

    //Given:
    cpu.TestReset(CPU::RESET_VECTOR);
    mem[0xFFFC] = ins;

    try {
        //When:
        std::tie(UsedCycles, ExpectedCycles) = cpu.executeOneInstruction();
    }

    catch ([[maybe_unused]] const std::runtime_error& e) {
        caughtRuntimeException = true;
    }

    catch (...) {
        caughtOtherException = true;
    }

    EXPECT_TRUE(caughtRuntimeException);
    EXPECT_FALSE(caughtOtherException);
}
