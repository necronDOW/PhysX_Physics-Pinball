#ifndef helper_h
#define helper_h

#include <string>
#include <iostream>
#include <vector>
#include "PxPhysicsAPI.h"

using namespace physx;

class Mathv
{
	public:
		static PxQuat EulerToQuat(float x, float y, float z);
		static PxVec3 Rotate(PxVec3 v, PxReal rad);
		static PxVec3 Mathv::Rotate(PxVec3 v, PxReal rad, PxVec3 axis);
		static PxVec3 Max(PxVec3* values, int size);
		static std::vector<PxVec3> Plot(PxVec3 start, float startRads, float radVarience, PxVec3 scalar = PxVec3(1.f));

};

class IO
{
	public:
		static void Debug(PxVec3 v);
};

#endif