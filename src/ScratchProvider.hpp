#pragma once
#include <cstdint>
#include <cstdlib>
#include <assert.h>

namespace Jalloc
{
    //Scratch allocator which does not call constructor or destructors, just provides raw memory
    class ScratchProvider
    {
    public:
        ScratchProvider(uint32_t sizeBytes)
        : maxSizeBytes_(sizeBytes)
        {
            base_ = static_cast<uint8_t*>(std::malloc(maxSizeBytes_));
            head_ = base_;
        }

        ~ScratchProvider() = default;

        [[nodiscard]] void* Alloc(std::size_t sizeBytes) noexcept
        {
            uint8_t* current = head_;
            head_ += sizeBytes;
            assert((head_ + sizeBytes) - base_ < maxSizeBytes_);

            return current;
        }

        uint32_t AvailableSpace()
        {
            return static_cast<uint32_t>(maxSizeBytes_ - (head_ - base_));
        }

        void Reset(){
            head_ = base_;    
        }

    private:
        uint32_t maxSizeBytes_ = 0;
        uint8_t* base_ = nullptr;
        uint8_t* head_ = nullptr;
    };
}