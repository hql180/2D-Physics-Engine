#pragma once
#include "RigidBody2D.h"


class Circle :
	public RigidBody2D
{
public:
	Circle();
	Circle(vec2 pos = vec2(0), vec2 vel = vec2(0), 
			float r = 1.f, float m = 1.f, float bouncy = 0.5f, 
			float a = 0, float rot = 0, vec4 col = vec4(1, 0, 0, 1));
	~Circle();

	virtual void draw();

	virtual void collideWithCircle(Circle* circle);
	virtual void collideWithPlane(Plane* plane);
	virtual void collideWithBox(Box* box);

	virtual bool isInside(vec2 pt);
		
	float radius;
};

