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
	switch (shapes[0]->getSimulationFilterData().word0)
	{
		case FilterGroup::ACTOR0:
			std::cout << "ball hit" << std::endl;
			break;
		case FilterGroup::ACTOR1:
			Game::Instance().lives(-1);
			break;
		case FilterGroup::ACTOR2:
			break;
		case FilterGroup::ACTOR3:
			break;
		default:
			break;
	}
}

void CustomSimulationCallback::event_ContactLost(PxShape* const* shapes, const int size)
{
	
}