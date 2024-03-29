#include "Helper.h"

PxQuat Mathv::EulerToQuat(float x, float y, float z)
{
	// Very basic trigonometry to calculate a PxQuat from a provided set of X, Y, and Z values (euler angles).
	float c1 = cos(y / 2);
	float s1 = sin(y / 2);
	float c2 = cos(x / 2);
	float s2 = sin(x / 2);
	float c3 = cos(z / 2);
	float s3 = sin(z / 2);

	return physx::PxQuat(s1*s2*c3 + c1*c2*s3, s1*c2*c3 + c1*s2*s3, c1*s2*c3 - s1*c2*s3, c1*c2*c3 - s1*s2*s3);
}

PxVec3 Mathv::Rotate(PxVec3 v, PxReal rad, PxVec3 axis)
{
	// Returns a rotated vector v by a radius of rad on a given axis. For example, (1, 0, 0) rotated by PxHalfPi on the
	// y-axis is would return (0, 0, 1);
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
	// Retrieve the maximum PxVec3 in a set of vectors.
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

std::vector<PxVec3> Mathv::Plot(PxVec3 start, float startRads, float radVarience, PxVec3 scalar)
{
	// Plot (PxPi*2 / radVarience) number of elements around a central point, each moving in radial increments of radVarience.
	// The initial rotation is set by startRads and the scalar can amplify the radius of the plotted points.
	int size = (PxPi * 2.f) / radVarience;
	std::vector<PxVec3> arr = std::vector<PxVec3>(size);

	for (int i = 0; i < size; i++)
	{
		arr[i] = Rotate(start, startRads, PxVec3(1, 0, 0)).multiply(scalar);
		startRads += radVarience;
	}

	return arr;
}

void IO::Debug(PxVec3 v)
{
	// Debug helper for PxVec3.
	std::cout << v.x << "," << v.y << "," << v.z << std::endl;
}

PxVec3 Mathv::Multiply(PxQuat quat, PxVec3 vec)
{
	// More complex trigonometry functions for vector and quaternion multiplication. The goal is to provide a value whereby
	// the vector has been rotated by a given quaternion.
	float num = quat.x * 2.f;
	float num2 = quat.y * 2.f;
	float num3 = quat.z * 2.f;
	float num4 = quat.x * num;
	float num5 = quat.y * num2;
	float num6 = quat.z * num3;
	float num7 = quat.x * num2;
	float num8 = quat.x * num3;
	float num9 = quat.y * num3;
	float num10 = quat.w * num;
	float num11 = quat.w * num2;
	float num12 = quat.w * num3;

	PxVec3 result;
	result.x = (1.f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
	result.y = (num7 + num12) * vec.x + (1.f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
	result.z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.f - (num4 + num5)) * vec.z;
	return result;
}

PxTransform Mathv::Multiply(PxTransform t, PxQuat q)
{
	// Simple multiplaction extensions for multiplying a PxQuat within a PxTransform by another given PxQuat.
	return PxTransform(t.p, t.q * q);
}