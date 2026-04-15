#pragma once
namespace Jalloc 
{
   #define JALLOC_UNUSED(x) (void)(x) 

   //Thanks Herb Sutter, https://herbsutter.com/2016/09/25/to-store-a-destructor/
   struct Destructor {
    const void* p;
    void(*destroy)(const void*);
   };
}