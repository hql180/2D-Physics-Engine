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

	//restart();

	PhysicsObject2D::gravity = vec2(0, -1);

	poolTable();



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
		//restart();
		poolTable();
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
					rb->isAwake = true;
					rb->applyForce(2.f*(mousePoint - contactPoint), contactPoint - rb->position);
				}
			}
		}
		mouseDown = isDown;
	}



	float dt = 1.0f / 120.0f;

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

	float bounce = 1.f;
	float boxBounce = 1.f;


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

	((Spring*)physicsObjects.front())->restoringForce = .5f;

	camera.radius = 10;
	
}

void PhysicsApplication::poolTable()
{
	while (physicsObjects.size() > 0)
	{
		delete physicsObjects.front();
		physicsObjects.remove(physicsObjects.front());
	}

	PhysicsObject2D::gravity = vec2(0, 0);

	physicsObjects.push_back(new Box(vec2(-37, 0), vec2(0), 3, 40, 100, 1, 0, 0));
	((RigidBody2D*)physicsObjects.back())->isFixed = true;
	physicsObjects.push_back(new Box(vec2(37, 0), vec2(0), 3, 40, 100, 1, 3.14159f, 0));
	((RigidBody2D*)physicsObjects.back())->isFixed = true;
	physicsObjects.push_back(new Box(vec2(0, 20), vec2(0), 80, 3, 100, 1, 0, 0));
	((RigidBody2D*)physicsObjects.back())->isFixed = true;
	physicsObjects.push_back(new Box(vec2(0, -20), vec2(0), 80, 3, 100, 1, 0, 0));
	((RigidBody2D*)physicsObjects.back())->isFixed = true;



	Spring* spring = new Spring();

	Box* box1 = new Box();
	Box* box2 = new Box();
	box1->position = vec2(0);
	box2->position = vec2(0, 4);

	physicsObjects.push_back(box1);
	physicsObjects.push_back(box2);
	physicsObjects.push_back(spring);

	spring->body1 = box1;
	spring->body2 = box2;
	spring->restLength = 10;
	spring->restoringForce = 0.1;


	Box* box3 = new Box(vec2(0, -10));
	Circle* circle1 = new Circle(vec2(-15, -5));
	Circle* circle2 = new Circle(vec2(15, -5));
	Spring* spring1 = new Spring(circle1, box3);
	Spring* spring2 = new Spring(circle2, box3);
	physicsObjects.push_back(box3);
	physicsObjects.push_back(circle1);
	physicsObjects.push_back(circle2);
	physicsObjects.push_back(spring1);
	physicsObjects.push_back(spring2);

	spring1->restoringForce = 0.05f;
	spring2->restoringForce = 0.05f;




	circle1 = new Circle(vec2(-30, 15));
	circle2 = new Circle(vec2(-25, 15));

	physicsObjects.push_back(new Circle(vec2(-19, 15)));
	((RigidBody2D*)physicsObjects.back())->bounce = 1;

	spring1 = new Spring(circle1, circle2);
	spring1->restLength = 5;
	spring1->restoringForce = 0.5f;
	circle1->velocity = vec2(0, -40);
	circle1->isAwake = true;
	circle2->isFixed = true;

	physicsObjects.push_back(circle1);
	physicsObjects.push_back(circle2);
	physicsObjects.push_back(spring1);

	physicsObjects.push_back(new Circle(vec2(-3, 0)));
	((RigidBody2D*)physicsObjects.back())->bounce = 1;
	physicsObjects.push_back(new Box(vec2(3, 0)));
	((RigidBody2D*)physicsObjects.back())->bounce = 1;
	
	box2->position = vec2(0, 4);
	box2->isFixed = true;


	box3 = new Box(vec2(25, 15));
	circle1 = new Circle(vec2(30, 15));
	circle1->isFixed = true;
	circle2 = new Circle(vec2(20, 15));
	spring1 = new Spring(circle1, box3);
	spring2 = new Spring(box3, circle2);
	physicsObjects.push_back(box3);
	physicsObjects.push_back(circle1);
	physicsObjects.push_back(circle2);
	physicsObjects.push_back(spring1);
	physicsObjects.push_back(spring2);

	spring1->restoringForce = 0.05f;
	spring2->restoringForce = 0.05f;





	camera.radius = 3;
}
