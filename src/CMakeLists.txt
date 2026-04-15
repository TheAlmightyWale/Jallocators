set(LIBRARY_NAME "Jallocators")

add_library(${LIBRARY_NAME} "BitSet.h" "IntrusiveStack.hpp" "Pool.hpp" "Scratch.hpp" "ScratchProvider.hpp" "Utils.h")

set_target_properties(${LIBRARY_NAME} PROPERTIES CXX_STANDARD 20 LINKER_LANGUAGE CXX)
