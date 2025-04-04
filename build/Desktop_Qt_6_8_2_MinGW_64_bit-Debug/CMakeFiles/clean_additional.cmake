# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\laCarteu_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\laCarteu_autogen.dir\\ParseCache.txt"
  "laCarteu_autogen"
  )
endif()
