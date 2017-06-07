#include "RigidBody2D.h"

#include <stdio.h> // Debug

void RigidBody2D::update(float dt)
{
	angle += rotation * dt;
	position += velocity * dt;

	float cs = cosf(angle);
	float sn = sinf(angle);

	localX = vec2(cs, sn);
	localY = vec2(-sn, cs);

	velocity += gravity * dt;
}

void RigidBody2D::applyForce(vec2 force, vec2 pos)
{
	velocity += force / mass;
	rotation += (force.y * pos.x - force.x * pos.y) / moment;
}

void RigidBody2D::resolveCollision(RigidBody2D * other, vec2 contact, vec2 * direction)
{
	// finds vector between centres if direction is not provided
	vec2 unitDisp = direction ? *direction : normalize(other->position - position);

	// get perpendicular vector
	vec2 unitPerp(unitDisp.y, -unitDisp.x);

	// calculate total velocity of contact points, both linear and rotational
	float r1 = dot(contact - position, -unitPerp);
	float r2 = dot(contact - other->position, unitPerp);
	float v1 = dot(velocity, unitDisp) + r1 * rotation;
	float v2 = dot(other->velocity, unitDisp) - r2 * other->rotation;

	if (v1 > v2) // moving closer
	{
		// calulate effect mass at contact point for each object
		float mass1 = 1.f / (1.f / mass + (r1*r1) / moment);
		float mass2 = 1.f / (1.f / other->mass + (r2*r2) / other->moment);

		float totalBounce = (bounce + other->bounce) / 2.f;

		//vec2 force = (1.f + bounce)*mass1*mass2 / (mass1 + mass2)*(v1 - v2)*unitDisp;
		vec2 force = (1.f + totalBounce)*mass1*mass2 / (mass1 + mass2)*(v1 - v2)*unitDisp;

		// apply equal and opposite forces according to each object's restitution/bounciness

		applyForce(-force, contact - position);
		other->applyForce(force, contact - other->position);
	}
}
