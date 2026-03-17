- Replace std::function with c style function pointer for holding destructor calls. To remove extra memory overhead used by std::function

- Aligned memory allocations to alginof(std::max_align_t)

- Page aligned backing slabs