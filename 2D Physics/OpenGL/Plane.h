#pragma once
#include "PhysicsObject2D.h"
#include "glm.hpp"

using namespace glm;

class Plane :
	public PhysicsObject2D
{
public:
	Plane();
	Plane(vec2 pos, vec2 norm = vec2(0, 1), vec4 col = vec4(0.5f, 0.35f, 0.1f, 1));
	~Plane();

	virtual void draw();
	virtual void update(float dt);

	virtual void collideWithCircle(Circle* other);
	virtual void collideWithPlane(Plane* other);

	vec2 position;
	vec2 normal;
	vec2 parallel;
	vec4 colour;
};

