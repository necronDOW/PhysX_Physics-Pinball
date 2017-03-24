#ifndef actors_h
#define actors_h

#include <iostream>
#include <iomanip>
#include "Joints.h"
#include "Complex.h"
#include "Primitive.h"
#include "../Extras/Helper.h"
#include "../Extras/Triggers.h"

namespace PhysicsEngine
{
	class Plunger
	{
		private:
			vector<DistanceJoint*> springs;
			BoxStatic* walls[2];
			BoxStatic *bottom;
			Box *top;
			PxTransform _transform;
			PxReal _stiffness;

		public:
			Plunger(const PxTransform& pose = PxTransform(PxIdentity), const PxVec3& dimensions = PxVec3(1.f, 1.f, 1.f), PxReal surfaceDistance = 1.f, PxReal thickness = .1f, PxReal stiffness = 1.f, PxReal damping = 1.f)
			{
				_transform = pose;
				_stiffness = stiffness;

				bottom = new BoxStatic(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top = new Box(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

				walls[0] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(dimensions.x + thickness, surfaceDistance - thickness, 0.f)), pose.q), PxVec3(thickness*.99f, surfaceDistance, dimensions.z));
				walls[1] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(-dimensions.x - thickness, surfaceDistance - thickness, 0.f)), pose.q), PxVec3(thickness*.99f, surfaceDistance, dimensions.z));

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

			void Pull()
			{
				top->Get()->isRigidDynamic()->addForce(Mathv::Multiply(_transform.q, PxVec3(0, -1, 0))*10);
				for (unsigned int i = 0; i < springs.size(); i++)
					springs[i]->stiffness(0.f);
			}

			void Release()
			{
				for (unsigned int i = 0; i < springs.size(); i++)
					springs[i]->stiffness(_stiffness);
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
			Flipper(Scene* scene, const PxTransform& pose = PxTransform(PxIdentity), float scale = 1.f, float drive = 0.f, float lowerBounds = 0.f, float upperBounds = (PxPi * 2.f))
			{
				PxVec3 wedgeDimensions = PxVec3(.5f, 1.f, .3f);
				wedge = new Wedge(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(0.f, wedgeDimensions.z / 2.f, 0.f)), pose.q), 1.f, wedgeDimensions * scale);
				wedge->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				scene->Add(wedge);

				joint = new RevoluteJoint(nullptr, pose, wedge, PxTransform(PxVec3(0.f, wedgeDimensions.z / 2.f, 0.f)));
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
				Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				SetupFiltering(FilterGroup::PLAYER, FilterGroup::HITPOINT);
			}
	};

	class Hitpoint
	{
		private:
			SphericalJoint* _joint;
			Capsule* _body;

		public:
			Hitpoint(Scene* scene, const PxTransform& pose = PxTransform(PxIdentity), PxVec2 scale = PxVec2(0.f), float density = 1.f)
			{
				_body = new Capsule(pose, scale, density);
				scene->Add(_body);

				PxVec3 bodyP = Mathv::Multiply(pose.q, PxVec3(0.f, 0.f, scale.x+scale.y));
				_joint = new SphericalJoint(nullptr, pose, _body, PxTransform(bodyP));
				_joint->SetLimits(PxPi/8, PxPi/8);
			}

			void SetMaterial(PxMaterial* material)
			{
				_body->Material(material, 0);
			}

			Actor* Get()
			{
				return _body;
			}
	};

	class TriggerZone : public BoxStatic
	{
		private:
			bool visible = false;

		public:
			TriggerZone(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f, .5f, .5f), int filterGroup = -1)
				: BoxStatic(pose, dimensions)
			{
				PxShape* shape = GetShape();
				shape->setFlag(PxShapeFlag::eVISUALIZATION, visible);
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

				SetupFiltering(filterGroup, FilterGroup::PLAYER);
			}

			void ToggleVisible()
			{
				GetShape()->setFlag(PxShapeFlag::eVISUALIZATION, visible = !visible);
			}
	};
}

#endif