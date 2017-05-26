#pragma once
#include "RigidBody2D.h"


class Circle :
	public RigidBody2D
{
public:
	Circle(vec2 pos = vec2(0), vec2 vel = vec2(0), float r = 1.f, float m = 1.f, float bouncy = 1.f, float a = 0, vec4 col = vec4(1, 0, 0, 1));
	~Circle();

	virtual void draw();
	virtual void update(float dt);

	virtual void collideWithCircle(Circle* other);
	virtual void collideWithPlane(Plane* other);

	float angle;
	float radius;
	vec4 colour;
};

