#pragma once

#include <conio.h>
#include <gl_core_4_4.h>
#include <glfw-3.1.2\include\GLFW\glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <list>

#include "Application.h"
#include "Camera.h"

class PhysicsObject2D;

class PhysicsApplication : public Application
{
public:
	static PhysicsApplication* app;
	PhysicsApplication() { PhysicsApplication* app = this; }

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void restart();
	void poolTable();

	int day = 0;

	Camera camera;

	GLuint shaderID;

	

	std::list<PhysicsObject2D*> physicsObjects;

	vec2 contactPoint;
	vec2 mousePoint;
	bool mouseDown;
};
