#ifndef triggers_h
#define triggers_h

#include <iostream>
#include "PxPhysicsAPI.h"
#include "../Game.h"

using namespace physx;

struct FilterGroup
{
	enum Enum
	{
		ACTOR0 = (1 << 0),
		ACTOR1 = (1 << 1),
		ACTOR2 = (1 << 2),
		ACTOR3 = (1 << 3)
	};
};

class SimulationCallback : public PxSimulationEventCallback
{
	public:
		bool trigger;

		SimulationCallback() : trigger(false) { }

		void onTrigger(PxTriggerPair* pairs, PxU32 count);
		void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs);
		void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) { }
		void onWake(PxActor **actors, PxU32 count) { }
		void onSleep(PxActor **actors, PxU32 count) { }

		virtual void event_TriggerFound(PxShape* shape) { trigger = true; }
		virtual void event_TriggerLost(PxShape* shape) { trigger = false; }
		virtual void event_ContactFound(PxShape* const* shapes, const int size) { }
		virtual void event_ContactLost(PxShape* const* shapes, const int size) { }
};

class CustomSimulationCallback : public SimulationCallback
{
	public:
		CustomSimulationCallback() : SimulationCallback() { }

		void event_TriggerFound(PxShape* shape) override;
		void event_TriggerLost(PxShape* shape) override;
		void event_ContactFound(PxShape* const* shapes, const int size) override;
		void event_ContactLost(PxShape* const* shapes, const int size) override;
};

#endif