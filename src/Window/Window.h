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
	void UpdateFPSCounter(float updateFrequency = 0.5f);

	VkSurfaceKHR CreateSurface(VkInstance instance);

	GLFWwindow* GetWindow() { return m_GLFWwindow; }
	void SetFPSCounterEnabled(bool enabled) { m_FPSCounterEnabled = enabled; }

	std::string Name;
	uint32_t Width;
	uint32_t Height;

private:
	GLFWwindow* m_GLFWwindow;
	bool m_FPSCounterEnabled = false;
};