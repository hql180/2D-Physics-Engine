#include "PhysicsObject2D.h"
#include "Plane.h"

void PhysicsObject2D::checkCollisions(PhysicsObject2D * other)
{
	switch (other->objectType)
	{
	case PLANE: 
		collideWithPlane((Plane*)other); 
		break;
	case CIRCLE:
		collideWithCircle((Circle*)other);
	default:
		break;
	}
}
