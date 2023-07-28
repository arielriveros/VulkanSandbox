cmake_minimum_required(VERSION 3.16)
project(VulkanSandbox CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(EXTERNAL_LIBS_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm"
    "${Vulkan_INCLUDE_DIRS}"
)

set(EXTERNAL_LIBS
    glfw
    Vulkan::Vulkan
)

add_subdirectory(lib)
add_subdirectory(src)

# GLSLC command to compile shaders to SPIR-V
find_program(GLSLC glslc)
set(shader_path ${CMAKE_HOME_DIRECTORY}/resources/shaders/)
file(GLOB shaders RELATIVE ${CMAKE_SOURCE_DIR} "${shader_path}*.vert" "${shader_path}*.frag")

foreach(shader ${shaders})
    set(input_glsl "${CMAKE_HOME_DIRECTORY}/${shader}")
    set(output_spv "${input_glsl}.spv")
    add_custom_command(
        OUTPUT "${output_spv}"
        COMMAND "${GLSLC}" "${input_glsl}" "-o" "${output_spv}"
        DEPENDS "${input_glsl}"
    )
    list(APPEND SPV_FILES "${output_spv}")
endforeach()

# Create a custom target to compile shaders on every build
add_custom_target(CompileShaders ALL DEPENDS ${SPV_FILES})

# Make sure the shaders are compiled before building the main target
add_dependencies(VulkanSandbox CompileShaders)

# Copy .spv files to the output directory during the build
file(COPY ${SPV_FILES} DESTINATION ${CMAKE_BINARY_DIR})
