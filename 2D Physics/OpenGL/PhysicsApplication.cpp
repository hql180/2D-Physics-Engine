#include "PhysicsApplication.h"

#include <iterator>

#include "PhysicsObject2D.h"
#include "Plane.h"
#include "RigidBody2D.h"
#include "Circle.h"
#include "Box.h"
#include "Spring.h"

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
	Gizmos::clear();

	if (glfwGetKey(window, GLFW_KEY_R))
	{
		restart();
	}

	bool isDown = glfwGetMouseButton(window, 0);

	double x0, y0;
	glfwGetCursorPos(window, &x0, &y0);
	mat4 view = camera.getView();
	mat4 projection = camera.getProjection();

	vec3 windowCoord = vec3(x0, y0, 0);
	vec4 viewPort = vec4(0, 0, 1280, 720);
	vec3 worldCoord = unProject(windowCoord, view, projection, viewPort);

	mousePoint = vec2(worldCoord[0] * camera.getDistance(), worldCoord[1] * (-camera.getDistance()));

	if (isDown != mouseDown)
	{
		if (isDown)
		{
			contactPoint = mousePoint;
		}
		else
		{
			for (auto it = physicsObjects.begin(); it != physicsObjects.end(); ++it)
			{
				PhysicsObject2D* obj = *it;
				if (obj->isInside(mousePoint))
				{
					RigidBody2D* rb = (RigidBody2D*)obj;
					rb->applyForce(2.f*(mousePoint - contactPoint), contactPoint - rb->position);
				}
			}
		}
		mouseDown = isDown;
	}



	float dt = 1.0f / 60.0f;

	for (auto it = physicsObjects.begin(); it != physicsObjects.end(); ++it)
	{
		PhysicsObject2D* obj = *it;
		obj->update(dt);
		for (auto it2 = --physicsObjects.end(); it2 != it; --it2)
		{
			obj->checkCollisions(*it2);
		}
	}

	camera.update(window);




	
	return (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
}

void PhysicsApplication::draw()
{
	mat4 view = camera.getView();
	mat4 projection = camera.getProjection();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	for (auto& object : physicsObjects)
	{
		object->draw();
	}

	Gizmos::addTransform(glm::mat4(1));
	vec4 orange(1, 0.7f, 0.2f, 1.0f);
	vec4 red(1, 0, 0, 1);
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	//for (int i = 0; i < 21; ++i) {
	//	Gizmos::add2DLine(vec2(-10 + i, 10), vec2(-10 + i, -10), i == 10 ? orange : white);
	//	Gizmos::add2DLine(vec2(10, -10 + i), vec2(-10, -10 + i), i == 10 ? orange : white);
	//}

	// test rendering code
	//Gizmos::add2DCircle(vec2(0, -4), 1, 32, red);
	//Gizmos::add2DAABBFilled(vec2(0, 1), vec2(1, 3), red);

	if (mouseDown)
		Gizmos::add2DLine(contactPoint, mousePoint, white);
	
	Gizmos::draw2D(projection * view);
		
	glfwSwapBuffers(window);
	glfwPollEvents();

	day++;
}

void PhysicsApplication::restart()
{
	while (physicsObjects.size() > 0)
	{
		delete physicsObjects.front();
		physicsObjects.remove(physicsObjects.front());
	}

	float bounce = .2f;
	float boxBounce = .5f;


	physicsObjects.push_back(new Spring());
	physicsObjects.push_back(new Plane(vec2(0, -5), vec2(0.5, 1)));
	physicsObjects.push_back(new Plane(vec2(25, 0), vec2(1, 0)));
	physicsObjects.push_back(new Plane(vec2(0, 20), vec2(0, 1)));
	physicsObjects.push_back(new Circle(vec2(0, 10), vec2(0, 1), 1, 1, bounce));
	((Spring*)physicsObjects.front())->body1 = ((RigidBody2D*)physicsObjects.back());
	physicsObjects.push_back(new Circle(vec2(2, 10), vec2(1, -1), 1, 1, bounce));
	((RigidBody2D*)physicsObjects.back())->isFixed = true;
	

	physicsObjects.push_back(new Circle(vec2(-2, 10), vec2(-1, -1), 1, 1, bounce));
	physicsObjects.push_back(new Box(vec2(-8, 5), vec2(0), 2, 2, 2, boxBounce));
	((Spring*)physicsObjects.front())->body2 = ((RigidBody2D*)physicsObjects.back());
	physicsObjects.push_back(new Box(vec2(-4, 0), vec2(0), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-8, 10), vec2(0), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-6, 15), vec2(2, 3), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-12, 15), vec2(2, 3), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-12, 17), vec2(2, 3), 2, 2, 2, boxBounce));
	((RigidBody2D*)physicsObjects.back())->isAwake = true;
	physicsObjects.push_back(new Box(vec2(-12, 4), vec2(2, 3), 2, 2, 2, boxBounce));
	physicsObjects.push_back(new Box(vec2(-12, 6), vec2(2, 3), 2, 2, 2, boxBounce));


	((RigidBody2D*)physicsObjects.back())->isFixed = true;

	((Spring*)physicsObjects.front())->restoringForce = 0.1f;
	
}

void PhysicsApplication::poolTable()
{
	while (physicsObjects.size() > 0)
	{
		delete physicsObjects.front();
		physicsObjects.remove(physicsObjects.front());
	}

	PhysicsObject2D::gravity = vec2(0);


}
