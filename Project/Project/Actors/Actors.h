#ifndef actors_h
#define actors_h

#include <iostream>
#include <iomanip>
#include "Joints.h"
#include "Complex.h"
#include "Primitive.h"
#include "../Extras/Helper.h"

namespace PhysicsEngine
{
	class Plunger
	{
		private:
			vector<DistanceJoint*> springs;
			BoxStatic *bottom;
			Box *top;

		public:
			Plunger(const PxTransform& pose = PxTransform(PxIdentity), const PxVec3& dimensions = PxVec3(1.f, 1.f, 1.f), PxReal thickness = .1f, PxReal stiffness = 1.f, PxReal damping = 1.f)
			{
				bottom = new BoxStatic(PxTransform(pose.p + PxVec3(0.f, thickness, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top = new Box(PxTransform(pose.p + PxVec3(0.f, dimensions.y + thickness, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));

				springs.resize(4);
				springs[0] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, thickness, dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -dimensions.y, dimensions.z)));
				springs[1] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, thickness, -dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -dimensions.y, -dimensions.z)));
				springs[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, thickness, dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -dimensions.y, dimensions.z)));
				springs[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, thickness, -dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -dimensions.y, -dimensions.z)));

				for (unsigned int i = 0; i < springs.size(); i++)
				{
					springs[i]->stiffness(stiffness);
					springs[i]->damping(damping);
				}
			}

			void AddToScene(Scene* scene)
			{
				scene->Add(bottom);
				scene->Add(top);
			}

			~Plunger()
			{
				for (unsigned int i = 0; i < springs.size(); i++)
					delete springs[i];
			}
	};

	class Flipper
	{
		private:
			RevoluteJoint* joint;

		public:
			Flipper(Scene* scene, PxVec3 pos = PxVec3(0), PxVec3 rot = PxVec3(0), float scale = 1.f, float drive = 0.f, float lowerBounds = 0.f, float upperBounds = (PxPi * 2.f))
			{
				Wedge* actor = new Wedge(PxTransform(PxVec3(1), PxQuat(PxHalfPi, PxVec3(0, 0, 1.f))), 1.f, PxVec3(.5f, 1.f, .25f) * scale);
				scene->Add(actor);

				joint = new RevoluteJoint(nullptr, PxTransform(pos, Mathv::EulerToQuat(rot.x, rot.y, rot.z)), actor, PxTransform(PxVec3(0)));
				joint->driveVelocity(drive);
				joint->SetLimits(lowerBounds, upperBounds);
			}

			void InvertDrive()
			{
				joint->driveVelocity(-joint->driveVelocity());
			}
	};
}

#endif