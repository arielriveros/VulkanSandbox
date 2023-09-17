#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
	Window(std::string name, uint32_t width, uint32_t height);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void Initialize();
	void Terminate();
	bool ShouldClose();
	void PollEvents();

	VkSurfaceKHR CreateSurface(VkInstance instance);

	GLFWwindow* GetWindow() { return m_GLFWwindow; }

	std::string Name;
	uint32_t Width;
	uint32_t Height;

private:
	GLFWwindow* m_GLFWwindow;
};