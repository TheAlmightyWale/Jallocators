#pragma once
#include <concepts>
#include <cstdint>
#include <vector>
#include <bit>

namespace Jalloc
{

uint32_t const k_bitStride = 64;

//BitSet which provides methods to quickly find first unset bit
// or flip a selected bit.
class BitSet
{
public:
    BitSet(uint32_t numBits)
    : _bits((numBits + k_bitStride - 1) / k_bitStride)
    {
        //BitSet uses uint32::max as an error code, so cannot be of that size 
        assert(numBits < std::numeric_limits<uint32_t>::max());
    }
    ~BitSet() = default;

    //Gets first zero and flips it to on
    uint32_t FlipFirstOff()
    {
        for(uint32_t i = 0; i < _bits.size(); i++)
        {
            uint64_t& block = _bits[i];
            //full block, skip it
            if(block == ~0ULL) continue;

            uint32_t pos = std::countr_one(block);
            //found a free bit, flip it
            block |= (1ULL << pos);
            return i * k_bitStride + pos;
        }

        //Should not hit here
        assert(false);
        return std::numeric_limits<uint32_t>::max();
    }

    //Turns off bit at index
    void SetOff(uint32_t index)
    {
        //calculate block this resides in
        uint32_t blockIndex = index / k_bitStride;
        uint64_t bitPos = index % k_bitStride;
        _bits[blockIndex] &= ~(1ULL << bitPos);
    }

    bool Get(uint32_t index) const
    {
        uint32_t blockIndex = index / k_bitStride;
        uint64_t bitPos = index % k_bitStride;
        return (_bits[blockIndex] >> bitPos) & 1ULL;
    }

    void Reset(){
        uint32_t size = (uint32_t)_bits.size();
        _bits.clear();
        _bits.resize(size);
    }

private:
    std::vector<uint64_t> _bits;
};

}