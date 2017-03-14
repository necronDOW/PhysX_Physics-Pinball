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