#pragma once
#include "PhysicsObject2D.h"

class RigidBody2D :
	public PhysicsObject2D
{
public:
	virtual void draw() = 0;
	virtual void update(float dt);

	void applyForce(vec2 force, vec2 pos);

	void resolveCollision(RigidBody2D* other, vec2 contact, vec2* direction = NULL);

	vec2 position;
	vec2 velocity;
	float rotation;
	float mass;
	float bounce;
	float angle;
	float moment;

	vec2 localX;
	vec2 localY;

	vec4 colour;
};

