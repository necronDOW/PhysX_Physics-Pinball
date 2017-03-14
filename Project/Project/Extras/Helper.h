#ifndef helper_h
#define helper_h

#include "PxPhysicsAPI.h"

using namespace physx;

class Mathv
{
	public:
		static PxQuat EulerToQuat(float x, float y, float z);
};

#endif