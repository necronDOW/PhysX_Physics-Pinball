#ifndef helper_h
#define helper_h

#include <string>
#include <iostream>
#include "PxPhysicsAPI.h"

using namespace physx;

class Mathv
{
	public:
		static PxQuat EulerToQuat(float x, float y, float z);
		static PxVec3 Rotate(PxVec3 v, PxReal rad);
		static PxVec3 Mathv::Rotate(PxVec3 v, PxReal rad, PxVec3 axis);
		static PxVec3 Max(PxVec3* values, int size);

};

class IO
{
	public:
		static void Debug(PxVec3 v);
};

#endif