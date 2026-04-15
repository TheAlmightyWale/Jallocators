#pragma once
#include <cstdint>
#include <cstdlib>
#include <assert.h>
#include "Utils.h"

namespace Jalloc
{
    namespace Detail
    {
        template <typename T>
        struct BookKeeping
        {
            uint16_t sizeBytes = sizeof(T);
            Destructor destructor = {this, // address of object
                                [](const void* x) { static_cast<const T*>(x)->~T(); }};
        };

        template <>
        struct BookKeeping<void>
        {
            uint16_t sizeBytes;
            Destructor destructor;
        };
    }
    // Fixed size Intrusive stack class. Can allocate and deallocate memory
    //   all bookkeeping is held between allocations
    //   can optionally provide usage statistics
    class IntrusiveStack
    {
    public:
        IntrusiveStack(uint32_t sizeBytes)
            : maxSizeBytes_(sizeBytes)
        {
            base_ = static_cast<uint8_t*>(std::malloc(maxSizeBytes_));
            head_ = base_;
        }

        ~IntrusiveStack()
        {
            Reset();
            std::free(base_);
        }

        uint32_t AvailableSpace()
        {
            return static_cast<uint32_t>(maxSizeBytes_ - (head_ - base_));
        }

        template <typename T, typename... Args>
        [[nodiscard]] T* Alloc(Args &&...args) noexcept
        {
            static_assert(sizeof(T) < std::numeric_limits<uint16_t>::max(), "Can't allocate an individual object larger then 65,535 bytes");
            // optional logging

            // Do size check
            uint32_t attemptedAllocation = sizeof(T) + sizeof(Detail::BookKeeping<T>);
            assert((head_ + attemptedAllocation) - base_ < maxSizeBytes_);
            T *obj = new (head_) T(std::forward<Args>(args)...);
            head_ += sizeof(T);

            // create bookkeeping
            new(head_) Detail::BookKeeping<T>();
            // Attempt to construct object
            head_ += sizeof(Detail::BookKeeping<T>);

            // return pointer
            return obj;
        }

        void Reset() noexcept
        {
            while (head_ != base_)
            {
                assert(head_ > base_);
                Delete();
            }
        }

        // Deletes the object at the top of the stack
        // Whatever it may be
        void Delete() noexcept
        {
            // grab bookkeeper
            uint8_t *bkStart = head_ - sizeof(Detail::BookKeeping<void>);
            Detail::BookKeeping<void> *bk = static_cast<Detail::BookKeeping<void> *>((void *)bkStart);
            uint8_t *objStart = bkStart - bk->sizeBytes;
            // call destructor
            bk->destructor.destroy(bk->destructor.p);

            // decrement head
            head_ = objStart;
        }

    private:
        uint32_t maxSizeBytes_ = 0;
        uint8_t* base_ = nullptr;
        uint8_t* head_ = nullptr;
    };
}