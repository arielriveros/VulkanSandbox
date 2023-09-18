#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "../Window/Window.h"
#include "../Modules/Renderer/Camera.h"
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
	void HandleInput();
	void OnMouseMoveCallback(float xPos, float yPos, float xOffset, float yOffset);

	std::unique_ptr<Renderer> m_Renderer;
	Camera m_Camera;
	Window m_Window{ "App", 800, 600 };
};