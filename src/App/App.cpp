#include <iostream>
#include <chrono>
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
	m_Window.SetFPSCounterEnabled(true);
	m_Camera = Camera();
	m_Camera.Position.y = 3.0f;
	m_Camera.Position.z = -3.0f;
	m_Camera.Rotation.x = -45.0f;

	m_Renderer = std::make_unique<Renderer>(m_Window, m_Camera);
	m_Renderer->Initialize();
	glfwSetWindowUserPointer(m_Window.GetWindow(), this);
	glfwSetFramebufferSizeCallback(m_Window.GetWindow(), OnResizeCallback);
}

void App::Run()
{
	while (!m_Window.ShouldClose())
	{
		m_Window.UpdateFPSCounter();
		m_Window.PollEvents();

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		m_Camera.Position.x = sin(time) * 2.0f;

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
