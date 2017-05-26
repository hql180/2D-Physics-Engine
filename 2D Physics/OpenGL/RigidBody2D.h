#pragma once
#include "PhysicsObject2D.h"

class RigidBody2D :
	public PhysicsObject2D
{
public:
	virtual void draw() = 0;
	virtual void update(float dt) = 0;

	void applyForce(vec2 force);

	vec2 position;
	vec2 velocity;
	float mass;
	float bounce;
};

