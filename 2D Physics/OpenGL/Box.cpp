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
	RigidBody2D::update(dt);
}

void Box::collideWithCircle(Circle * circle)
{
	// relative position of circle
	vec2 circlePos = circle->position - position;

	float circleDist = length(circlePos);
	float boxRadius = length(width / 2.f + height / 2.f); // debug

	// excludes circles too far away to collide
	if (length(circlePos) > circle->radius + length(width / 2.f + height / 2.f))
		return;

	float w2 = width / 2.f;
	float h2 = height / 2.f;

	int numContacts = 0;
	// contact point is in box local space
	vec2 contact(0);
	vec2 direction = vec2(0);
	int otherContacts = 0;

	float penetration = 0;
	// checks to see if any of the four corners are inside circle
	for (float x = -w2; x < width; x += width)
	{
		for (float y = -h2; y < height; y += height)
		{
			vec2 p = x*localX + y*localY;
			vec2 dp = p - circlePos;
			if (dp.x*dp.x + dp.y*dp.y < circle->radius*circle->radius)
			{
				++numContacts;
				contact += vec2(x, y);
				penetration = circle->radius * circle->radius - (dp.x*dp.x + dp.y*dp.y);
			}
		}


		// get local position of circle centre
		vec2 localPos(dot(localX, circlePos), dot(localY, circlePos));

		if (localPos.y < h2 && localPos.y >-h2)
		{
			if (localPos.x > 0 && localPos.x < w2 + circle->radius)
			{
				++otherContacts;
				contact += vec2(w2, localPos.y);
				direction = localX;
				penetration = w2 + circle->radius - localPos.x;
			}
			if (localPos.x < 0 && localPos.x > -(w2 + circle->radius))
			{
				++otherContacts;
				contact += vec2(-w2, localPos.y);
				direction = -localX;
				penetration = localPos.x + (w2 + circle->radius);
			}
		}
		if (localPos.x < w2 && localPos.x >-w2)
		{
			if (localPos.y > 0 && localPos.y < h2 + circle->radius)
			{
				++otherContacts;
				contact += vec2(localPos.x, h2);
				direction = localY;
				penetration = h2 + circle->radius - localPos.y;
			}
			if (localPos.y < 0 && localPos.y >-(h2 + circle->radius))
			{
				++otherContacts;
				contact += vec2(localPos.x, -h2);
				direction = -localY;
				penetration = localPos.y + (h2 + circle->radius); // localPos.y + (h2 + circle->radius);
			}
		}

		numContacts += otherContacts;
	}

	if (numContacts > 0)
	{
		contact = position + (1.f / numContacts) * (localX*contact.x + localY*contact.y);
		// add position again to convert to world space
		resolveCollision(circle, contact, (otherContacts > 0) ? &direction : nullptr);
		penetration /= 2.f;
		position -= normalize(otherContacts ? direction : circlePos) * penetration;
		circle->position += normalize(otherContacts ? direction : circlePos) * penetration;
	}
}

void Box::collideWithPlane(Plane * plane)
{
	int numContacts = 0;
	vec2 contact(0);
	float contactV = 0;
	float radius = 0.5f * std::fminf(width, height);

	float comFromPlane = dot(position - plane->position, plane->normal);
	float penetration = 0;

	for (float x = -width / 2.f; x < width; x += width)
	{
		for (float y = -height / 2.f; y < height; y += height)
		{
			vec2 p = position + x*localX + y*localY;
			float distFromPlane = dot(p - plane->position, plane->normal);

			float velocityIntoPlane = dot(velocity + rotation * (-y*localX + x*localY), plane->normal);

			if ((distFromPlane > 0 && comFromPlane < 0 && velocityIntoPlane > 0) ||
				(distFromPlane < 0 && comFromPlane > 0 && velocityIntoPlane < 0))
			{
				numContacts++;
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

	if (numContacts > 0)
	{
		float collisionV = contactV / (float)numContacts;

		vec2 acceleration = -plane->normal * (1.0f + bounce) * collisionV;

		vec2 localContact = (contact / (float)numContacts) - position;

		float r = dot(localContact, vec2(plane->normal.y, -plane->normal.x));

		float mass0 = 1.0f / (1.0f / mass + (r*r) / moment);

		applyForce(acceleration * mass0, localContact);
		position -= plane->normal * penetration;
	}
}

void Box::collideWithBox(Box * box)
{
}
