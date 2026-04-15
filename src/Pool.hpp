#pragma once
#include <cassert>
#include <cstdint>
#include <limits>
#include <cstdlib>

#include "BitSet.h"

namespace Jalloc
{
    //Pool allocator which constructs and destructs objects
    template<typename T>
    class Pool
    {
    public:
        Pool(uint32_t numElements)
        : poolSize_(numElements)
        , tracker_(numElements)
        {
            //initial allocation
            base_ = static_cast<T*>(std::malloc(poolSize_ * sizeof(T)));
        }

        ~Pool(){
            Reset();
            std::free(base_);
        }

        template <typename... Args>
        [[nodiscard]] T* Alloc(Args&&... args) noexcept
        {
            uint32_t pos = tracker_.FlipFirstOff();
            assert(pos != std::numeric_limits<uint32_t>::max());

            T* alloc = base_ + pos;
            return new (alloc) T(std::forward<Args>(args)...);
        }

        void Free(T* p)
        {
            //should only attempt to free something in this pool!
            assert(base_ <= p && p < base_ + poolSize_);

            //find position in pool based on pointer
            uint32_t freeIndex = (uint32_t)(p - base_);
            tracker_.SetOff(freeIndex); 
            
            //call destructor
            p->~T();

        }

        void Reset(){
            // iterate through pool, calling destructors
            for(uint32_t i = 0; i < poolSize_; i++){
                if(tracker_.Get(i)){
                    T* item = base_ + i;
                    item->~T();
                }
            }
            tracker_.Reset();
        }

    private:
        T* base_ = nullptr;
        uint32_t poolSize_ = 0;
        BitSet tracker_;
    };
}