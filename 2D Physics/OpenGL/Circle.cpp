#include "stdafx.h"
#include "Circle.h"
#include "Gizmos.h"
#include "Plane.h"

Circle::Circle(vec2 pos, vec2 vel, float r, float m, float bouncy, float a, vec4 col)
{
	position = pos;
	velocity = vel;
	mass = m;
	radius = r;
	bounce = bouncy;
	angle = a;
	colour = col;
	objectType = CIRCLE;
}

Circle::~Circle()
{
}

void Circle::draw()
{
	Gizmos::add2DCircle(position, radius, 20, colour);
}

void Circle::update(float dt)
{
	position += velocity * dt;
	velocity += gravity * dt;
}

void Circle::collideWithCircle(Circle * other)
{

}

void Circle::collideWithPlane(Plane * other)
{
	float distance = glm::dot(position - other->position, other->normal);
	float vPerp = glm::dot(velocity, other->normal);

	if ((distance > 0 && distance < radius && vPerp < 0) || (distance < 0 && distance> -radius&& vPerp > 0))
	{
		vec2 force = -mass * (vPerp * other->normal) * (1.f + radius);
		applyForce(force);
	}
}

