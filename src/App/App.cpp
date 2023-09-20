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
	m_Camera.Position.z = 3.0f;
	m_Camera.Rotation.x = -45.0f;

	Model* cube1 = new Model("cube1", MeshData::Cube());
	m_Models.push_back(cube1);
	Model* cube2 = new Model("cube2", MeshData::Cube());
	cube2->Position.x = -1.0f;
	cube2->Position.z = -1.0f;
	m_Models.push_back(cube2);

	m_Renderer = std::make_unique<Renderer>(m_Window, m_Camera, m_Models);
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
	for (auto& model : m_Models)
		delete model;
	m_Renderer->Terminate();
	m_Window.Terminate();
}

void App::OnResize(int width, int height)
{
	m_Renderer->Resize( static_cast<uint32_t>(width), static_cast<uint32_t>(height) );
}

void App::HandleInput()
{
	m_Window.OnMouseMove(std::bind(&App::OnMouseMoveCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	if (m_Window.IsMouseButtonReleased(Mouse::Button::Right) && m_Window.IsMouseButtonReleased(Mouse::Button::Left))
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

	if (m_Window.IsKeyPressed(Keyboard::Key::Home))
		m_Models[0]->Position.z -= delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::End))
		m_Models[0]->Position.z += delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::Delete))
		m_Models[0]->Position.x -= delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::PageDown))
		m_Models[0]->Position.x += delta;
}

void App::OnMouseMoveCallback(float xPos, float yPos, float xOffset, float yOffset)
{
	if (m_Window.IsMouseButtonPressed(Mouse::Button::Right))
	{
		m_Camera.Rotation.y += xOffset;
		m_Camera.Rotation.x += yOffset;
		m_Camera.UpdateRotation();
	}

	if (m_Window.IsMouseButtonPressed(Mouse::Button::Left))
	{
		m_Models[0]->Rotation.y += xOffset;
		m_Models[0]->Rotation.x -= yOffset;
	}
}
