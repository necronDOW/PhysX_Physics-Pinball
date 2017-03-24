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
		PLAYER = (1 << 0),
		HITPOINT = (1 << 1),
		SCOREZONE = (1 << 2),
		KILLZONE = (1 << 3)
	};
};

class SimulationCallback : public PxSimulationEventCallback
{
	public:
		SimulationCallback() { }

		void onTrigger(PxTriggerPair* pairs, PxU32 count);
		void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs);
		void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) { }
		void onWake(PxActor **actors, PxU32 count) { }
		void onSleep(PxActor **actors, PxU32 count) { }

		virtual void event_TriggerFound(PxShape* shape, PxShape* trigger) { }
		virtual void event_TriggerLost(PxShape* shape, PxShape* trigger) { }
		virtual void event_ContactFound(PxShape* const* shapes, const int size) { }
		virtual void event_ContactLost(PxShape* const* shapes, const int size) { }
};

class CustomSimulationCallback : public SimulationCallback
{
	public:
		CustomSimulationCallback() : SimulationCallback() { }

		void event_TriggerFound(PxShape* shape, PxShape* trigger) override;
		void event_TriggerLost(PxShape* shape, PxShape* trigger) override;
		void event_ContactFound(PxShape* const* shapes, const int size) override;
		void event_ContactLost(PxShape* const* shapes, const int size) override;
};

#endif