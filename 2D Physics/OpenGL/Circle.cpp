#include "stdafx.h"
#include "Circle.h"
#include "Gizmos.h"
#include "Plane.h"
#include "Box.h"

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
	
	localX = vec2(0);
	localY = vec2(0);

	moment = 0.5f * mass * radius * radius;

	isFixed = false;
	isAwake = false;

	objectType = CIRCLE;
}

Circle::~Circle()
{
}

void Circle::draw()
{
	Gizmos::add2DCircle(position, radius, 20, colour);
	Gizmos::add2DCircle(position + radius* 0.5f * localX, radius*0.2f, 20, vec4(1));
}

void Circle::collideWithCircle(Circle * circle)
{
	vec2 vecToOther = circle->position - position;
	float distance = length(vecToOther);
	if (distance < circle->radius + radius)
	{
		float penetration = (circle->radius + radius - distance) / 2.f;
		vec2 contactForce = normalize(vecToOther) * penetration;
		if (!isFixed)
			position -= contactForce;
		if (!circle->isFixed)
			circle->position += contactForce;
		// adding position to convert to world space
		resolveCollision(circle, position + normalize(vecToOther) * radius);
	}
}

void Circle::collideWithPlane(Plane * plane)
{
	float distance = dot(position - plane->position, plane->normal);
	float vPerp = dot(velocity, plane->normal);

	float penetration = 0;

	if ((distance > 0 && distance < radius && vPerp < 0) || 
		(distance < 0 && distance >-radius && vPerp > 0))
	{
		if (distance > 0 && distance < radius)
		{
			penetration = distance - radius;
		}
		else
		{
			penetration = distance + radius;
		}

		vec2 force = -mass * (vPerp * plane->normal) * (1.f + bounce);


		position -= plane->normal * penetration;
		applyForce(force, plane->parallel * dot(position - plane->position, vec2(0)) );
	}
}

void Circle::collideWithBox(Box * box)
{
	box->collideWithCircle(this);
}

bool Circle::isInside(vec2 pt)
{
	return (length(pt - position) < radius);
}

