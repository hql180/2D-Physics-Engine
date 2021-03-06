#include "PhysicsObject2D.h"

#include "Plane.h"
#include "Circle.h"
#include "Box.h"

bool PhysicsObject2D::isInside(vec2 pt)
{
	return false;
}

void PhysicsObject2D::checkCollisions(PhysicsObject2D * other)
{
	switch (other->objectType)
	{
	case PLANE: 
		collideWithPlane((Plane*)other); 
		break;
	case CIRCLE:
		collideWithCircle((Circle*)other);
		break;
	case BOX:
		collideWithBox((Box*)other);
		break;
	default:
		break;
	}
}
