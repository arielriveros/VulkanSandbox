#include <iostream>
#include "App.h"

App::App()
{

}

App::~App()
{
	
}

void App::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_Window = glfwCreateWindow(m_Width, m_Height, "Window", nullptr, nullptr);
	m_Renderer = std::make_unique<Renderer>(m_Window);
	m_Renderer->Initialize();
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, OnResizeCallback);
}

void App::Run()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		glfwPollEvents();
		m_Renderer->Update();
	}

	m_Renderer->WaitIdle();
}

void App::Exit()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
	m_Renderer->Terminate();
}

void App::OnResize(int width, int height)
{
	m_Renderer->Resize( static_cast<uint32_t>(width), static_cast<uint32_t>(height) );
}

void App::OnResizeCallback(GLFWwindow* window, int width, int height)
{
	App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->OnResize(width, height);
}
