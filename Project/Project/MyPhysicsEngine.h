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

	static const PxReal PxQuartPi = PxHalfPi / 2.f;
	static const PxVec3 color_palette[] = { PxVec3(1.f), PxVec3(.2f), PxVec3(1.f,.3f,.3f), PxVec3(.3f,.3f,1.f), PxVec3(1.f,.65f,.3f) };

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
		private:
			Plane *plane;
			Platform *platform;
			Pinball *ball;
			CustomSimulationCallback *my_callback;
			PxTransform* initialBallPos;
		
		public:
			Plunger *plunger;
			Flipper *flipperL;
			Flipper *flipperR;

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

				InitActors();

				Game::Instance().player(ball->Get());
			}

			virtual void CustomUpdate() 
			{
				
			}

			virtual void PostUpdate()
			{
				Game::Instance().Update();
			}

			void InitActors()
			{
				float angleZ = -PxPi / 3.f;

				plane = new Plane();
				plane->Color(PxVec3(210.f / 255.f, 210.f / 255.f, 210.f / 255.f));
				Add(plane);

				platform = new Platform(PxTransform(PxVec3(.0f, 7.f, .0f), Mathv::EulerToQuat(0, 0, angleZ)), 4, .05f, PxVec3(4.f, 8.f, 5.f));
				platform->Materials(MaterialLibrary::Instance().New("wood", 0.125f, 0.f, 0.603f));
				platform->SetColor(color_palette[0], color_palette[1]);
				Add(platform);

				ball = new Pinball(platform->RelativeTransform(PxVec2(.9475f, -.4f)), .1f, 1.f);
				ball->Material(MaterialLibrary::Instance().New("steel", 0.25f, 0.f, 0.597f), 0);
				ball->Color(color_palette[3]);
				Add(ball);

				plunger = new Plunger(platform->RelativeTransform(PxVec2(.9475f, -.9825f)), PxVec3(.1f, .1f, .25f), 1.545f, .05f, 12.f, .5f);
				plunger->SetColor(color_palette[2]);
				plunger->AddToScene(this);

				flipperL = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(-.3f, -.75f)), Mathv::EulerToQuat(0, PxHalfPi, PxHalfPi)), 30.0f);
				flipperR = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(.3f, -.75f)), Mathv::EulerToQuat(0, PxHalfPi, -PxHalfPi)), -30.0f);

				// WALLS
				// Left Walls
				AddWall(PxVec2(-.4f, -.85f), PxHalfPi, .9f);
				AddWall(PxVec2(-.675f, -.55f), -PxQuartPi, 1.325f);
				AddWall(PxVec2(-.755f, -0.1525f), -PxQuartPi * 2.75f, 1.25f, 3, .125f);
				AddWall(PxVec2(-.5f, .725f), PxPi + PxQuartPi, 2.f, 4, .225f);
				AddWall(PxVec2(-.755f, .2125f), -PxQuartPi*1.25f, 1.25f, 3, .15f);

				// Right Walls
				AddWall(PxVec2(.4f, -.85f), PxHalfPi, .9f);
				AddWall(PxVec2(.62f, -.5775f), PxQuartPi, 1.12f);
				AddWall(PxVec2(.755f, -0.1525f), PxQuartPi * 2.75f, 1.25f, 3, .125f);
				AddWall(PxVec2(.5f, .725f), PxHalfPi + PxQuartPi, 2.f, 4, .225f);
				AddWall(PxVec2(.755f, .2125f), PxQuartPi*1.25f, 1.25f, 3, .15f);

				// Central Walls
				AddWall(PxVec2(.45f, .475f), PxHalfPi, 1.5f, 4, .25f);

				// HITPOINTS
				// Upper
				AddHitpoint(PxVec2(-.5f, .7f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(-.1f, .2f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(-.3f, .4f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(.1f, .4f), -PxHalfPi, PxVec2(.2f, .02f));

				// Lower
				AddHitpoint(PxVec2(-.4f, -.3f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(.4f, -.3f), -PxHalfPi, PxVec2(.2f, .02f));

				// TRIGGER ZONES
				AddTrigger(PxVec2(0.f, -1.1f), 0.f, 1.1f, FilterGroup::KILLZONE);
				AddTrigger(PxVec2(.65f, .135f), .2f, .5f, FilterGroup::SCOREZONE);
				AddTrigger(PxVec2(.65f, .81f), -.2f, .5f, FilterGroup::SCOREZONE);
			}

			Flipper* AddFlipper(const PxTransform& transform, float initDrive, const char* material = "wood", PxVec3 color = color_palette[2])
			{
				Flipper* f = new Flipper(this, transform, .8f, initDrive, -PxPi / 4.f, PxPi / 4.f);
				f->SetMaterial(MaterialLibrary::Instance().Get(material));
				f->SetColor(color);

				return f;
			}

			Hitpoint* AddHitpoint(PxVec2 placement, PxReal rotation, PxVec2 scale, PxVec3 color = color_palette[4])
			{
				Hitpoint* hp = new Hitpoint(this, Mathv::Multiply(platform->RelativeTransform(placement, -.15f), PxQuat(rotation, PxVec3(0, 1, 0))), scale, .1f);
				hp->SetMaterial(MaterialLibrary::Instance().Get("wood"));
				hp->Get()->SetupFiltering(FilterGroup::HITPOINT, FilterGroup::PLAYER);
				hp->Get()->Color(color);

				return hp;
			}

			void AddWall(PxVec2 placement, PxReal rotation, float scale = 1.f, int divisions = 0, float bendFactor = 0.f, float height = .25f, float thickness = .05f)
			{
				CurvedWall* cw = new CurvedWall(Mathv::Multiply(platform->RelativeTransform(placement), PxQuat(rotation, PxVec3(0, 0, 1))), scale, divisions, bendFactor, height, thickness);
				cw->SetMaterial(MaterialLibrary::Instance().Get("wood"));
				Add(cw);
			}

			void AddTrigger(PxVec2 placement, PxReal rotation, PxReal scale, int filterGroup)
			{
				TriggerZone* trigger = new TriggerZone(Mathv::Multiply(platform->RelativeTransform(placement), PxQuat(rotation, PxVec3(0, 0, 1))), PxVec3(scale), filterGroup);
				Add(trigger);
			}
	};
}

#endif