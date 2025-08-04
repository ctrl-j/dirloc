# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/dirloc_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/dirloc_autogen.dir/ParseCache.txt"
  "dirloc_autogen"
  )
endif()
