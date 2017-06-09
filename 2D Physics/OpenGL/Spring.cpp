#include "Spring.h"

#include "RigidBody2D.h"

#include "Gizmos.h"

Spring::Spring(RigidBody2D * rb1, RigidBody2D * rb2, vec4 col, vec2 & const c1, vec2 & const c2, float rLength, float rForce, float mLength, float damp)
{	
	body1 = rb1;
	body2 = rb2;
	
	contact1 = c1;
	contact2 = c2;

	restLength = rLength;
	maxLength = mLength;

	colour = col;

	restoringForce = rForce;
	damping = damp;
}

void Spring::draw()
{
	Gizmos::add2DLine(p1, p2, colour);
}

void Spring::update(float dt)
{
	if (body1 && body2)
	{
		getPoints();

		vec2 direction = p2 - p1;
		float len = length(direction);

		// dampening
		vec2 dv = body2->velocity - body1->velocity;

		vec2 force = direction * restoringForce * (restLength - len) - damping * dv * dt;

		body1->applyForce(-force, p1 - body1->position);
		body2->applyForce(force, p2 - body2->position);
	}
}

vec2 Spring::toWorld(RigidBody2D * rb, vec2 & contact)
{
	return rb->position + contact;
}

void Spring::getPoints()
{
	p1 = toWorld(body1, contact1);
	p2 = toWorld(body2, contact2);
}
