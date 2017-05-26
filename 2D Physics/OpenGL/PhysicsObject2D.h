#pragma once
#include "glm.hpp"

using namespace glm;

class Circle;
class Plane;
class Box;
class Spring;

enum PhysicsObjectType
{
	PLANE,
	CIRCLE,
	BOX,
	SPRING
};

class PhysicsObject2D
{
public:
	virtual void draw() = 0;
	virtual void update(float dt) = 0;	
	virtual void collideWithCircle(Circle* other) = 0;
	virtual void collideWithPlane(Plane* other) = 0;

	void checkCollisions(PhysicsObject2D* other);

	static vec2 gravity;

	PhysicsObjectType objectType;
};

