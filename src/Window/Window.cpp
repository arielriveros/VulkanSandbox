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
