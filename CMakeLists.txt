cmake_minimum_required(VERSION 3.16)
project(VulkanSandbox CXX)

set(CMAKE_CXX_STANDARD 17)

# Vulkan SDK required
find_package(Vulkan REQUIRED)

# Third party includes
set(EXTERNAL_LIBS_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm"
    "${Vulkan_INCLUDE_DIRS}"
)

# Third party libraries
set(EXTERNAL_LIBS
    glfw
    Vulkan::Vulkan
)

add_subdirectory(lib)
add_subdirectory(src)

# Resources
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources
    ${PROJECT_BINARY_DIR}/resources
)

add_dependencies(VulkanSandbox CopyResources)

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
  
# Make sure the shaders are compiled before copying the resources
add_dependencies(CopyResources CompileShaders)