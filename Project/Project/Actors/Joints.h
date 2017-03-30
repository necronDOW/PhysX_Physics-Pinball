#ifndef joints_h
#define joints_h

#include "../PhysicsEngine.h"

namespace PhysicsEngine
{
	class DistanceJoint : public Joint
	{
		public:
			DistanceJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
			{
				// If a first actor is provided, assign a rigidactor object, otherwise the first actor is null and thus this is a static joint.
				PxRigidActor* px_actor0 = 0;
				if (actor0)
					px_actor0 = (PxRigidActor*)actor0->Get();

				// Create a distance joint using the provided actors and poses, when actor0 is nullptr, this will be a static joint.
				joint = (PxJoint*)PxDistanceJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);

				// Enable visualisation and spring physics on the joint.
				joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
				((PxDistanceJoint*)joint)->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);

				// Set the default damping and stiffness values to 1.f.
				damping(1.f);
				stiffness(1.f);
			}

			void stiffness(PxReal value) { ((PxDistanceJoint*)joint)->setStiffness(value); }
			PxReal stiffness() { return ((PxDistanceJoint*)joint)->getStiffness(); }

			void damping(PxReal value) { ((PxDistanceJoint*)joint)->setDamping(value); }
			PxReal damping() { return ((PxDistanceJoint*)joint)->getDamping(); }
	};

	class RevoluteJoint : public Joint
	{
		Actor* pivot;
		Actor* actor;

		public:
			RevoluteJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
			{
				// If a first actor is provided, assign a rigidactor object, otherwise the first actor is null and thus this is a static joint.
				PxRigidActor* px_actor0 = 0;
				if (actor0)
					px_actor0 = (PxRigidActor*)actor0->Get();

				// Create a revolute joint, allowing for rotation on a fixed axis.
				// If the actor0 is equal to nullptr, this joint will once again be static in the environment, i.e. the pivot will remain fixed.
				joint = PxRevoluteJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);

				// Enable visualisation for this joint.
				joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			}

			void driveVelocity(PxReal value)
			{
				// Get both actors within this joint, this will return nullptr if either are null.
				PxRigidDynamic *actor_0, *actor_1;
				((PxRevoluteJoint*)joint)->getActors((PxRigidActor*&)actor_0, (PxRigidActor*&)actor_1);

				// If either of the actors are valid and sleeping, wake them up.
				if (actor_0)
				{
					if (actor_0->isSleeping())
						actor_0->wakeUp();
				}
				if (actor_1)
				{
					if (actor_1->isSleeping())
						actor_1->wakeUp();
				}

				// Set the drive velocity for the joint to the provided value and ensure that the joint flag of 'eDRIVE_ENABLED' is set to true.
				((PxRevoluteJoint*)joint)->setDriveVelocity(value);
				((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			}
			PxReal driveVelocity() { return ((PxRevoluteJoint*)joint)->getDriveVelocity(); }

			void SetLimits(PxReal lower, PxReal upper)
			{
				// Set the angular limits of this joint to a given value, this will mean that any force will not push the joint beyond a given angle.
				((PxRevoluteJoint*)joint)->setLimit(PxJointAngularLimitPair(lower, upper));
				((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
			}
	};

	class SphericalJoint : public Joint
	{
		public:
			SphericalJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
			{
				// If a first actor is provided, assign a rigidactor object, otherwise the first actor is null and thus this is a static joint.
				PxRigidActor* px_actor0 = 0;
				if (actor0)
					px_actor0 = (PxRigidActor*)actor0->Get();
				//else actor1->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

				// Create a spherical joint, allowing for rotation of the x and y axis. If the actor0 is set to nullptr, this joint
				// will remain static within the environment.
				joint = PxSphericalJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);

				// Enable visualisation for this joint.
				joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			}

			void SetLimits(PxReal yLimit, PxReal zLimit)
			{
				// Set the angular limits of this joint to a given value, this will mean that any force will not push the joint beyond a given angle.
				((PxSphericalJoint*)joint)->setLimitCone(PxJointLimitCone(yLimit, zLimit, 0.01f));
				((PxSphericalJoint*)joint)->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
			}
	};
}

#endif