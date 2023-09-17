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

VkSurfaceKHR Window::CreateSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, m_GLFWwindow, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");

	return surface;
}
