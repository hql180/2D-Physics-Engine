#include "stdafx.h"
#include "Circle.h"
#include "Gizmos.h"
#include "Plane.h"

Circle::Circle()
{
	objectType = CIRCLE;
}

Circle::Circle(vec2 pos, vec2 vel, float r, float m, float bouncy, float a, float rot, vec4 col)
{
	position = pos;
	velocity = vel;
	mass = m;
	radius = r;
	bounce = bouncy;
	angle = a;
	colour = col;
	rotation = rot;
	
	moment = 0.5f * mass * radius * radius;

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

void Circle::collideWithCircle(Circle * circle)
{

}

void Circle::collideWithPlane(Plane * plane)
{
	float distance = glm::dot(position - plane->position, plane->normal);
	float vPerp = glm::dot(velocity, plane->normal);

	if ((distance > 0 && distance < radius && vPerp < 0) || (distance < 0 && distance> -radius&& vPerp > 0))
	{
		vec2 force = -mass * (vPerp * plane->normal) * (1.f + bounce);
		applyForce(force, plane->normal * distance);
	}
}

void Circle::collideWithBox(Box * box)
{
}

