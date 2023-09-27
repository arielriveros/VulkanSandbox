add_subdirectory(glfw)
add_subdirectory(glm)

add_library(imgui STATIC
    "imgui/imgui.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_vulkan.cpp"
    "imgui/misc/cpp/imgui_stdlib.cpp"
)

target_include_directories(imgui PUBLIC
    "imgui"
    "imgui/backends"
    "imgui/misc/cpp"
    "glfw/include"
    "${Vulkan_INCLUDE_DIRS}"
)

target_link_libraries(imgui PUBLIC
    glfw
    Vulkan::Vulkan
)