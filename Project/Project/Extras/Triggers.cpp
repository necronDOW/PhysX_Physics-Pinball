#include "Triggers.h";

void SimulationCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	// Core callback functionality for trigger collisions, this remains unchanged in child classes which simply override the
	// event_ functions. Check each actor within the given pair ...
	for (PxU32 i = 0; i < count; i++)
	{
		if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
		{
			// If a trigger occurs, execute the virtual void event_TriggerFound.
			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				event_TriggerFound(pairs[i].otherShape, pairs[i].triggerShape);

			// If a trigger is lost, execute the virtual void event_TriggerLost.
			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				event_TriggerLost(pairs[i].otherShape, pairs[i].triggerShape);
		}
	}
}

void SimulationCallback::onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs)
{
	// Core callback functionality for trigger collisions, this remains unchanged in child classes which simply override the
	// event_ functions. Check each actor within the given pair ...
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		// If contact occurs, execute the virtual void event_ContactFound.
		if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			event_ContactFound(pairs[i].shapes, 2);

		// If contact is lost, execute the virtual void event_ContactLost.
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
					// ... a score zone. Increment the gamestate score by 200.
					Game::Instance().score(200);
					break;
				case FilterGroup::KILLZONE:
					// ... a kill zone. Decrement the gamestate lives by 1.
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
				// ... a hitpoint. Increment the gamestate score by 100.
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