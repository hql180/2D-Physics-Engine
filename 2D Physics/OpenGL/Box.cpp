#include "stdafx.h"
#include "Box.h"

#include "Circle.h"
#include "Plane.h"

#include "Gizmos.h"

Box::Box()
{
	objectType = BOX;
}

Box::Box(vec2 pos, vec2 vel, float w, float h, float m, float bouncy, float a, float rot, vec4 col)
{
	position = pos;
	velocity = vel;
	mass = m;
	bounce = bouncy;
	angle = a;
	rotation = rot;
	colour = col;

	width = w;
	height = h;

	moment = 1.0f / 12.0f * mass * width * height;

	localX = vec2(0);
	localY = vec2(0);

	objectType = BOX;
}


Box::~Box()
{
}

void Box::draw()
{
	vec2 p1 = position - localX * width / 2.0f - localY * height / 2.0f;
	vec2 p2 = position + localX * width / 2.0f - localY * height / 2.0f;
	vec2 p3 = position - localX * width / 2.0f + localY * height / 2.0f;
	vec2 p4 = position + localX * width / 2.0f + localY * height / 2.0f;

	Gizmos::add2DCircle(p1, 0.2f, 10, vec4(1, 0, 0, 1));
	Gizmos::add2DCircle(p2, 0.2f, 10, vec4(1, 1, 0, 1));
	Gizmos::add2DCircle(p3, 0.2f, 10, vec4(0, 1, 1, 1));
	Gizmos::add2DCircle(p4, 0.2f, 10, vec4(0, 1, 0, 1));
	Gizmos::add2DTri(p1, p2, p4, colour);
	Gizmos::add2DTri(p1, p4, p3, colour);
}

void Box::update(float dt)
{
	angle += rotation * dt;
	position += velocity * dt;

	float cs = cosf(angle);
	float sn = sinf(angle);

	localX = vec2(cs, sn);
	localY = vec2(-sn, cs);

	velocity += gravity * dt;
}

void Box::collideWithCircle(Circle * circle)
{
}

void Box::collideWithPlane(Plane * plane)
{
	int contacts = 0;
	vec2 contact(0, 0);
	float contactV = 0;
	float radius = 0.5f * std::fminf(width, height);

	float comFromPlane = dot(position - plane->position, plane->normal);
	float penetration = 0;

	for (float x = -width / 2; x < width; x += width)
	{
		for (float y = -height / 2; y < height; y += height)
		{
			vec2 p = position + x*localX + y*localY;
			float distFromPlane = dot(p - plane->position, plane->normal);

			float velocityIntoPlane = dot(velocity + rotation * (-y*localX + x*localY), plane->normal);

			if ((distFromPlane > 0 && comFromPlane < 0 && velocityIntoPlane > 0) ||
				(distFromPlane < 0 && comFromPlane > 0 && velocityIntoPlane < 0))
			{
				contacts++;
				contact += p;
				contactV += velocityIntoPlane;
				if (comFromPlane >= 0)
				{
					if (penetration > distFromPlane)
						penetration = distFromPlane;
				}
				else 
				{
					if (penetration < distFromPlane)
						penetration = distFromPlane;
				}

			}
		}
	}

	if (contacts > 0)
	{
		float collisionV = contactV / (float)contacts;

		vec2 acceleration = -plane->normal * (1.0f + bounce) * collisionV;

		vec2 localContact = (contact / (float)contacts) - position;

		float r = dot(localContact, vec2(plane->normal.y, -plane->normal.x));

		float mass0 = 1.0f / (1.0f / mass + (r*r) / moment);

		applyForce(acceleration * mass0, localContact);
		position -= plane->normal * penetration;
	}
}

void Box::collideWithBox(Box * box)
{
}
