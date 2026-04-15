#include <gtest/gtest.h>
#include "Scratch.hpp"
#include "Utils.h"

using namespace Jalloc;

TEST(Scratch, AllocateSome)
{
    Scratch alloc(256);
    EXPECT_EQ(alloc.AvailableSpace(), 256);

    struct Data{
        uint32_t num;
    };

    void* data = alloc.Alloc<Data>();
    JALLOC_UNUSED(data);

    EXPECT_EQ(alloc.AvailableSpace(), 228);
}

TEST(Scratch, DeallocateAll)
{
    Scratch alloc(256);
    EXPECT_EQ(alloc.AvailableSpace(), 256);

    struct Data{
        uint32_t num;
    };

    void* data = alloc.Alloc<Data>();
    JALLOC_UNUSED(data);
    EXPECT_EQ(alloc.AvailableSpace(), 228);

    alloc.Reset();
    EXPECT_EQ(alloc.AvailableSpace(), 256);
}
