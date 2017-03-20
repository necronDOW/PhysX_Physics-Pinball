#ifndef myphysicsengine_h
#define myphysicsengine_h

#include <iostream>
#include <iomanip>
#include "Actors/Actors.h"
#include "Extras/MaterialLibrary.h"
#include "Extras/Triggers.h"

namespace PhysicsEngine
{
	using namespace std;

	static const PxVec3 color_palette[] = { PxVec3(.3f), PxVec3(1.f,.3f,.3f), PxVec3(.3f,.3f,1.f) };

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
			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
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
		CustomSimulationCallback *my_callback;
		
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

				my_callback = new CustomSimulationCallback();
				px_scene->setSimulationEventCallback(my_callback);
				px_scene->setFlag(PxSceneFlag::eENABLE_CCD, true);

				float angleZ = -PxPi / 4.f;

				plane = new Plane();
				plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
				Add(plane);

				platform = new Platform(PxVec3(.0f,7.f,.0f), PxVec3(0, 0, angleZ), 4, .05f, PxVec3(5.f, 10.f, 5.f));
				platform->Materials(MaterialLibrary::Instance().New("wood", 0.125f, 0.f, 0.603f));
				platform->SetColor(color_palette[1], color_palette[0]);
				platform->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1);
				Add(platform);

				ball = new Sphere(platform->RelativeTransform(PxVec3(.8f, -.6f, 0.f)), .1f, 1.f);
				ball->Material(MaterialLibrary::Instance().New("steel", 0.25f, 0.f, 0.597f), 0);
				ball->Color(color_palette[2]);
				ball->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				ball->SetupFiltering(FilterGroup::ACTOR1, FilterGroup::ACTOR0);
				Add(ball);

				plunger = new Plunger(PxTransform(platform->RelativeTransform(PxVec3(.8f, -.8f, -0.1f)).p, Mathv::EulerToQuat(0, 0, angleZ)), PxVec3(.2f), .3f, .05f, 300.0f, 1.f);
				plunger->SetColor(color_palette[1]);
				plunger->AddToScene(this);

				flipperL = new Flipper(this, platform->RelativeTransform(PxVec3(-.4f, -.7f, .0f)).p, PxVec3(angleZ, PxHalfPi, PxHalfPi), .9f, 20.f, -PxPi/4.f, PxPi/4.f);
				flipperL->SetMaterial(MaterialLibrary::Instance().Get("wood"));
				flipperL->SetColor(color_palette[1]);

				flipperR = new Flipper(this, platform->RelativeTransform(PxVec3(.4f, -.7f, .0f)).p, PxVec3(angleZ, PxHalfPi, -PxHalfPi), .9f, -20.f, -PxPi / 4.f, PxPi / 4.f);
				flipperR->SetMaterial(MaterialLibrary::Instance().Get("wood"));
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