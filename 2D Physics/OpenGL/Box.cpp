#include "stdafx.h"
#include "Box.h"

#include "Circle.h"
#include "Plane.h"

#include "Gizmos.h"

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
	isAwake = false;
}


void Box::draw()
{
	halfWidth = width / 2.f;
	halfHeight = height / 2.f;

	p1 = position - localX * halfWidth - localY * halfHeight;
	p2 = position + localX * halfWidth - localY * halfHeight;	
	p3 = position + localX * halfWidth + localY * halfHeight;
	p4 = position - localX * halfWidth + localY * halfHeight;

	Gizmos::add2DCircle(p1, 0.2f, 10, vec4(1, 0, 0, 1));
	Gizmos::add2DCircle(p2, 0.2f, 10, vec4(1, 1, 0, 1));
	Gizmos::add2DCircle(p3, 0.2f, 10, vec4(0, 1, 1, 1));
	Gizmos::add2DCircle(p4, 0.2f, 10, vec4(0, 1, 0, 1));

	Gizmos::add2DTri(p1, p2, p3, colour);
	Gizmos::add2DTri(p3, p4, p1, colour);
}


void Box::collideWithCircle(Circle * circle)
{
	// relative position of circle
	vec2 circlePos = circle->position - position;

	float circleDist = length(circlePos);

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
			if (length(dp) < circle->radius)
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
				penetration = localPos.y + (halfHeight + circle->radius);
			}
		}

		numContacts += otherContacts;
	}

	if (numContacts > 0)
	{
		contact = position + (1.f / numContacts) * (localX*contact.x + localY*contact.y);

		// calculating contactforces
		if (!isFixed && !circle->isFixed)
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
	
	vec2 contact;
	float pen = 0;

	int numContacts = 0;

	vec2 edgeNormal;

	bool b1 = checkBoxCorners(box, contact, numContacts, pen, edgeNormal);
	bool b2 = box->checkBoxCorners(this, contact, numContacts, pen, edgeNormal);

	if (b1 || b2)
	{
		if (b2)
			edgeNormal = -edgeNormal;

		resolveCollision(box, contact / (float)numContacts, &edgeNormal);

		if (pen > 0)
		{
			vec2 contactForce = edgeNormal * pen;
			if (!isFixed)
				position -= contactForce;

			if (!box->isFixed)
				box->position += contactForce;
		}
	}
}

bool Box::isInside(vec2 pt)
{
	pt -= position;
	vec2 boxPt(dot(pt, localX), dot(pt, localY));
	return (fabs(boxPt.x) < halfWidth && fabs(boxPt.y) < halfHeight);
}

bool Box::checkBoxCorners(Box* box, glm::vec2& contact, int& numContacts, float &pen, glm::vec2& edgeNormal)
{
	float minX, maxX, minY, maxY;
	int numLocalContacts = 0;
	glm::vec2 localContact;
	bool first = true;
	for (float x = -box->halfWidth; x < box->width; x += box->width)
	{
		for (float y = -box->halfHeight; y < box->height; y += box->height)
		{
			glm::vec2 p = box->position + x*box->localX + y*box->localY; // position in worldspace
			glm::vec2 p0(glm::dot(p - position, localX), glm::dot(p - position, localY)); // position in our box's space
			if (first || p0.x < minX) minX = p0.x;
			if (first || p0.x > maxX) maxX = p0.x;
			if (first || p0.y < minY) minY = p0.y;
			if (first || p0.y > maxY) maxY = p0.y;
			if (p0.x >= -halfWidth && p0.x <= halfWidth && p0.y >= -halfHeight && p0.y <= halfHeight)
			{
				numLocalContacts++;
				localContact += p0;
			}
			first = false;
		}
	}
	if (maxX <-halfWidth || minX >halfWidth || maxY<-halfHeight || minY >halfHeight)
		return false;
	if (numLocalContacts == 0)
		return false;
	bool res = false;
	contact += position + (localContact.x*localX + localContact.y*localY) / (float)numLocalContacts;
	numContacts++;
	float pen0 = halfWidth - minX;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = localX;
		pen = pen0;
		res = true;
	}
	pen0 = maxX + halfWidth;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = -localX;
		pen = pen0;
		res = true;
	}
	pen0 = halfHeight - minY;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = localY;
		pen = pen0;
		res = true;
	}
	pen0 = maxY + halfHeight;
	if (pen0 > 0 && (pen0 < pen || pen == 0))
	{
		edgeNormal = -localY;
		pen = pen0;
		res = true;
	}
	return res;
}