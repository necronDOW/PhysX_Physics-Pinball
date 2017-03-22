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
			BoxStatic* walls[2];
			BoxStatic *bottom;
			Box *top;

		public:
			Plunger(const PxTransform& pose = PxTransform(PxIdentity), const PxVec3& dimensions = PxVec3(1.f, 1.f, 1.f), PxReal surfaceDistance = 1.f, PxReal thickness = .1f, PxReal stiffness = 1.f, PxReal damping = 1.f)
			{
				bottom = new BoxStatic(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top = new Box(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

				PxReal wallLength = (surfaceDistance / 2.f) + thickness;
				walls[0] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(dimensions.x + thickness, surfaceDistance/2.f, 0.f)), pose.q), PxVec3(thickness*.95f, wallLength, dimensions.z));
				walls[1] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(-dimensions.x - thickness, surfaceDistance/2.f, 0.f)), pose.q), PxVec3(thickness*.95f, wallLength, dimensions.z));

				springs.resize(4);
				springs[0] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, 0.f, dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -surfaceDistance, dimensions.z)));
				springs[1] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, 0.f, -dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -surfaceDistance, -dimensions.z)));
				springs[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, 0.f, dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -surfaceDistance, dimensions.z)));
				springs[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, 0.f, -dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -surfaceDistance, -dimensions.z)));

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
				
				for (int i = 0; i < 2; i++)
					scene->Add(walls[i]);
			}

			void SetColor(PxVec3 rgb)
			{
				bottom->Color(rgb, 0);
				top->Color(rgb, 0);
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
			Wedge* wedge;
			RevoluteJoint* joint;

		public:
			Flipper(Scene* scene, PxTransform pose, float scale = 1.f, float drive = 0.f, float lowerBounds = 0.f, float upperBounds = (PxPi * 2.f))
			{
				wedge = new Wedge(PxTransform(PxIdentity), 1.f, PxVec3(.5f, 1.f, .25f) * scale);
				scene->Add(wedge);

				joint = new RevoluteJoint(nullptr, pose, wedge, PxTransform(PxVec3(0)));
				joint->driveVelocity(drive);
				joint->SetLimits(lowerBounds, upperBounds);
			}

			void SetColor(PxVec3 rgb)
			{
				wedge->Color(rgb, 0);
			}

			void InvertDrive()
			{
				joint->driveVelocity(-joint->driveVelocity());
			}

			void SetMaterial(PxMaterial* material)
			{
				wedge->Material(material, 0);
			}
	};

	class Pinball : public Sphere
	{
		public:
			Pinball(const PxTransform& pose = PxTransform(PxIdentity), PxReal radius = 1.f, PxReal density = 1.f)
				: Sphere(pose, radius, density)
			{

			}
	};
}

#endif