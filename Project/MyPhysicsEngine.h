#ifndef myphysicsengine_h
#define myphysicsengine_h

#include <iostream>
#include <iomanip>
#include "Actors/Actors.h"
#include "Extras/MaterialLibrary.h"
#include "Extras/ColorLibrary.h"
#include "Extras/Triggers.h"

namespace PhysicsEngine
{
	using namespace std;

	static const PxReal PxQuartPi = PxHalfPi / 2.f;	// Quarter of PI

	static PxFilterFlags CustomFilterShader( PxFilterObjectAttributes attributes0,	PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,	PxFilterData filterData1,
		PxPairFlags& pairFlags,	const void* constantBlock,	PxU32 constantBlockSize)
	{
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		// Enable CCD and CONTACT reporting on the scene level.
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

	class MyScene : public Scene
	{
		private:
			// Actor variables for storing pointers to newly created actors which require some level of interaction after
			// they have been added to the scene.
			Plane *plane;
			Platform *platform;						// Reference kept for access to Platform::RelativeTransform.
			PxTransform* initialBallPos;			// Storage of the ball 'spawn' position.
			Pinball *ball;							// Reference kept for moving the player back to spawn.
			std::vector<TriggerZone*> triggers;		// Reference to the triggers within the scene for visualisation toggling.
			CustomSimulationCallback *my_callback;	// Pointer to a CustomSimulationCallback.
		
		public:
			// Public Actor variables which require access in other classes after they have been added to the scene.
			Plunger *plunger;
			Flipper *flipperL;
			Flipper *flipperR;

			MyScene() : Scene(CustomFilterShader) {};

			void SetVisualisation()
			{
				// Enable visualisation of:
				//    # scale gizmos
				//    # collision wireframes
				//    # joint transforms
				//    # joint limits (i.e. cones and angular)
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

				// Enable CCD on the scene.
				px_scene->setFlag(PxSceneFlag::eENABLE_CCD, true);

				// Call the InitActors function to build all of the necessary actors for the scene.
				InitActors();

				// Set the player reference in the game manager class.
				Game::Instance().player(ball->Get());
			}

			virtual void CustomUpdate() 
			{
				
			}

			virtual void PostUpdate()
			{
				// Update the current game state, this will update the player position, if necessary, after fetchResults has
				// been called. This is essential to avoiding API errors when performing direct and immediate re-positioning
				// of actors in a scene using setGlobalPose.
				Game::Instance().Update();
			}

			void InitActors()
			{
				// This describes the tilt factor of the table.
				float table_tilt = -PxPi / 3.f;

				// Initialize a basic plane and add it to the scene.
				plane = new Plane();
				plane->Color(PxVec3(210.f / 255.f, 210.f / 255.f, 210.f / 255.f));
				Add(plane);

				// Initialize the pinball table as a Platform with its top cap set to transparent, and add it to the scene.
				// The material library is utilised here to create a wood texture with a set of real-life parameters. The
				// table is coloured with white walls and a 20% gray floor using the colour library Fetch function.
				platform = new Platform(PxTransform(PxVec3(.0f, 7.f, .0f), Mathv::EulerToQuat(0, 0, table_tilt)), 4, .05f, PxVec3(4.f, 8.f, 5.f));
				platform->Materials(MaterialLibrary::Instance().New("wood", 0.125f, 0.f, 0.603f));
				platform->SetColor(LColor::Get().Fetch("white"), LColor::Get().Fetch("gray-20"));
				Add(platform);

				// Initialize the pinball object at a relative transform which places it above the intended plunger position.
				// The material library is utilised here to create a steel texture with a set of real-life parameters. The ball
				// is coloured a soft blue using the colour library fetch function.
				ball = new Pinball(platform->RelativeTransform(PxVec2(.9475f, -.4f)), .1f, 1.f);
				ball->Material(MaterialLibrary::Instance().New("steel", 0.25f, 0.f, 0.597f), 0);
				ball->Color(LColor::Get().Fetch("soft-blue"));
				Add(ball);

				// Initialize the plunger at roughly the bottom right of the platform, colouring it soft red.
				plunger = new Plunger(platform->RelativeTransform(PxVec2(.9475f, -.9825f)), PxVec3(.1f, .1f, .25f), 1.545f, .05f, 12.f, .5f);
				plunger->SetColor(LColor::Get().Fetch("soft-red"));
				plunger->AddToScene(this);

				// Initialize both the flippers in a mirrored fashion towards the lower end of the table. Multiple rotations
				// are executed and the Mathv::Multiply(PxTransform, PxQuat) function is utilised to neaten this up as much
				// as possition.
				flipperL = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(-.3f, -.75f)), Mathv::EulerToQuat(0, PxHalfPi, PxHalfPi)), 30.0f);
				flipperR = AddFlipper(Mathv::Multiply(platform->RelativeTransform(PxVec2(.3f, -.75f)), Mathv::EulerToQuat(0, PxHalfPi, -PxHalfPi)), -30.0f);

				// Initialize and add all of the walls/obstacles to the platform at a selection of tried and tested positions.
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

				// Initialize and all all of the hitpoints within the scene, this describes with obstaces which can be
				// interacted with and which provide score to the player.
				// Upper
				AddHitpoint(PxVec2(-.5f, .7f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(-.1f, .2f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(-.3f, .4f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(.1f, .4f), -PxHalfPi, PxVec2(.2f, .02f));

				// Lower
				AddHitpoint(PxVec2(-.4f, -.3f), -PxHalfPi, PxVec2(.2f, .02f));
				AddHitpoint(PxVec2(.4f, -.3f), -PxHalfPi, PxVec2(.2f, .02f));

				// Initialize and add all of the trigger areas, including those with negative and positive effects. These can
				// be visualised using F5 during runtime.
				AddTrigger(PxVec2(0.f, -1.15f), 0.f, 1.1f, FilterGroup::KILLZONE);
				AddTrigger(PxVec2(.65f, .135f), .2f, .5f, FilterGroup::SCOREZONE);
				AddTrigger(PxVec2(.65f, .81f), -.2f, .5f, FilterGroup::SCOREZONE);
			}

			Flipper* AddFlipper(const PxTransform& transform, float initDrive, const char* material = "wood", PxVec3 color = LColor::Get().Fetch("soft-purple"))
			{
				// Initialize a flipper object with some default values and return the result.
				Flipper* f = new Flipper(this, transform, .8f, initDrive, -PxPi / 4.f, PxPi / 4.f);
				f->SetMaterial(MaterialLibrary::Instance().Get(material));
				f->SetColor(color);

				return f;
			}

			Hitpoint* AddHitpoint(PxVec2 placement, PxReal rotation, PxVec2 scale, PxVec3 color = LColor::Get().Fetch("soft-orange"))
			{
				// Initialize a hitpoint object with some default values and return the result. The filter group of
				// HITPOINT is applied to each object added this way which interacts and reports contact with PLAYER layers.
				Hitpoint* hp = new Hitpoint(this, Mathv::Multiply(platform->RelativeTransform(placement, -.15f), PxQuat(rotation, PxVec3(0, 1, 0))), scale, .1f);
				hp->SetMaterial(MaterialLibrary::Instance().New("glass", 0.475f, 0.f, .69f));
				hp->Get()->SetupFiltering(FilterGroup::HITPOINT, FilterGroup::PLAYER);
				hp->Get()->Color(color);

				return hp;
			}

			void AddWall(PxVec2 placement, PxReal rotation, float scale = 1.f, int divisions = 0, float bendFactor = 0.f, float height = .25f, float thickness = .05f)
			{
				// Initialize a curved wall object with some default values. This implementation caters to both curved and 
				// flat walls by simply changing the bend factor of the wall.
				CurvedWall* cw = new CurvedWall(Mathv::Multiply(platform->RelativeTransform(placement), PxQuat(rotation, PxVec3(0, 0, 1))), scale, divisions, bendFactor, height, thickness);
				cw->SetMaterial(MaterialLibrary::Instance().Get("wood"));
				Add(cw);
			}

			void AddTrigger(PxVec2 placement, PxReal rotation, PxReal scale, int filterGroup)
			{
				// Initialize a trigger zone, making use of the Mathv library to perform complex operations in a neat fashion.
				TriggerZone* t = new TriggerZone(Mathv::Multiply(platform->RelativeTransform(placement), PxQuat(rotation, PxVec3(0, 0, 1))), PxVec3(scale), filterGroup);

				// Determine the colour of the trigger based on the filterGroup provided, this is only visible when trigger
				// visualisation is active (F5).
				if (filterGroup == FilterGroup::KILLZONE)
					t->Color(LColor::Get().Fetch("soft-red"));
				else if (filterGroup == FilterGroup::SCOREZONE)
					t->Color(LColor::Get().Fetch("soft-green"));

				// Add the trigger zone to the scene and push it back to the list of triggers.
				Add(t);
				triggers.push_back(t);
			}

			void ToggleTriggersVisible()
			{
				// Toggle the visibility for all trigger zones in the scene.
				for (int i = 0; i < triggers.size(); i++)
					triggers[i]->ToggleVisible();
			}
	};
}

#endif