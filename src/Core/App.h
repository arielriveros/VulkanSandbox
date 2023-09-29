#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "../Core/Window.h"
#include "../Modules/Scene/Camera.h"
#include "../Modules/Scene/Graph.h"
#include "../Modules/Scene/Model.h"
#include "../Modules/Renderer/Renderer.h"
#include "../Modules/Renderer/Lighting/DirectionalLight.h"

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
	SceneGraph m_Scene;
	DirectionalLight m_DirLight;
	Window m_Window{ "App", 800, 600 };
};