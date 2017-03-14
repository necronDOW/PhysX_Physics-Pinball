#ifndef actors_h
#define actors_h

#include <iostream>
#include <iomanip>
#include "Joints.h"
#include "Complex.h"
#include "Primitive.h"

namespace PhysicsEngine
{
	class Trampoline
	{
		private:
			vector<DistanceJoint*> springs;
			Box *bottom, *top;

		public:
			Trampoline(const PxVec3& dimensions = PxVec3(1.f, 1.f, 1.f), PxReal stiffness = 1.f, PxReal damping = 1.f)
			{
				PxReal thickness = .1f;
				bottom = new Box(PxTransform(PxVec3(0.f, thickness, 0.f)), PxVec3(dimensions.x, thickness, dimensions.z));
				top = new Box(PxTransform(PxVec3(0.f, dimensions.y + thickness, 0.f)), PxVec3(dimensions.x, thickness, dimensions.z));
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

			~Trampoline()
			{
				for (unsigned int i = 0; i < springs.size(); i++)
					delete springs[i];
			}
	};
}

#endif