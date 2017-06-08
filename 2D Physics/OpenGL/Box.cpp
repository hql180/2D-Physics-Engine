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

	halfWidth = width / 2.f;
	halfHeight = height / 2.f;

	moment = 1.0f / 12.0f * mass * width * height;
	
	localX = vec2(0, 1);
	localY = vec2(-1, 0);

	p1 = position - localX * halfWidth - localY * halfHeight;
	p2 = position + localX * halfWidth - localY * halfHeight;
	p3 = position + localX * halfWidth + localY * halfHeight;
	p4 = position - localX * halfWidth + localY * halfHeight;

	objectType = BOX;

	isFixed = false;
}


Box::~Box()
{
}

void Box::draw()
{
	p1 = position - localX * halfWidth - localY * halfHeight;
	p2 = position + localX * halfWidth - localY * halfHeight;	
	p3 = position + localX * halfWidth + localY * halfHeight;
	p4 = position - localX * halfWidth + localY * halfHeight;

	//Gizmos::add2DCircle(p1, 0.2f, 10, vec4(1, 0, 0, 1));
	//Gizmos::add2DCircle(p2, 0.2f, 10, vec4(1, 1, 0, 1));
	//Gizmos::add2DCircle(p3, 0.2f, 10, vec4(0, 1, 1, 1));
	//Gizmos::add2DCircle(p4, 0.2f, 10, vec4(0, 1, 0, 1));

	Gizmos::add2DCircle(position + localX, 0.2f, 10, vec4(1, 0, 0, 1));
	Gizmos::add2DCircle(position - localX, 0.2f, 10, vec4(0, 0, 1, 1));
	Gizmos::add2DCircle(position + localY, 0.2f, 10, vec4(0, 1, 0, 1));
	Gizmos::add2DCircle(position - localY, 0.2f, 10, vec4(1, 1, 0, 1));

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
	float boxRadius = length(halfWidth + halfHeight); // debug

	// excludes circles too far away to collide
	if (length(circlePos) > circle->radius + length(halfWidth + halfHeight))
		return;

	int numContacts = 0;
	// contact point is in box local space
	vec2 contact(0);
	vec2 direction = vec2(0);
	int otherContacts = 0;

	float penetration = 0;
	// checks to see if any of the four corners are inside circle
	for (float x = -halfWidth; x < width; x += width)
	{
		for (float y = -halfHeight; y < height; y += height)
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

		if (localPos.y < halfHeight && localPos.y >-halfHeight)
		{
			if (localPos.x > 0 && localPos.x < halfWidth + circle->radius)
			{
				++otherContacts;
				contact += vec2(halfWidth, localPos.y);
				direction = localX;
				penetration = halfWidth + circle->radius - localPos.x;
			}
			if (localPos.x < 0 && localPos.x > -(halfWidth + circle->radius))
			{
				++otherContacts;
				contact += vec2(-halfWidth, localPos.y);
				direction = -localX;
				penetration = localPos.x + (halfWidth + circle->radius);
			}
		}
		if (localPos.x < halfWidth && localPos.x >-halfWidth)
		{
			if (localPos.y > 0 && localPos.y < halfHeight + circle->radius)
			{
				++otherContacts;
				contact += vec2(localPos.x, halfHeight);
				direction = localY;
				penetration = halfHeight + circle->radius - localPos.y;
			}
			if (localPos.y < 0 && localPos.y >-(halfHeight + circle->radius))
			{
				++otherContacts;
				contact += vec2(localPos.x, -halfHeight);
				direction = -localY;
				penetration = localPos.y + (halfHeight + circle->radius); // localPos.y + (h2 + circle->radius);
			}
		}

		numContacts += otherContacts;
	}

	if (numContacts > 0)
	{
		contact = position + (1.f / numContacts) * (localX*contact.x + localY*contact.y);

		// calculating contactforces
		penetration *= 0.5f;
		if (!isFixed)
			position -= normalize(otherContacts ? direction : circlePos) * penetration;
		if (!circle->isFixed)
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

	for (float x = -halfWidth; x < width; x += width)
	{
		for (float y = -halfHeight; y < height; y += height)
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
	if (length(position - box->position) > length(vec2(halfWidth, halfHeight) + length(vec2(box->halfWidth, box->halfHeight))))
		return;
	
	vec2 n1, n2;
	vec2 contact;
	vec2 cf1 = vec2(0), cf2 = vec2(0);
	float pen1 = 0, pen2 = 0;

	int numContacts = 0;
	bool b1 = false, b2 = false;
	b1 = checkCorners(box, contact, numContacts, n1, pen1, cf1);
	b2 = box->checkCorners(this, contact, numContacts, n2, pen2, cf2);
	if (b1 || b2)
	{
		vec2 contactForce = (cf1 - cf2) * 0.5f;

		float penetration = (pen1 + pen2) * 0.5f;


		vec2 edgeNormal = normalize(n1 - n2);



		//if (penetration == 0)
		//{
		//	position -= contactForce;
		//	box->position += contactForce;
		//}
		//else
		//{
		//	position -= edgeNormal * penetration;
		//	box->position += edgeNormal * penetration;
		//}

		if (!isFixed)
			position -= contactForce;
		if (!box->isFixed)
			box->position += contactForce;

		resolveCollision(box, contact / (float)numContacts, &edgeNormal);



	}
}

bool Box::checkCorners(Box * box, vec2 & contact, int & numContacts, vec2 & edgeNormal, float& pen, vec2& contactForce)
{
	float penetration = 0;

	// looping through local extents of other box
	for (float x = -box->halfWidth; x <= box->width; x += box->width)
	{
		for (float y = -box->halfHeight; y <= box->height; y += box->height)
		{
			// position in world space
			vec2 p = box->position + x*box->localX + y*box->localY;

			// projecting into current box's localSpace
			vec2 p0(dot(p - position, localX), dot(p - position, localY));

			float offSet = 1.f;

			//if (p0.y * offSet <= halfHeight  && p0.y * offSet>= -halfHeight)
			//{
			//	if (p0.x > 0 && p0.x < halfWidth* offSet) // checks if penetrating right hand side
			//	{
			//		edgeNormal = localX;
			//		penetration = halfWidth - p0.x;
			//	}
			//	if (p0.x < 0 && p0.x > -halfWidth* offSet) // checks if penetrating left hand side
			//	{
			//		edgeNormal = -localX;
			//		penetration = halfWidth + p0.x;
			//	}
			//}
			//if (p0.x * offSet <= halfWidth && p0.x * offSet>= -halfWidth)
			//{
			//	if (p0.y > 0 && p0.y < halfHeight* offSet) // top face
			//	{
			//		float pen0 = halfHeight - p0.y;
			//		if (pen0 < penetration || penetration == 0)
			//		{
			//			penetration = pen0;
			//			edgeNormal = localY;
			//		}
			//	}
			//	if (p0.y < 0 && p0.y >= -halfHeight* offSet) // bottom face
			//	{
			//		float pen0 = halfHeight + p0.y;
			//		if (pen0 < penetration || penetration == 0)
			//		{
			//			penetration = pen0;
			//			edgeNormal = -localY;
			//		}
			//	}
			//}

			float error = 0.0f;

			if (p0.y < halfHeight && p0.y > -halfHeight) // checks if other box is within left or right face of box
			{
				if (p0.x > 0 && p0.x < halfWidth) // checks if penetrating right hand side
				{
					++numContacts;
					contact += position + halfWidth * localX + p0.y * localY; // right face
					edgeNormal = localX;
					penetration = halfWidth - p0.x;
					if (halfHeight - fabs(p0.y) < error && halfWidth - fabs(p0.x) < error)
						pen = halfWidth - p0.x * offSet;
				}
				if (p0.x < 0 && p0.x > -halfWidth) // checks if penetrating left hand side
				{
					++numContacts;
					contact += position - halfWidth * localX + p0.y * localY; // left face
					edgeNormal = -localX;
					penetration = halfWidth + p0.x;
					if (halfHeight - fabs(p0.y) < error && halfWidth - fabs(p0.x) < error)
						pen = halfWidth + p0.x * offSet;
				}
			}
			if (p0.x < halfWidth && p0.x > -halfWidth) // checks for penetration / contact on top and bottom face of box
			{
				if (p0.y > 0 && p0.y < halfHeight) // top face
				{
					++numContacts;
					contact += position + p0.x * localX + halfHeight * localY;
					float pen0 = halfHeight - p0.y;
					if (pen0 < penetration || penetration == 0) 
					{
						penetration = pen0;
						edgeNormal = localY;

					}
					if (halfHeight - fabs(p0.y) < error && halfWidth - fabs(p0.x) < error)
						pen = halfHeight - p0.y * offSet;

				}
				if (p0.y < 0 && p0.y > -halfHeight) // bottom face
				{
					++numContacts;
					contact += position + p0.x * localX - halfHeight * localY;
					float pen0 = halfHeight + p0.y;
					if (pen0 < penetration || penetration == 0)
					{
						penetration = pen0;
						edgeNormal = -localY;
					}
					if (halfHeight - fabs(p0.y) < error && halfWidth - fabs(p0.x) < error)
						pen = halfHeight + p0.y * offSet;
				}
			}

		}
	}

	contactForce = penetration * edgeNormal;

	if (pen != 0)
	{
		contactForce = pen * edgeNormal;
	}
	
	return (penetration != 0);	
}
