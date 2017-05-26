#include "stdafx.h"
#include "Plane.h"
#include "Gizmos.h"

#include "Circle.h"

Plane::Plane()
{
	objectType = PLANE;
}

Plane::Plane(vec2 pos, vec2 norm, vec4 col) : position(pos), normal(norm), colour(col)
{
	objectType = PLANE;
	parallel = vec2(normal.y, -normal.x);
}


Plane::~Plane()
{
}

void Plane::draw()
{
	Gizmos::add2DLine(position - 100.f * parallel, position + 100.f * parallel, colour);
}

void Plane::update(float dt)
{
}

void Plane::collideWithCircle(Circle* other)
{
	other->collideWithPlane(this);
}

void Plane::collideWithPlane(Plane* other)
{
}
