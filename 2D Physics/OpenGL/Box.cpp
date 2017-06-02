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

	w2 = width / 2.f;
	h2 = height / 2.f;

	moment = 1.0f / 12.0f * mass * width * height;
	
	localX = vec2(0, 1);
	localY = vec2(-1, 0);

	p1 = position - localX * w2 - localY * h2;
	p2 = position + localX * w2 - localY * h2;
	p3 = position + localX * w2 + localY * h2;
	p4 = position - localX * w2 + localY * h2;

	objectType = BOX;
}


Box::~Box()
{
}

void Box::draw()
{
	p1 = position - localX * w2 - localY * h2;
	p2 = position + localX * w2 - localY * h2;	
	p3 = position + localX * w2 + localY * h2;
	p4 = position - localX * w2 + localY * h2;

	Gizmos::add2DCircle(p1, 0.2f, 10, vec4(1, 0, 0, 1));
	Gizmos::add2DCircle(p2, 0.2f, 10, vec4(1, 1, 0, 1));
	Gizmos::add2DCircle(p3, 0.2f, 10, vec4(0, 1, 1, 1));
	Gizmos::add2DCircle(p4, 0.2f, 10, vec4(0, 1, 0, 1));

	Gizmos::add2DTri(p1, p2, p3, colour);
	Gizmos::add2DTri(p3, p4, p1, colour);
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
	float boxRadius = length(w2 + h2); // debug

	// excludes circles too far away to collide
	if (length(circlePos) > circle->radius + length(w2 + h2))
		return;

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
			if (length(dp) < circle->radius) //dp.x*dp.x + dp.y*dp.y
			{
				++numContacts;
				contact += vec2(x, y);
				penetration = circle->radius - length(dp);
			}
		}

		// get local position of circle centre
		vec2 localPos(dot(circlePos, localX), dot(circlePos, localY));

		otherContacts = 0;

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

		// calculating contactforces
		penetration /= 2.f;
		position -= normalize(otherContacts ? direction : circlePos) * penetration;
		circle->position += normalize(otherContacts ? direction : circlePos) * penetration;
		// add position again to convert to world space
		resolveCollision(circle, contact, (otherContacts > 0) ? &direction : nullptr);
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

	for (float x = -w2; x < width; x += width)
	{
		for (float y = -h2; y < height; y += height)
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

		position -= plane->normal * penetration;

		applyForce(acceleration * mass0, localContact);
	}
}

void Box::collideWithBox(Box * box)
{
	// relative pos of other box
	vec2 boxPos = box->position - position;

	vec2 norm= vec2(0);
	vec2 contact;
	vec2 contactForce;
	float pen = 0;
	int numContacts = 0;
	checkCorners(box, contact, numContacts, pen, norm, contactForce);
	if (box->checkCorners(this, contact, numContacts, pen, norm, contactForce))
		norm = -norm;

	if (pen > 0)
	{
		if (norm != vec2(0))
		{
			contactForce = normalize(norm) * pen / 2.f;
			position -= contactForce;
			box->position += contactForce;
		}

		resolveCollision(box, contact / (float)numContacts, &norm);
		float numDynamic = 0;
		if (numDynamic > 0)
		{
			vec2 contactForce = norm*pen / numDynamic;
			if (false)
				position -= contactForce;
			if (false)
				box->position += contactForce;
		}
	}
}

bool Box::checkCorners(Box * box, vec2 & contact, int & numContacts, float & pen, vec2 & edgeNormal, vec2 & contactForce)
{
	float minX, maxX, minY, maxY;
	int numLocalContacts = 0;

	float penetration = 0;

	vec2 localContact = vec2(0);

	bool first = true;

	for (float x = -box->w2; x < box->width; x += box->width)
	{
		for (float y = -box->h2; y < box->height; y += box->height)
		{
			// getting position in world space
			vec2 p = box->position + x*box->localX + y*box->localY;

			// projecting into current box's localSpace
			vec2 p0(dot(p - position, localX), dot(p - position, localY));

			if (first || p0.x < minX) minX = p0.x;
			if (first || p0.x > maxX) maxX = p0.x;
			if (first || p0.y < minY) minY = p0.y;
			if (first || p0.y < maxY) maxY = p0.y;

			if (p0.x >= -w2 && p0.x <= w2 && p0.y >= -h2 && p0.y <= h2)
			{
				++numLocalContacts;
				localContact += p0;
			}
			first = false;
		}
	}

	if (maxX < -w2 || minX > w2 || maxY < -h2 || minY > h2)
		return false;
	if (numLocalContacts == 0)
		return false;

	bool res = false;

	contact += position + (localContact.x*localX + localContact.y*localY) / (float)numLocalContacts;
	++numContacts;

	float pen0 = w2 - minX;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = localX;
		pen = pen0;
		res = true;
	}
	pen0 = maxX + w2;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = -localX;
		pen = pen0;
		res = true;
	}
	pen0 = h2 - minY;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = localY;
		pen = pen0;
		res = true;
	}
	pen0 = maxY + h2;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = -localY;
		pen = pen0;
		res = true;
	}
	return res;
}


bool Box::SAT(Box * box)
{
	vec2 vecToOther = box->position - position;

	float distanceToOther = length(vecToOther);
	
	// return false if other box too far to collide
	//if (distanceToOther > length(vec2(h2, w2)) + length(vec2(box->h2, box->w2)))
	//	return false;

	int counter = 0;

	vec2 currentPlane = localX;
	
	while (counter < 2)
	{
		float minX, maxX, minY, maxY;

		float minXB, maxXB, minYB, maxYB;

		for (float x = -w2; x < width; x += width)
		{
			for (float y = -h2; y < height; y += height)
			{

			}
		}
		
		currentPlane = localY;
		++counter;
	}
	
	return false;
}
