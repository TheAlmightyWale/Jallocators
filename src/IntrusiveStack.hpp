#pragma once
#include <cstdint>
#include <cstdlib>
#include <assert.h>
#include <functional>

namespace Jalloc
{
    namespace Detail
    {
        template <typename T>
        struct BookKeeping
        {
            uint16_t sizeBytes = sizeof(T);
            std::function<void(void const *)> destructor = ([](void const *p)
                                                            { static_cast<T const *>(p)->~T(); });
        };

        template <>
        struct BookKeeping<void>
        {
            uint16_t sizeBytes;
            std::function<void(void const *)> destructor;
        };
    }
    // Fixed size Intrusive stack class. Can allocate and deallocate memory
    //   all bookkeeping is held between allocations
    //   can optionally provide usage statistics
    class IntrusiveStack
    {
    public:
        IntrusiveStack(uint32_t sizeBytes)
            : maxSizeBytes(sizeBytes)
        {
            base = static_cast<uint8_t*>(std::malloc(maxSizeBytes));
            head = base;
        }

        ~IntrusiveStack()
        {
            std::free(base);
        }

        uint32_t AvailableSpace()
        {
            return static_cast<uint32_t>(maxSizeBytes - (head - base));
        }

        template <typename T, typename... Args>
        [[nodiscard]] T* Alloc(Args &&...args) noexcept
        {
            static_assert(sizeof(T) < std::numeric_limits<uint16_t>::max(), "Can't allocate an individual object larger then 65,535 bytes");
            // optional logging

            // Do size check
            uint32_t attemptedAllocation = sizeof(T) + sizeof(Detail::BookKeeping<T>);
            assert((head + attemptedAllocation) - base < maxSizeBytes);
            T *obj = new (head) T(std::forward<Args>(args)...);
            head += sizeof(T);

            // create bookkeeping
            new(head) Detail::BookKeeping<T>();
            // Attempt to construct object
            head += sizeof(Detail::BookKeeping<T>);

            // return pointer
            return obj;
        }

        void Reset() noexcept
        {
            while (head != base)
            {
                assert(head > base);
                Delete();
            }
        }

        // Deletes the object at the top of the stack
        // Whatever it may be
        void Delete() noexcept
        {
            // grab bookkeeper
            uint8_t *bkStart = head - sizeof(Detail::BookKeeping<void>);
            Detail::BookKeeping<void> *bk = static_cast<Detail::BookKeeping<void> *>((void *)bkStart);
            uint8_t *objStart = bkStart - bk->sizeBytes;
            // call destructor
            bk->destructor(objStart);

            // decrement head
            head = objStart;
        }

    private:
        uint32_t maxSizeBytes = 0;
        uint8_t* base = nullptr;
        uint8_t* head = nullptr;
    };
}