#include "RigidBody2D.h"

void RigidBody2D::applyForce(vec2 force)
{
	velocity += force / mass;
}
