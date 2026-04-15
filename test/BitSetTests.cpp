#include <gtest/gtest.h>
#include "BitSet.h"

using namespace Jalloc;

TEST(BitSet, FirstFlipReturnsZero)
{
    BitSet bs(64);
    EXPECT_EQ(bs.FlipFirstOff(), 0u);
}

TEST(BitSet, SequentialFlipsReturnIncreasingIndices)
{
    BitSet bs(128);
    for(uint32_t i = 0; i < 128; ++i)
    {
        EXPECT_EQ(bs.FlipFirstOff(), i);
    }
}

TEST(BitSet, FlipSetsGetToTrue)
{
    BitSet bs(64);
    uint32_t idx = bs.FlipFirstOff();
    EXPECT_TRUE(bs.Get(idx));
}

TEST(BitSet, UnflippedBitsReturnFalseFromGet)
{
    BitSet bs(64);
    for(uint32_t i = 0; i < 64; ++i)
    {
        EXPECT_FALSE(bs.Get(i));
    }
}

TEST(BitSet, SetOffMakesBitAvailableAgain)
{
    BitSet bs(64);
    uint32_t idx = bs.FlipFirstOff();  // flips bit 0
    bs.SetOff(idx);
    EXPECT_FALSE(bs.Get(idx));
    // next flip should return the same index again
    EXPECT_EQ(bs.FlipFirstOff(), idx);
}

TEST(BitSet, FlipsCorrectlyAcrossBlockBoundary)
{
    // Fill first block entirely, then check second block starts at 64
    BitSet bs(128);
    for(uint32_t i = 0; i < 64; ++i)
    {
        bs.FlipFirstOff();
    }
    EXPECT_EQ(bs.FlipFirstOff(), 64u);
}

TEST(BitSet, SetOffAndReflipAcrossBlockBoundary)
{
    BitSet bs(128);
    for(uint32_t i = 0; i < 65; ++i)
    {
        bs.FlipFirstOff();
    }
    // Turn off bit 63 (last bit of first block)
    bs.SetOff(63);
    EXPECT_FALSE(bs.Get(63));
    // Next free should be 63, not 65
    EXPECT_EQ(bs.FlipFirstOff(), 63u);
}

TEST(BitSet, GetReturnsTrueOnlyForFlippedBits)
{
    BitSet bs(64);
    bs.FlipFirstOff();  // bit 0
    bs.FlipFirstOff();  // bit 1
    bs.FlipFirstOff();  // bit 2

    EXPECT_TRUE(bs.Get(0));
    EXPECT_TRUE(bs.Get(1));
    EXPECT_TRUE(bs.Get(2));
    EXPECT_FALSE(bs.Get(3));
    EXPECT_FALSE(bs.Get(63));
}

TEST(BitSet, NonAlignedSizeAllocatesCorrectly)
{
    // 65 bits requires 2 blocks; should still work without OOB
    BitSet bs(65);
    for(uint32_t i = 0; i < 65; ++i)
    {
        EXPECT_EQ(bs.FlipFirstOff(), i);
    }
}
