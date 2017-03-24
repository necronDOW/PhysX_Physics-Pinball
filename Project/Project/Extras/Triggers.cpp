#include "Triggers.h";

void SimulationCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
		{
			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				event_TriggerFound(pairs[i].otherShape);

			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				event_TriggerLost(pairs[i].otherShape);
		}
	}
}

void SimulationCallback::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			event_ContactFound(pairs[i].shapes, 2);

		if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			event_ContactLost(pairs[i].shapes, 2);
	}
}

void CustomSimulationCallback::event_TriggerFound(PxShape* shape)
{
	
}

void CustomSimulationCallback::event_TriggerLost(PxShape* shape)
{
	
}

void CustomSimulationCallback::event_ContactFound(PxShape* const* shapes, const int size)
{
	// If the player (ACTOR0) hits ...
	if (shapes[1]->getSimulationFilterData().word0 == FilterGroup::ACTOR0)
	{
		switch (shapes[0]->getSimulationFilterData().word0)
		{
			case FilterGroup::ACTOR1:
				// ... a hitpoint.
				Game::Instance().score(100);
				break;
			case FilterGroup::ACTOR2:
				// ... a score zone.
				Game::Instance().score(200);
				break;
			case FilterGroup::ACTOR3:
				// ... a kill zone.
				Game::Instance().lives(-1);
				break;
			default:
				break;
		}
	}
}

void CustomSimulationCallback::event_ContactLost(PxShape* const* shapes, const int size)
{
	
}