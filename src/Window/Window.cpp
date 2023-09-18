#include <stdexcept>
#include "Window.h"

Window::Window(std::string name, uint32_t width, uint32_t height)
	: Name{name}, Width{width}, Height{height}
{
}

Window::~Window()
{
}

void Window::Initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_GLFWwindow = glfwCreateWindow(Width, Height, Name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_GLFWwindow, this);
	glfwSetFramebufferSizeCallback(m_GLFWwindow, OnResize);
}

void Window::Terminate()
{
	glfwDestroyWindow(m_GLFWwindow);
	glfwTerminate();
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_GLFWwindow);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::UpdateFPSCounter(float updateFrequency)
{
	if (!m_FPSCounterEnabled)
        return;

	static double lastTime = glfwGetTime();
	static int frameCount = 0;

	double currentTime = glfwGetTime();
	float delta = static_cast<float>(currentTime - lastTime);
	frameCount++;

	if (delta < updateFrequency)
		return;

	float fps = frameCount / delta;

	std::string title = Name + " - FPS: " + std::to_string(fps);
	glfwSetWindowTitle(m_GLFWwindow, title.c_str());
	frameCount = 0;
	lastTime = currentTime;
}

void Window::OnMouseMove(std::function<void(float xPos, float yPos, float xOffset, float yOffset)> callback)
{
	double mouseX, mouseY;
	glfwGetCursorPos(m_GLFWwindow, &mouseX, &mouseY);

	float xPos = static_cast<float>(mouseX);
	float yPos = static_cast<float>(mouseY);

	if (m_FirstMouse)
	{
		m_LastXPos = xPos;
		m_LastYPos = yPos;
		m_FirstMouse = false;
	}

	float xOffset = xPos - m_LastXPos;
	float yOffset = m_LastYPos - yPos;

	callback(xPos, yPos, xOffset, yOffset);

	m_LastXPos = xPos;
	m_LastYPos = yPos;
}

VkSurfaceKHR Window::CreateSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, m_GLFWwindow, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");

	return surface;
}

bool Window::IsMouseButtonPressed(Mouse::Button button)
{
	return glfwGetMouseButton(m_GLFWwindow, button) == GLFW_PRESS;
}

bool Window::IsKeyPressed(Keyboard::Key key)
{
	return glfwGetKey(m_GLFWwindow, key) == GLFW_PRESS;
}

bool Window::IsMouseButtonReleased(Mouse::Button button)
{
	return glfwGetMouseButton(m_GLFWwindow, button) == GLFW_RELEASE;
}

void Window::OnResize(GLFWwindow* window, int width, int height)
{
	Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	win->Width = width;
	win->Height = height;
	win->m_OnResizeCallback(width, height);
}