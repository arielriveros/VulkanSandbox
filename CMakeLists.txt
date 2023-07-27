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