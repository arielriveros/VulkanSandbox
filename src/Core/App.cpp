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

	m_Scene = SceneGraph();
	m_Scene.Initialize();

	Model cube = Model(MeshData::Cube(), {{{0.0f, 0.0f, 1.0f, 1.0f}}, "images/bricks.jpg"});
	Model sphere = Model(MeshData::Sphere(96), {{{1.0f, 1.0f, 1.0f, 1.0f}}, "images/world.png"});
	Model pyramid = Model(MeshData::Pyramid(), {{{0.0f, 1.0f, 0.0f, 1.0f}}, "images/bricks.jpg"});
	Model floor = Model(MeshData::Quad(), {{{1.0f, 1.0f, 1.0f, 1.0f}}, "images/bricks.jpg"});

	Node* inner = new Node("Inner", sphere);
	Node* cubeNode = new Node("cube", cube);
	inner->AddNode(cubeNode);
	inner->GetTransform().Rotation.y = 45.0f;
	(*inner)["cube"]->GetTransform().Position.x = 2.0f;
	(*inner)["cube"]->GetTransform().Position.y = 2.0f;
	m_Scene.AddNode(inner);

	Node* a = new Node("a");
	Node* b = new Node("b");
	Node* c = new Node("c");

	b->AddNode(c);
	a->AddNode(b);
	m_Scene.AddNode(a);

	Node* sphereNode = new Node("sphere", sphere);
	m_Scene.AddNode(sphereNode);
	m_Scene["sphere"].GetTransform().Position.x = -2.0f;

	Node* pyramidNode = new Node("pyramid", pyramid);
	m_Scene.AddNode(pyramidNode);
	m_Scene["pyramid"].GetTransform().Position.z = -2.0f;

	Node* floorNode = new Node("floor", floor);
	m_Scene.AddNode(floorNode);
	m_Scene["floor"].GetTransform().Position.y = -1.0f;
	m_Scene["floor"].GetTransform().Scale = glm::vec3(10.0f);
	m_Scene["floor"].GetTransform().Rotation.x = -90.0f;

	Node* sun = new Node("sun", DirectionalLight({ 1.0f, 1.0f, 1.0f }, 1.0f));
	m_Scene.AddNode(sun);
	m_Scene["sun"].GetTransform().Rotation.x = 45.0f;
	m_Scene["sun"].GetTransform().Rotation.y = 45.0f;

	m_Renderer = std::make_unique<Renderer>(m_Window, m_Camera, m_Scene);
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
