#include "RigidBody2D.h"

void RigidBody2D::applyForce(vec2 force, vec2 pos)
{
	velocity += force / mass;
	rotation += (force.y * pos.x - force.x * pos.y) / moment;
}
