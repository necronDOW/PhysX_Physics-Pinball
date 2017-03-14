#pragma once

#include <vector>
#include "PxPhysicsAPI.h"
#include "Exception.h"
#include "Extras\UserData.h"
#include <string>

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;
	
	void PxInit();

	void PxRelease();

	PxPhysics* GetPhysics();

	PxCooking* GetCooking();

	PxMaterial* GetMaterial(PxU32 index=0);

	PxMaterial* CreateMaterial(PxReal sf=.0f, PxReal df=.0f, PxReal cr=.0f);

	static const PxVec3 default_color(.8f,.8f,.8f);

	class Actor
	{
	protected:
		PxActor* actor;
		std::vector<PxVec3> colors;
		std::string name;

	public:
		Actor()
			: actor(0)
		{
		}

		PxActor* Get();

		void Color(PxVec3 new_color, PxU32 shape_index=-1);

		const PxVec3* Color(PxU32 shape_indx=0);

		void Actor::Name(const string& name);

		string Actor::Name();

		void Material(PxMaterial* new_material, PxU32 shape_index=-1);

		PxShape* GetShape(PxU32 index=0);

		std::vector<PxShape*> Actor::GetShapes(PxU32 index=-1);

		virtual void CreateShape(const PxGeometry& geometry, PxReal density) {}

		void SetTrigger(bool value, PxU32 index=-1);

		void SetupFiltering(PxU32 filterGroup, PxU32 filterMask, PxU32 shape_index=-1);
	};

	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const PxTransform& pose);

		~DynamicActor();

		void CreateShape(const PxGeometry& geometry, PxReal density);

		void SetKinematic(bool value, PxU32 index=-1);
	};

	class StaticActor : public Actor
	{
	public:
		StaticActor(const PxTransform& pose);

		~StaticActor();

		void CreateShape(const PxGeometry& geometry, PxReal density=0.f);
	};

	class Scene
	{
	protected:
		PxScene* px_scene;
		bool pause;
		PxRigidDynamic* selected_actor;
		std::vector<PxVec3> sactor_color_orig;
		PxSimulationFilterShader filter_shader;

		void HighlightOn(PxRigidDynamic* actor);

		void HighlightOff(PxRigidDynamic* actor);

	public:
		Scene(PxSimulationFilterShader custom_filter_shader=PxDefaultSimulationFilterShader) : filter_shader(custom_filter_shader) {}

		void Init();

		virtual void CustomInit() {}

		void Update(PxReal dt);

		virtual void CustomUpdate() {}

		void Add(Actor* actor);

		PxScene* Get();

		void Reset();

		void Pause(bool value);

		bool Pause();

		PxRigidDynamic* GetSelectedActor();

		void SelectNextActor();

		std::vector<PxActor*> GetAllActors();
	};

	class Joint
	{
	protected:
		PxJoint* joint;

	public:
		Joint() : joint(0) {}

		PxJoint* Get() { return joint; }
	};


}

