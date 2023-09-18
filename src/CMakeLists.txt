add_executable(${PROJECT_NAME} 
	"entry.cpp"
	"App/App.h"
	"App/App.cpp"
	"Window/Window.h"
	"Window/Window.cpp"
	"Modules/ModuleInterface.h"
	"Modules/Renderer/Renderer.h"
	"Modules/Renderer/Renderer.cpp"
	"Modules/Renderer/Camera.h"
	"Modules/Renderer/Camera.cpp"
	"Modules/Renderer/Mesh.h"
	"Modules/Renderer/Mesh.cpp"
	"Modules/Renderer/Vulkan/Buffer.h"
	"Modules/Renderer/Vulkan/Buffer.cpp"
	"Modules/Renderer/Vulkan/Device.h"
	"Modules/Renderer/Vulkan/Device.cpp"
	"Modules/Renderer/Vulkan/Pipeline.h"
	"Modules/Renderer/Vulkan/Pipeline.cpp"
	"Modules/Renderer/Vulkan/ValidationLayer.h"
	"Modules/Renderer/Vulkan/ValidationLayer.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS})