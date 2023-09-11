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
	m_Window.Initialize();
	m_Renderer = std::make_unique<Renderer>(&m_Window);
	m_Renderer->Initialize();
	glfwSetWindowUserPointer(m_Window.GetWindow(), this);
	glfwSetFramebufferSizeCallback(m_Window.GetWindow(), OnResizeCallback);
}

void App::Run()
{
	while (!m_Window.ShouldClose())
	{
		m_Window.PollEvents();
		m_Renderer->Update();
	}

	m_Renderer->WaitIdle();
}

void App::Exit()
{
	m_Renderer->Terminate();
	m_Window.Terminate();
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
