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
	m_Window.SetOnResizeCallback(std::bind(&App::OnResize, this, std::placeholders::_1, std::placeholders::_2));
	m_Camera = Camera();
	m_Camera.Position.y = 3.0f;
	m_Camera.Position.z = -3.0f;
	m_Camera.Rotation.x = -45.0f;

	m_Renderer = std::make_unique<Renderer>(m_Window, m_Camera);
	m_Renderer->Initialize();
}

void App::Run()
{
	while (!m_Window.ShouldClose())
	{
		m_Window.UpdateFPSCounter();
		m_Window.PollEvents();
		HandleInput();
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

void App::HandleInput()
{
	if (m_Window.IsMouseButtonPressed(Mouse::Button::Left))
		m_Window.OnMouseMove(std::bind(&App::OnMouseMoveCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	else
		m_Window.ResetOffset();

	float delta = 0.001f;

	if (m_Window.IsKeyPressed(Keyboard::Key::LeftShift))
		delta = 0.005f;
	else
		delta = 0.001f;


	if (m_Window.IsKeyPressed(Keyboard::Key::W))
		m_Camera.MoveForward(delta);

	if (m_Window.IsKeyPressed(Keyboard::Key::S))
		m_Camera.MoveForward(-delta);

	if (m_Window.IsKeyPressed(Keyboard::Key::A))
		m_Camera.MoveRight(-delta);

	if (m_Window.IsKeyPressed(Keyboard::Key::D))
		m_Camera.MoveRight(delta);

	if (m_Window.IsKeyPressed(Keyboard::Key::E))
		m_Camera.Position.y += delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::Q))
		m_Camera.Position.y -= delta;
}

void App::OnMouseMoveCallback(float xPos, float yPos, float xOffset, float yOffset)
{
	m_Camera.Rotation.y += xOffset;
	m_Camera.Rotation.x += yOffset;
	m_Camera.UpdateRotation();
}
