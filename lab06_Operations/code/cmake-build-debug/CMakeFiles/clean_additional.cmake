# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "labo6_gui/CMakeFiles/PCO_lab06_gui_autogen.dir/AutogenUsed.txt"
  "labo6_gui/CMakeFiles/PCO_lab06_gui_autogen.dir/ParseCache.txt"
  "labo6_gui/PCO_lab06_gui_autogen"
  "labo6_tests/CMakeFiles/PCO_lab06_tests_autogen.dir/AutogenUsed.txt"
  "labo6_tests/CMakeFiles/PCO_lab06_tests_autogen.dir/ParseCache.txt"
  "labo6_tests/PCO_lab06_tests_autogen"
  )
endif()
