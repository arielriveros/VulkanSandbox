#include <iostream>
#include "App.h"

App::App()
{
	m_Renderer = std::make_unique<Renderer>();
}

App::~App()
{
	
}

void App::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_Window = glfwCreateWindow(m_Width, m_Height, "Window", nullptr, nullptr);
	m_Renderer->Initialize();
}

void App::Run()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		glfwPollEvents();
		m_Renderer->Update();
	}
}

void App::Exit()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
	m_Renderer->Terminate();
}
