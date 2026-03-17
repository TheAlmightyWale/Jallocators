#pragma once
#include "IntrusiveStack.hpp"

namespace Jalloc
{
    //Scratch allocator. A stack that you cannot free memory from, unless you free all of it's memory at once
    class Scratch
    {
    public:
        Scratch(uint32_t size) : m_stack(size)
        {}

        ~Scratch() = default;

        template <typename T, typename... Args>
        [[nodiscard]] T* Alloc(Args &&...args) noexcept
        {
            return m_stack.Alloc<T,Args...>(std::forward(args)...);
        }

        uint32_t AvailableSpace()
        {
            return m_stack.AvailableSpace();
        }

        void Reset(){
            m_stack.Reset();
        }

    private:
        IntrusiveStack m_stack;
    };
}