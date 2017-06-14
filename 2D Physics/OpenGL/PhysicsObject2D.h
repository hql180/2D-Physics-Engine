#pragma once
#include "glm.hpp"

using namespace glm;

class Circle;
class Plane;
class Box;
class Spring;

enum PhysicsObjectType
{
	NONE,
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
	virtual void collideWithCircle(Circle* circle) = 0;
	virtual void collideWithPlane(Plane* plane) = 0;
	virtual void collideWithBox(Box* box) = 0;
	
	virtual bool isInside(vec2 pt);

	void checkCollisions(PhysicsObject2D* other);

	static vec2 gravity;

	PhysicsObjectType objectType;
};

