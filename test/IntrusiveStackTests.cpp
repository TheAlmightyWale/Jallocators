#include <gtest/gtest.h>
#include "Utils.h"
#include "IntrusiveStack.hpp"

using namespace Jalloc;

TEST(IntrusiveStack, CallConstructorOnAllocation)
{
    struct DoThinger
    {
        DoThinger(int arg)
        :arg(arg)
        {
            constructorWasRun = true;
        }

        bool constructorWasRun = false;
        int arg = 0;
    };

    IntrusiveStack stack(256);
    DoThinger* dt = stack.Alloc<DoThinger>(1);
    EXPECT_TRUE(dt->constructorWasRun);
    EXPECT_EQ(dt->arg, 1);
}

TEST(IntrusiveStack, CallDestructorOnAllocation)
{
    static bool baseDestroyed = false;
    static bool derivedDestroyed = false;

    struct Base{
        ~Base(){
            baseDestroyed = true;
        }
    };

    struct Derived: public Base{
        ~Derived(){
            derivedDestroyed = true;
        }
    };

    IntrusiveStack stack(256);
    Base* b = stack.Alloc<Derived>();
    JALLOC_UNUSED(b);

    stack.Delete();
    EXPECT_TRUE(baseDestroyed);
    EXPECT_TRUE(derivedDestroyed);
}

TEST(IntrusiveStack, DeletesTop)
{
    IntrusiveStack stack(256);
    JALLOC_UNUSED(stack.Alloc<int>());
    JALLOC_UNUSED(stack.Alloc<int64_t>());

    stack.Delete();
    EXPECT_EQ(stack.AvailableSpace(),180);
}

TEST(IntrusiveStack, ResetsAll)
{
    IntrusiveStack stack(256);
    JALLOC_UNUSED(stack.Alloc<int>());
    JALLOC_UNUSED(stack.Alloc<int64_t>());

    stack.Reset();
    EXPECT_EQ(stack.AvailableSpace(),256);
}
