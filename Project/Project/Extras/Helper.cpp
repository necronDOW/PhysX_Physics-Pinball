#include "Helper.h"

PxQuat Mathv::EulerToQuat(float x, float y, float z)
{
	float c1 = cos(y / 2);
	float s1 = sin(y / 2);
	float c2 = cos(x / 2);
	float s2 = sin(x / 2);
	float c3 = cos(z / 2);
	float s3 = sin(z / 2);

	return physx::PxQuat(s1*s2*c3 + c1*c2*s3, s1*c2*c3 + c1*s2*s3, c1*s2*c3 - s1*c2*s3, c1*c2*c3 - s1*s2*s3);
}

PxVec3 Mathv::Rotate(PxVec3 v, PxReal rad)
{
	PxReal cO = cos(rad);
	PxReal sO = sin(rad);

	return PxVec3(cO*v.x - sO*v.y, sO*v.x + cO*v.y, v.z);
}

PxVec3 Mathv::Rotate(PxVec3 v, PxReal rad, PxVec3 axis)
{
	PxReal c = cos(rad);
	PxReal s = sin(rad);

	if (axis.x == 1)
		return PxVec3(v.x*c - v.y*s, v.x*s + v.y*c, v.z);
	else if (axis.y == 1)
		return PxVec3(v.x*c + v.z*s, v.y, -v.x*s + v.z*c);
	else if (axis.z == 1)
		return PxVec3(v.x, v.y*c - v.z*s, v.y*s + v.z*c);

	return v;
}

PxVec3 Mathv::Max(PxVec3* values, int size)
{
	PxVec3 maxV = PxVec3(0.f);
	
	for (int i = 0; i < size; i++)
	{
		if (values[i].x > maxV.x)
			maxV.x = values[i].x;

		if (values[i].y > maxV.y)
			maxV.y = values[i].y;

		if (values[i].z > maxV.z)
			maxV.z = values[i].z;
	}

	return maxV;
}

void IO::Debug(PxVec3 v)
{
	std::cout << v.x << "," << v.y << "," << v.z << std::endl;
}