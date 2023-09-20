#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "InputKeys.h"

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

	void OnMouseMove(std::function<void(float xPos, float yPos, float xOffset, float yOffset)> callback);
	bool IsMouseButtonPressed(Mouse::Button button);
	bool IsMouseButtonReleased(Mouse::Button button);
	bool IsKeyPressed(Keyboard::Key key);

	VkSurfaceKHR CreateSurface(VkInstance instance);

	GLFWwindow* GetWindow() { return m_GLFWwindow; }
	void SetFPSCounterEnabled(bool enabled) { m_FPSCounterEnabled = enabled; }
	void SetOnResizeCallback(std::function<void(int width, int height)> callback) { m_OnResizeCallback = callback; }
	void SetCursorEnabled(bool enabled) { glfwSetInputMode(m_GLFWwindow, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED); }
	void ResetOffset() { m_FirstMouse = true; }

	std::string Name;
	uint32_t Width;
	uint32_t Height;

private:
	static void OnResize(GLFWwindow* window, int width, int height);

	GLFWwindow* m_GLFWwindow;
	std::function<void(int width, int height)> m_OnResizeCallback;
	bool m_FPSCounterEnabled = false;
	float m_LastXPos = 0.0f;
	float m_LastYPos = 0.0f;
	bool m_FirstMouse = true;
};