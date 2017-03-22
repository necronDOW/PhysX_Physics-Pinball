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
	static const PxVec3 color_palette[] = { PxVec3(1.f), PxVec3(.3f), PxVec3(1.f,.3f,.3f), PxVec3(.3f,.3f,1.f) };

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
			Sphere *ball;
			Plunger *plunger;
			CustomSimulationCallback *my_callback;
		
		public:
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

				float angleZ = -PxPi / 3.f;

				plane = new Plane();
				plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
				Add(plane);

				platform = new Platform(PxTransform(PxVec3(.0f, 7.f, .0f), Mathv::EulerToQuat(0, 0, angleZ)), 4, .05f, PxVec3(4.f, 8.f, 5.f));
				platform->Materials(MaterialLibrary::Instance().New("wood", 0.125f, 0.f, 0.603f));
				platform->SetColor(color_palette[0], color_palette[1]);
				platform->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1);
				Add(platform);

				ball = new Sphere(platform->RelativeTransform(PxVec2(.875f, -.4f)), .1f, 1.f);
				ball->Material(MaterialLibrary::Instance().New("steel", 0.25f, 0.f, 0.597f), 0);
				ball->Color(color_palette[3]);
				ball->Get()->isRigidBody()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
				ball->SetupFiltering(FilterGroup::ACTOR1, FilterGroup::ACTOR0);
				Add(ball);

				plunger = new Plunger(platform->RelativeTransform(PxVec2(.875f, -.97f)), PxVec3(.05f, .1f, .25f), 2.6f, .125f, 3.0f, 1.5f);
				plunger->SetColor(color_palette[2]);
				plunger->AddToScene(this);

				flipperL = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(-.35f, -.725f)), Mathv::EulerToQuat(0, PxHalfPi, PxHalfPi)), 30.0f);
				flipperR = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(.35f, -.725f)), Mathv::EulerToQuat(0, PxHalfPi, -PxHalfPi)), -30.0f);

				// Left Walls
				AddWall(PxVec2(-.4f, -.85f), PxHalfPi, .9f);
				AddWall(PxVec2(-.675f, -.55f), -PxQuartPi, 1.325f);
				AddWall(PxVec2(-.75f, -0.1f), PxPi + PxQuartPi, 1.f, 3, .15f);
				AddWall(PxVec2(-.5f, .725f), PxPi + PxQuartPi, 2.f, 4, .225f);
				AddWall(PxVec2(-.755f, .2125f), -PxQuartPi*1.25f, 1.25f, 3, .15f);

				// Right Walls
				AddWall(PxVec2(.4f, -.85f), PxHalfPi, .9f);
				AddWall(PxVec2(.5625f, -.605f), PxQuartPi, .875f);
				AddWall(PxVec2(.75f, -0.1f), PxHalfPi + PxQuartPi, 1.f, 3, .15f);
				AddWall(PxVec2(.5f, .725f), PxHalfPi + PxQuartPi, 2.f, 4, .225f);
				AddWall(PxVec2(.755f, .2125f), PxQuartPi*1.25f, 1.25f, 3, .15f);
				AddWall(PxVec2(.45f, .475f), PxHalfPi, 1.5f, 4, .25f);
			}

			virtual void CustomUpdate() 
			{
				
			}

			Flipper* AddFlipper(const PxTransform& transform, float initDrive, const char* material = "wood", PxVec3 color = color_palette[2])
			{
				Flipper* f = new Flipper(this, transform, .8f, initDrive, -PxPi / 4.f, PxPi / 4.f);
				f->SetMaterial(MaterialLibrary::Instance().Get(material));
				f->SetColor(color);

				return f;
			}

			void AddWall(PxVec2 placement, PxReal rotation, float scale = 1.f, int divisions = 0, float bendFactor = 0.f, float height = .25f, float thickness = .05f)
			{
				CurvedWall* cw = new CurvedWall(Mathv::Multiply(platform->RelativeTransform(placement), PxQuat(rotation, PxVec3(0, 0, 1))), scale, divisions, bendFactor, height, thickness);
				cw->SetMaterial(MaterialLibrary::Instance().Get("wood"));
				Add(cw);
			}
	};
}

#endif