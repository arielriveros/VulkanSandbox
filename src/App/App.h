#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "../Window/Window.h"
#include "../Modules/Renderer/Camera.h"
#include "../Modules/Renderer/Model.h"
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
	std::vector<Model*> m_Models;
	Window m_Window{ "App", 800, 600 };
};