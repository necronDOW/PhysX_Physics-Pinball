#ifndef primitiveactors_h
#define primitiveactors_h

#include "../PhysicsEngine.h"

namespace PhysicsEngine
{
	class Plane : public StaticActor
	{
		public:
			Plane(PxVec3 normal = PxVec3(0.f, 1.f, 0.f), PxReal distance = 0.f)
				: StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
			{
				CreateShape(PxPlaneGeometry());
			}
	};

	class Sphere : public DynamicActor
	{
		public:
			Sphere(const PxTransform& pose = PxTransform(PxIdentity), PxReal radius = 1.f, PxReal density = 1.f)
				: DynamicActor(pose)
			{
				CreateShape(PxSphereGeometry(radius), density);
			}
	};

	class Box : public DynamicActor
	{
		public:
			Box(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
				: DynamicActor(pose)
			{
				CreateShape(PxBoxGeometry(dimensions), density);
			}
	};

	class BoxStatic : public StaticActor
	{
		public:
			BoxStatic(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), PxReal density = 1.f)
				: StaticActor(pose)
			{
				CreateShape(PxBoxGeometry(dimensions), density);
			}
	};

	class Capsule : public DynamicActor
	{
		public:
			Capsule(const PxTransform& pose = PxTransform(PxIdentity), PxVec2 dimensions = PxVec2(1.f, 1.f), PxReal density = 1.f)
				: DynamicActor(pose)
			{
				CreateShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
			}
	};
}

#endif