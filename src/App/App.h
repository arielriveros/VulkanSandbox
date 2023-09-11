#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "../Window/Window.h"
#include "../Modules/Renderer/Renderer.h"

class App
{
public:
	App();
	~App();
	void Init();
	void Run();
	void Exit();
	void OnResize(int width, int height);

private:
	static void OnResizeCallback(GLFWwindow* window, int width, int height);

private:
	std::unique_ptr<Renderer> m_Renderer;
	Window m_Window{ "App", 800, 600 };
};