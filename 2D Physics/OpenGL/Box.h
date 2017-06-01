#pragma once
#include "RigidBody2D.h"
class Box :
	public RigidBody2D
{
public:
	Box();
	Box(vec2 pos = vec2(0), vec2 vel = vec2(0),
		float w = 2.f, float h = 2.f,
		float m = 1.f, float bouncy = 0.5f,
		float a = 0, float rot = 0, vec4 col = vec4(0, 0, 1, 1));
	~Box();

	virtual void draw();
	virtual void update(float dt);

	virtual void collideWithCircle(Circle* circle);
	virtual void collideWithPlane(Plane* plane);
	virtual void collideWithBox(Box* box);

	float width, height;
};

