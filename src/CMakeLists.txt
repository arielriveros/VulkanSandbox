add_executable(${PROJECT_NAME} 
	"entry.cpp"
	"App/App.h"
	"App/App.cpp"
	"Window/Window.h"
	"Window/Window.cpp"
	"Modules/ModuleInterface.h"
	"Modules/Renderer/Renderer.h"
	"Modules/Renderer/Renderer.cpp"
	"Modules/Renderer/Pipeline/Pipeline.h"
	"Modules/Renderer/Pipeline/Pipeline.cpp"
	"Modules/Renderer/ValidationLayer/ValidationLayer.h"
	"Modules/Renderer/ValidationLayer/ValidationLayer.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTERNAL_LIBS})