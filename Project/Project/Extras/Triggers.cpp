#include "Triggers.h";

void SimulationCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
		{
			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				event_TriggerFound(pairs[i].otherShape, pairs[i].triggerShape);

			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				event_TriggerLost(pairs[i].otherShape, pairs[i].triggerShape);
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

void CustomSimulationCallback::event_TriggerFound(PxShape* shape, PxShape* trigger)
{
	// If the player (PLAYER) hits ...
	if (shape->getSimulationFilterData().word0 == FilterGroup::PLAYER)
	{
		switch (trigger->getSimulationFilterData().word0)
		{
			case FilterGroup::SCOREZONE:
					// ... a score zone.
					Game::Instance().score(200);
					break;
				case FilterGroup::KILLZONE:
					// ... a kill zone.
					Game::Instance().lives(-1);
					break;
				default:
					break;
		}
	}
}

void CustomSimulationCallback::event_TriggerLost(PxShape* shape, PxShape* trigger)
{
	
}

void CustomSimulationCallback::event_ContactFound(PxShape* const* shapes, const int size)
{
	// If the player (PLAYER) hits ...
	if (shapes[1]->getSimulationFilterData().word0 == FilterGroup::PLAYER)
	{
		switch (shapes[0]->getSimulationFilterData().word0)
		{
			case FilterGroup::HITPOINT:
				// ... a hitpoint.
				Game::Instance().score(100);
				break;
			default:
				break;
		}
	}
}

void CustomSimulationCallback::event_ContactLost(PxShape* const* shapes, const int size)
{
	
}