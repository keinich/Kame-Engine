cmake_minimum_required(VERSION "3.16.2" FATAL_ERROR)

# Solution
project("Kame" LANGUAGES CXX)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Projekt KameEngine

# Verzeichnis KameEngine muss CMakeLists.txt enthalten
add_subdirectory(KameEngine)

# target_link_libraries(KameEngine PRIVATE MathLib)