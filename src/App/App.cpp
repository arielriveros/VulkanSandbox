#include <iostream>
#include <chrono>
#include "App.h"

#define BIND_CALLBACK(func) std::bind(&App::func, this)
#define BIND_CALLBACK_1(func) std::bind(&App::func, this, std::placeholders::_1)
#define BIND_CALLBACK_2(func) std::bind(&App::func, this, std::placeholders::_1, std::placeholders::_2)
#define BIND_CALLBACK_3(func) std::bind(&App::func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define BIND_CALLBACK_4(func) std::bind(&App::func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)

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
	m_Window.SetOnResizeCallback(BIND_CALLBACK_2(OnResize));
	
	m_Camera = Camera();
	m_Camera.Position.y = 3.0f;
	m_Camera.Position.z = 3.0f;
	m_Camera.Rotation.x = -45.0f;

	Model* cube = new Model("cube", MeshData::Cube(), {{{0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 32.0f}}});
	m_Models.push_back(cube);
	Model* sphere = new Model("sphere", MeshData::Sphere(), {{{1.0f, 0.0f, 1.0f, 1.0f}}, "resources/assets/images/grass.jpg"});
	sphere->Position.x = -1.0f;
	sphere->Position.z = -1.0f;
	m_Models.push_back(sphere);
	Model* pyramid = new Model("pyramid", MeshData::Pyramid(), {{{0.0f, 1.0f, 0.0f, 1.0f}}, "resources/assets/images/bricks.jpg"});
	pyramid->Position.x = 1.0f;
	pyramid->Position.z = -1.0f;
	m_Models.push_back(pyramid);
	Model* floor = new Model("floor", MeshData::Quad(), {{{1.0f, 1.0f, 1.0f, 1.0f}}, "resources/assets/images/bricks.jpg"});
	floor->Position.y = -1.0f;
	floor->Rotation.x = -90.0f;
	floor->Scale = { 10.0f, 10.0f, 10.0f };
	m_Models.push_back(floor);

	m_DirLight = DirectionalLight({ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f);

	m_Renderer = std::make_unique<Renderer>(m_Window, m_Camera, m_Models);
	m_Renderer->SetDirectionalLight(&m_DirLight);
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
	m_Window.OnMouseMove(BIND_CALLBACK_4(OnMouseMoveCallback));

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
		m_DirLight.Position.y += delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::End))
		m_DirLight.Position.y -= delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::Delete))
		m_DirLight.Position.x -= delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::PageDown))
		m_DirLight.Position.x += delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::Insert))
		m_DirLight.Position.z += delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::PageUp))
		m_DirLight.Position.z -= delta;

	if (m_Window.IsKeyPressed(Keyboard::Key::Escape))
		m_Window.Close();
}

void App::OnMouseMoveCallback(float xPos, float yPos, float xOffset, float yOffset)
{
	if (m_Window.IsMouseButtonPressed(Mouse::Button::Right))
	{
		m_Camera.Rotation.y += xOffset * 0.25f;
		m_Camera.Rotation.x += yOffset * 0.25f;
		m_Camera.UpdateRotation();
	}
}
