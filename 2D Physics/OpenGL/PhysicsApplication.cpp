#include "PhysicsApplication.h"

#include <iterator>

#include "PhysicsObject2D.h"
#include "Plane.h"
#include "RigidBody2D.h"
#include "Circle.h"
#include "Box.h"

using namespace glm;

vec2 PhysicsObject2D::gravity = vec2(0);

bool PhysicsApplication::startup()
{
	glfwInit();

	window = glfwCreateWindow(1280, 720,
		"Computer Graphics",
		nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);


	glClearColor(0.0f, 0.0f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST); // enables the depth buffer

	camera.radius = 2;

	Gizmos::create(65335U, 65535U, 65535U, 65535U);

	restart();

	PhysicsObject2D::gravity = vec2(0, -2);

	return true;
}

void PhysicsApplication::shutdown()
{
	Gizmos::destroy();

	glfwDestroyWindow(window);


	glfwTerminate();
}

bool PhysicsApplication::update()
{
	if (glfwGetKey(window, GLFW_KEY_R))
	{
		restart();
	}

	camera.update(window);

	float dt = 1.0f / 60.0f;
	
	int counter = 0;
	for (auto it = physicsObjects.begin(); it != physicsObjects.end(); ++it)
	{	
		PhysicsObject2D* obj = *it;
		obj->update(dt);
		for (auto it2 = --physicsObjects.end(); it2 != it; --it2)
		{
			obj->checkCollisions(*it2);
			counter += 1;
		}		
	}
	
	return (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
}

void PhysicsApplication::draw()
{
	mat4 view = camera.getView();
	mat4 projection = camera.getProjection();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::clear();

	for (auto& object : physicsObjects)
	{
		object->draw();
	}

	Gizmos::addTransform(glm::mat4(1));
	vec4 orange(1, 0.7f, 0.2f, 1.0f);
	vec4 red(1, 0, 0, 1);
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::add2DLine(vec2(-10 + i, 10), vec2(-10 + i, -10), i == 10 ? orange : white);
		Gizmos::add2DLine(vec2(10, -10 + i), vec2(-10, -10 + i), i == 10 ? orange : white);
	}

	// test rendering code
	//Gizmos::add2DCircle(vec2(0, -4), 1, 32, red);
	//Gizmos::add2DAABBFilled(vec2(0, 1), vec2(1, 3), red);



	Gizmos::draw2D(projection * view);


	
	glfwSwapBuffers(window);
	glfwPollEvents();

	day++;
}

// will cause memory leaks
void PhysicsApplication::restart()
{
	physicsObjects.clear();
	float bounce = .2f;
	float boxBounce = .2f;

	physicsObjects.push_back(new Circle(vec2(0, 10), vec2(0, 1), 1, 1, bounce));
	physicsObjects.push_back(new Circle(vec2(2, 10), vec2(1, -1), 1, 1, bounce));
	physicsObjects.push_back(new Plane(vec2(0, -5), vec2(0.5, 1)));
	physicsObjects.push_back(new Plane(vec2(25, 0), vec2(1, 0)));
	physicsObjects.push_back(new Plane(vec2(0, 20), vec2(0, 1)));
	physicsObjects.push_back(new Circle(vec2(-2, 10), vec2(-1, -1), 1, 1, bounce));
	physicsObjects.push_back(new Box(vec2(-8, 10), vec2(2, 3), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-6, 10), vec2(4, 7), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-8, 5), vec2(-3, 0), 2, 2, 2, boxBounce));
}
