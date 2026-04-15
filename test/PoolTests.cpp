#include <gtest/gtest.h>
#include "Utils.h"
#include "Pool.hpp"

using namespace Jalloc;

TEST(Pool, CallsConstructorOnAlloc)
{
    struct Foo
    {
        Foo(int val) : value(val), constructed(true) {}
        int value = 0;
        bool constructed = false;
    };

    Pool<Foo> pool(4);
    Foo* f = pool.Alloc(42);
    EXPECT_TRUE(f->constructed);
    EXPECT_EQ(f->value, 42);
}

TEST(Pool, AllocReturnsNonNull)
{
    Pool<int> pool(4);
    int* p = pool.Alloc(1);
    EXPECT_NE(p, nullptr);
}

TEST(Pool, MultipleAllocsReturnDistinctPointers)
{
    Pool<int> pool(4);
    int* a = pool.Alloc(1);
    int* b = pool.Alloc(2);
    int* c = pool.Alloc(3);

    EXPECT_NE(a, b);
    EXPECT_NE(b, c);
    EXPECT_NE(a, c);
}

TEST(Pool, AllocsAreContiguousAndSized)
{
    Pool<int> pool(4);
    int* a = pool.Alloc(0);
    int* b = pool.Alloc(0);

    // Each slot must be exactly sizeof(T) bytes apart
    EXPECT_EQ(b - a, 1);
}

TEST(Pool, CallsDestructorOnFree)
{
    static bool destroyed = false;
    struct Foo
    {
        ~Foo() { destroyed = true; }
    };

    Pool<Foo> pool(4);
    Foo* f = pool.Alloc();
    EXPECT_FALSE(destroyed);
    pool.Free(f);
    EXPECT_TRUE(destroyed);
}

TEST(Pool, FreedSlotCanBeReallocated)
{
    Pool<int> pool(1);
    int* a = pool.Alloc(10);
    pool.Free(a);
    int* b = pool.Alloc(20);

    // Only one slot — must reuse the same address
    EXPECT_EQ(a, b);
    EXPECT_EQ(*b, 20);
}

TEST(Pool, CallsDestructorOnReset)
{
    static int destroyCount = 0;
    struct Foo
    {
        ~Foo() { destroyCount++; }
    };

    destroyCount = 0;
    Pool<Foo> pool(4);
    JALLOC_UNUSED(pool.Alloc());
    JALLOC_UNUSED(pool.Alloc());
    JALLOC_UNUSED(pool.Alloc());

    pool.Reset();
    EXPECT_EQ(destroyCount, 3);
}

TEST(Pool, ResetOnlyDestroysLiveObjects)
{
    static int destroyCount = 0;
    struct Foo
    {
        ~Foo() { destroyCount++; }
    };

    destroyCount = 0;
    Pool<Foo> pool(4);
    Foo* a = pool.Alloc();
    JALLOC_UNUSED(pool.Alloc());
    pool.Free(a); // destroyCount -> 1

    pool.Reset(); // only 1 remaining live object
    EXPECT_EQ(destroyCount, 2);
}

TEST(Pool, ResetAllowsReuse)
{
    Pool<int> pool(2);
    JALLOC_UNUSED(pool.Alloc(1));
    JALLOC_UNUSED(pool.Alloc(2));
    pool.Reset();

    int* a = pool.Alloc(10);
    int* b = pool.Alloc(20);
    EXPECT_NE(a, nullptr);
    EXPECT_NE(b, nullptr);
    EXPECT_EQ(*a, 10);
    EXPECT_EQ(*b, 20);
}

TEST(Pool, DestructorCallsDestructorsOnLiveObjects)
{
    static bool destroyed = false;
    struct Foo
    {
        ~Foo() { destroyed = true; }
    };

    {
        Pool<Foo> pool(4);
        JALLOC_UNUSED(pool.Alloc());
    }

    EXPECT_TRUE(destroyed);
}
