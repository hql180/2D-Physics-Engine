#pragma once
#include "PhysicsObject2D.h"

class RigidBody2D;
class Spring :
	public PhysicsObject2D
{
public:
	Spring(RigidBody2D* rb1 = NULL, RigidBody2D* rb2 = NULL, vec4 col = vec4(1), vec2& const c1 = vec2(0), vec2& const c2 = vec2(0), float rLength = 5.f, float rForce = 0.2f, float mLength = 0, float damp = 1.f);
	~Spring() {}

	virtual void draw();
	virtual void update(float dt);
	virtual void collideWithCircle(Circle* circle) {}
	virtual void collideWithPlane(Plane* plane) {}
	virtual void collideWithBox(Box* box) {}

	vec2 toWorld(RigidBody2D* rb, vec2& contact);
	void getPoints();

	RigidBody2D* body1;
	RigidBody2D* body2;

	vec2 p1, p2;
	vec2 contact1;
	vec2 contact2;
	vec4 colour;
	float restLength;
	float maxLength;
	float restoringForce;
	float damping;
};

