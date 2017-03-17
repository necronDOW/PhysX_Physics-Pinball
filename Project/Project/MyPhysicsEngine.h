#ifndef myphysicsengine_h
#define myphysicsengine_h

#include <iostream>
#include <iomanip>
#include "Actors/Actors.h"
#include "Extras/MaterialLibrary.h"

namespace PhysicsEngine
{
	using namespace std;

	static const PxVec3 color_palette[] = { PxVec3(.3f), PxVec3(1.f,.3f,.3f) };

	struct FilterGroup
	{
		enum Enum
		{
			ACTOR0		= (1 << 0),
			ACTOR1		= (1 << 1),
			ACTOR2		= (1 << 2)
		};
	};

	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		bool trigger;

		MySimulationEventCallback() : trigger(false) {}

		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) 
		{
			for (PxU32 i = 0; i < count; i++)
			{
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
				{
					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_FOUND" << endl;
						trigger = true;
					}

					if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
					{
						cerr << "onTrigger::eNOTIFY_TOUCH_LOST" << endl;
						trigger = false;
					}
				}
			}
		}

		virtual void onContact(const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 nbPairs) 
		{
			cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			for (PxU32 i = 0; i < nbPairs; i++)
			{
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}

				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {}
		virtual void onWake(PxActor **actors, PxU32 count) {}
		virtual void onSleep(PxActor **actors, PxU32 count) {}
	};

	static PxFilterFlags CustomFilterShader( PxFilterObjectAttributes attributes0,	PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,	PxFilterData filterData1,
		PxPairFlags& pairFlags,	const void* constantBlock,	PxU32 constantBlockSize)
	{
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eSOLVE_CONTACT;
		pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
		
		if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
//			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane *plane;
		Platform *platform;
		Flipper *flipperL;
		Flipper *flipperR;
		Sphere *ball;
		Plunger *plunger;
		MySimulationEventCallback *my_callback;
		
		public:
			MyScene() : Scene(CustomFilterShader) {};

			void SetVisualisation()
			{
				px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
				px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
				px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
				px_scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
			}

			virtual void CustomInit() 
			{
				SetVisualisation();			

				GetMaterial()->setDynamicFriction(.2f);

				my_callback = new MySimulationEventCallback();
				px_scene->setSimulationEventCallback(my_callback);
				px_scene->setFlag(PxSceneFlag::eENABLE_CCD, true);

				float angleZ = -PxPi / 4.f;

				plane = new Plane();
				plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
				Add(plane);

				platform = new Platform(PxTransform(PxVec3(.0f,7.f,.0f), Mathv::EulerToQuat(0, 0, angleZ)), 4, .05f, PxVec3(5.f, 10.f, 5.f));
				platform->Materials(MaterialLibrary::Instance().New("wood", 0.125f, 0.f, 0.603f));
				platform->SetColor(color_palette[1], color_palette[0]);
				Add(platform);

				ball = new Sphere(platform->RelativeTransform(PxVec2(.3f, .9f)), .1f, 1.f);
				ball->Material(MaterialLibrary::Instance().New("steel", 0.25f, 0.f, 0.597f), 0);
				ball->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				Add(ball);

				plunger = new Plunger(PxTransform(platform->RelativeTransform(PxVec2(.8f, -.9f)).p, Mathv::EulerToQuat(0, 0, angleZ)), PxVec3(.24f), .05f, 100.f, 1.f);
				plunger->SetColor(color_palette[1]);
				plunger->AddToScene(this);

				flipperL = new Flipper(this, platform->RelativeTransform(PxVec2(-.4f, -.8f)).p, PxVec3(angleZ, PxHalfPi, PxHalfPi), .9f, 20.f, -PxPi/4.f, PxPi/4.f);
				flipperL->SetColor(color_palette[1]);

				flipperR = new Flipper(this, platform->RelativeTransform(PxVec2(.4f, -.8f)).p, PxVec3(angleZ, PxHalfPi, -PxHalfPi), .9f, -20.f, -PxPi / 4.f, PxPi / 4.f);
				flipperR->SetColor(color_palette[1]);
			}

			virtual void CustomUpdate() 
			{
			}

			void Hit()
			{
				flipperL->InvertDrive();
				flipperR->InvertDrive();
			}
	};
}

#endif