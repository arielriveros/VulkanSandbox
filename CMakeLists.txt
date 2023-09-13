cmake_minimum_required(VERSION 3.16)
project(VulkanSandbox CXX)

set(CMAKE_CXX_STANDARD 17)

# Vulkan SDK required
find_package(Vulkan REQUIRED)

# Third party includes
set(EXTERNAL_LIBS_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb"
    "${Vulkan_INCLUDE_DIRS}"
)

# Third party libraries
set(EXTERNAL_LIBS
    glfw
    Vulkan::Vulkan
)

add_subdirectory(lib)
add_subdirectory(src)

set_target_properties(VulkanSandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin/Debug")
set_target_properties(VulkanSandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/Release")

###################### Shaders ######################
add_custom_target(CopyCompiledShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources/shaders/compiled
    ${PROJECT_BINARY_DIR}/bin/$<CONFIG>/resources/shaders
)

add_dependencies(VulkanSandbox CopyCompiledShaders)

# GLSLC command to compile shaders to SPIR-V
find_program(GLSLC glslc)
set(shader_path ${CMAKE_HOME_DIRECTORY}/resources/shaders/)
set(compiled_shader_path ${CMAKE_HOME_DIRECTORY}/resources/shaders/compiled/)
file(GLOB shaders RELATIVE ${CMAKE_SOURCE_DIR} "${shader_path}*.vert" "${shader_path}*.frag")

foreach(shader ${shaders})
    set(input_glsl "${CMAKE_HOME_DIRECTORY}/${shader}")
    get_filename_component(shader_name ${shader} NAME)
    set(output_spv "${compiled_shader_path}${shader_name}.spv")
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
add_dependencies(CopyCompiledShaders CompileShaders)

###################### End Shaders ######################

###################### Assets ######################

# Copy all files in the resources/assets folder to the build directory after clearing it
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${PROJECT_BINARY_DIR}/bin/$<CONFIG>/resources/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources/assets
    ${PROJECT_BINARY_DIR}/bin/$<CONFIG>/resources/assets
)

add_dependencies(VulkanSandbox CopyAssets)

###################### End Assets ######################