# Projekt KameEngine

file(GLOB KAME_ENGINE_SRC
    "*.h"
    "*.cpp"
)

# define KameEngine as Executable Project
add_executable(KameEngine ${KAME_ENGINE_SRC})

# define include directories

# include/lib projet directory
target_include_directories(KameEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# include/lib directory glfw
target_include_directories(KameEngine PRIVATE ${ROOT_DIR}/vendor/glfw-3.3.2.bin.WIN64/include)
target_link_directories(KameEngine PRIVATE ${ROOT_DIR}/vendor/glfw-3.3.2.bin.WIN64/lib-vc2019)
#include/lib directory vulkan
target_include_directories(KameEngine PRIVATE ${ROOT_DIR}/vendor/vulkan/include)
target_link_directories(KameEngine PRIVATE ${ROOT_DIR}/vendor/vulkan/lib)

# Link gflw
target_link_libraries(KameEngine PRIVATE glfw3)

# Link vulkan
target_link_libraries(KameEngine PRIVATE vulkan-1)