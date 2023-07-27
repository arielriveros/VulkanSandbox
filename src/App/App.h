#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "../Modules/Renderer/Renderer.h"

class App
{
public:
	App();
	~App();
	void Init();
	void Run();
	void Exit();

private:
	std::unique_ptr<Renderer> m_Renderer;
	GLFWwindow* m_Window = nullptr;
	uint32_t m_Width = 800;
	uint32_t m_Height = 600;
};