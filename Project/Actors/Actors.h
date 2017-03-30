#ifndef actors_h
#define actors_h

// C++ Includes
#include <iostream>
#include <iomanip>

// Custom Includes
#include "Joints.h"
#include "Complex.h"
#include "Primitive.h"
#include "../Extras/Helper.h"
#include "../Extras/Triggers.h"

namespace PhysicsEngine
{
	/// <summary>
	/// A class containing a number of DistanceJoints as springs and two connected boxes, one static and one dynamic.
	/// </summary>
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
			/// <summary>
			/// <para>Plunger Constructor (+0 overloads).</para>
			///	<para>const ref PxTransform pose : The pose of the plunger, this describes to base position for the spring joint. (default : PxTransform(PxIdentity))</para>
			/// <para>const ref PxVec3 dimensions : The dimensions for the Plunger, this describes the width of each surface. (default : PxVec3(1.0f))</para>
			/// <para>PxReal surfaceDistance : The distance between each of the two surfaces for this plunger. (default : 1.0f))</para>
			/// <para>PxReal thickness : The thickness of each surface and additionally the walls surrounding the spring mechanism. (default : 0.1f))</para>
			/// <para>PxReal stiffness : The initial stiffness of the springs. (default : 1.0f)</para>
			/// <para>PxReal damping : The initial damping value of the springs. (default : 1.0f)</para>
			/// </summary>
			Plunger(const PxTransform& pose = PxTransform(PxIdentity), const PxVec3& dimensions = PxVec3(1.f), PxReal surfaceDistance = 1.f, PxReal thickness = .1f, PxReal stiffness = 1.f, PxReal damping = 1.f)
			{
				// Set the required members for this function.
				_transform = pose;
				_stiffness = stiffness;

				// Initialize the two surfaces of the plunger at their respective positions and enable CCD on the dynamic (top) surface.
				bottom = new BoxStatic(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top = new Box(PxTransform(pose.p + PxVec3(0.f, 0.f, 0.f), pose.q), PxVec3(dimensions.x, thickness, dimensions.z));
				top->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

				// Initialize the walls at either side of the plunger surfaces.
				walls[0] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(dimensions.x + thickness, surfaceDistance - thickness, 0.f)), pose.q), PxVec3(thickness*.99f, surfaceDistance, dimensions.z));
				walls[1] = new BoxStatic(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(-dimensions.x - thickness, surfaceDistance - thickness, 0.f)), pose.q), PxVec3(thickness*.99f, surfaceDistance, dimensions.z));

				// Initialize 4 springs attached to each corner of the platforms.
				springs.resize(4);
				springs[0] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, 0.f, dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -surfaceDistance, dimensions.z)));
				springs[1] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x, 0.f, -dimensions.z)), top, PxTransform(PxVec3(dimensions.x, -surfaceDistance, -dimensions.z)));
				springs[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, 0.f, dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -surfaceDistance, dimensions.z)));
				springs[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x, 0.f, -dimensions.z)), top, PxTransform(PxVec3(-dimensions.x, -surfaceDistance, -dimensions.z)));

				// Set the stiffness and dampness of all 4 springs to the provided parameter values.
				for (unsigned int i = 0; i < springs.size(); i++)
				{
					springs[i]->stiffness(stiffness);
					springs[i]->damping(damping);
				}
			}

			/// <summary>
			/// <para>Add the relavant Actors within this class to the provided Scene.</para>
			///	<para>Scene* scene : The scene to add the actors to.</para>
			/// </summary>
			void AddToScene(Scene* scene)
			{
				// Add the bottom and top surfaces of the plunger to the scene.
				scene->Add(bottom);
				scene->Add(top);
				
				// Add the walls of the plunger to the scene.
				for (int i = 0; i < 2; i++)
					scene->Add(walls[i]);
			}

			/// <summary>
			/// <para>Set the surface colors for this object.</para>
			///	<para>PxVec3 rgb : The new color to change the surfaces to.</para>
			/// </summary>
			void SetColor(PxVec3 rgb)
			{
				// Set the colors of the two plunger surfaces, ignore the walls, maintaining their default color.
				bottom->Color(rgb, 0);
				top->Color(rgb, 0);
			}

			/// <summary>
			/// <para>Pull the plunger downwards, this disables stiffness and adds a downwards force.</para>
			/// </summary>
			void Pull()
			{
				// Add a force to the dynamic surface moving it towards the static platform.
				top->Get()->isRigidDynamic()->addForce(Mathv::Multiply(_transform.q, PxVec3(0, -1, 0))*10);

				// Remove any stiffness from all 4 springs to ensure the best results when applying the force.
				for (unsigned int i = 0; i < springs.size(); i++)
					springs[i]->stiffness(0.f);
			}

			/// <summary>
			/// <para>Release the plunger, this should be called after executing <seealso cref="Plunger::Pull"/> to reset stiffness.</para>
			/// </summary>
			void Release()
			{
				// Reset the 4 spring stiffness' back to the default values.
				for (unsigned int i = 0; i < springs.size(); i++)
					springs[i]->stiffness(_stiffness);
			}

			~Plunger()
			{
				// Delete all of the springs from memory.
				for (unsigned int i = 0; i < springs.size(); i++)
					delete springs[i];
			}
	};

	/// <summary>
	/// A class which presents a wedge shape attached to a pivot point.
	/// </summary>
	class Flipper
	{
		private:
			Wedge* wedge;
			RevoluteJoint* joint;

		public:
			/// <summary>
			/// <para>Flipper Constructor (+0 overloads).</para>
			///	<para>Scene* scene : The scene to add the wedge actor to.</para>
			///	<para>const ref PxTransform pose : The pose of the flipper, this is applied to the pivot point rather than the wedge. (default : PxTransform(PxIdentity))</para>
			///	<para>float scale : The scale of the wedge. (default : 1.0f)</para>
			///	<para>float drive : The initial drive velocity of this flipper. (default : 0.0f)</para>
			///	<para>float lowerBounds : The lower bounds of the flipper in radians, the flipper will not surpass this angular limit. (default : 0.0f)</para>
			///	<para>float upperBounds : The upper bounds of the flipper in radians, the flipper will not surpass this angular limit. (default : (PxPi * 2.0f))</para>
			/// </summary>
			Flipper(Scene* scene, const PxTransform& pose = PxTransform(PxIdentity), float scale = 1.f, float drive = 0.f, float lowerBounds = 0.f, float upperBounds = (PxPi * 2.f))
			{
				// Set the wedge dimensions, these values are tried and tested to provide the best results.
				PxVec3 wedgeDimensions = PxVec3(.5f, 1.f, .3f);

				// Initialize the wedge body of the flipper, positioning it so that the thickest part is central to the joint. Also enable CCD on the wedge.
				wedge = new Wedge(PxTransform(pose.p + Mathv::Multiply(pose.q, PxVec3(0.f, wedgeDimensions.z / 2.f, 0.f)), pose.q), 1.f, wedgeDimensions * scale);
				wedge->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

				// Add the wedge to the scene provided.
				scene->Add(wedge);

				// Initialize the RevoluteJoint, providing nullptr as the first actor causing a static position and rotation.
				joint = new RevoluteJoint(nullptr, pose, wedge, PxTransform(PxVec3(0.f, wedgeDimensions.z / 2.f, 0.f)));

				// Set the drive velocity and limits of the joint to the provided parameter values.
				joint->driveVelocity(drive);
				joint->SetLimits(lowerBounds, upperBounds);
			}

			/// <summary>
			/// <para>Set the color of the wedge.</para>
			///	<para>PxVec3 rgb : The color to set the wedge actor to.</para>
			/// </summary>
			void SetColor(PxVec3 rgb)
			{
				// Set the edge colour to the provided rgb value.
				wedge->Color(rgb, 0);
			}

			/// <summary>
			/// <para>Invert the drive of this flipper, this will cause it to repel towards its set limit.</para>
			/// </summary>
			void InvertDrive()
			{
				// Invert the drive velocity of the joint.
				joint->driveVelocity(-joint->driveVelocity());
			}

			/// <summary>
			/// <para>Set the material properties of the wedge.</para>
			///	<para>PxMaterial* material : The material properties to be used on the wedge actor.</para>
			/// </summary>
			void SetMaterial(PxMaterial* material)
			{
				// Set the material of the wedge.
				wedge->Material(material, 0);
			}
	};

	/// <summary>
	/// A class outlining the default parameters and behaviour for a pinball. (inherits : <seealso cref="Sphere"/>)
	/// </summary>
	class Pinball : public Sphere
	{
		public:
			/// <summary>
			/// <para>Pinball Constructor (+0 overloads).</para>
			///	<para>const ref PxTransform pose : The initial pose for the pinball and its sphere actor. (default : PxTransform(PxIdentity))</para>
			///	<para>PxReal radius : The radius of the sphere. (default : 1.0f)</para>
			///	<para>PxReal density : The density of the actor. (default : 1.0f)</para>
			/// </summary>
			Pinball(const PxTransform& pose = PxTransform(PxIdentity), PxReal radius = 1.f, PxReal density = 1.f)
				: Sphere(pose, radius, density)
			{
				// Set the CCD flags for the sphere actor.
				Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

				// Set the filtering parameters for this object to be a PLAYER and interact with HITPOINTS.
				SetupFiltering(FilterGroup::PLAYER, FilterGroup::HITPOINT);
			}
	};

	/// <summary>
	/// A class containing the necessary parameters and actors for a pinball hitpoint, these are attached via spherical joints and thus teeter on a given pivot point.
	/// </summary>
	class Hitpoint
	{
		private:
			SphericalJoint* _joint;
			Capsule* _body;

		public:
			/// <summary>
			/// <para>Hitpoint Constructor (+0 overloads).</para>
			///	<para>Scene* scene : The scene that the capsule body will be added to.</para>
			///	<para>const ref PxTransform pose : The initial pose for this object, this relates to the transform of the joint rather than the body. (default : PxTransform(PxIdentity))</para>
			///	<para>PxVec2 scale : The scale of the capsule body. (default : 0.0f)</para>
			///	<para>float density : The density of the capsule body. (default : 1.0f)</para>
			/// </summary>
			Hitpoint(Scene* scene, const PxTransform& pose = PxTransform(PxIdentity), PxVec2 scale = PxVec2(1.f), PxReal density = 1.f)
			{
				// Initialize the body using the provided values and add it to the scene.
				_body = new Capsule(pose, scale, density);
				scene->Add(_body);

				// Create the SphericalJoint with nullptr as actor 0, creating a static rotation and position for the body to teeter on.
				_joint = new SphericalJoint(nullptr, pose, _body, PxTransform(Mathv::Multiply(pose.q, PxVec3(0.f, 0.f, scale.x + scale.y))));
				_joint->SetLimits(PxPi/8, PxPi/8);
			}

			/// <summary>
			/// <para>Set the material of the capsule body.</para>
			///	<para>PxMaterial* material : The material properties to assign to the capsule body.</para>
			/// </summary>
			void SetMaterial(PxMaterial* material)
			{
				// Set the material properties of the body to the provided parameter values.
				_body->Material(material, 0);
			}

			/// <summary>
			/// <para>Get the body for this Hitpoint.</para>
			/// </summary>
			Actor* Get()
			{
				// Return the body of this hitpoint as an Actor pointer.
				return _body;
			}
	};

	/// <summary>
	/// A class which provides the necessary setup for triggers, this takes the form of a box primitive object.
	/// </summary>
	class TriggerZone : public BoxStatic
	{
		private:
			bool visible = false;

		public:
			/// <summary>
			/// <para>function name.</para>
			///	<para>const ref PxTransform pose : The transform of this trigger zone, this object has no simulation steps and thus its transform will remain constant. (default : PxTransform(PxIdentity))</para>
			///	<para>PxVec3 dimensions : The dimensions of the trigger. (default : PxVec3(0.5f))</para>
			///	<para>int filterGroup : The filter group for this trigger, this in un-important for trigger interaction but provides object differentiation. Refer to <seealso cref="FilterGroup"/>. (default : 0)</para>
			/// </summary>
			TriggerZone(const PxTransform& pose = PxTransform(PxIdentity), PxVec3 dimensions = PxVec3(.5f), int filterGroup = 0)
				: BoxStatic(pose, dimensions)
			{
				// Set all of the necessary flags for the BoxStatic to become a trigger shape.
				PxShape* shape = GetShape();
				shape->setFlag(PxShapeFlag::eVISUALIZATION, visible);
				shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

				// Setup the filtering for the box to interact with the PLAYER.
				SetupFiltering(filterGroup, FilterGroup::PLAYER);
			}

			/// <summary>
			/// <para>Toggle the visibility of this trigger, this will display the entire body in a solid form.</para>
			/// </summary>
			void ToggleVisible()
			{
				// Toggle the visualization properties of the BoxStatic shape.
				GetShape()->setFlag(PxShapeFlag::eVISUALIZATION, visible = !visible);
			}
	};
}

#endif