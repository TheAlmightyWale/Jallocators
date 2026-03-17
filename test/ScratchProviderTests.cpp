#include <gtest/gtest.h>
#include "Utils.h"
#include "ScratchProvider.hpp"

using namespace Jalloc;

TEST(ScratchProvider, AllocateSomeNoOverhead)
{
    ScratchProvider alloc(32);
    EXPECT_EQ(alloc.AvailableSpace(), 32);

    void* mem = alloc.Alloc(4);
    JALLOC_UNUSED(mem);

    EXPECT_EQ(alloc.AvailableSpace(), 28);

    void* mem2 = alloc.Alloc(12);
    JALLOC_UNUSED(mem2);

    EXPECT_EQ(alloc.AvailableSpace(), 16);
}

TEST(ScratchProvider, DeallocateAll)
{
        ScratchProvider alloc(32);
    EXPECT_EQ(alloc.AvailableSpace(), 32);

    void* mem = alloc.Alloc(4);
    JALLOC_UNUSED(mem);

    EXPECT_EQ(alloc.AvailableSpace(), 28);

    void* mem2 = alloc.Alloc(12);
    JALLOC_UNUSED(mem2);

    EXPECT_EQ(alloc.AvailableSpace(), 16);

    alloc.Reset();
    EXPECT_EQ(alloc.AvailableSpace(), 32);
}
