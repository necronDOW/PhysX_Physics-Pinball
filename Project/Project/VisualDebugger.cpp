#include "VisualDebugger.h"

namespace VisualDebugger
{
	Camera* camera;
	PhysicsEngine::MyScene* scene;
	PxReal delta_time = 1.f / 60.f;
	PxReal gForceStrength = 20;
	RenderMode render_mode = NORMAL;
	const int MAX_KEYS = 256;
	bool key_state[MAX_KEYS];
	bool hud_show = true;
	HUD hud;

	void Init(const char *window_name, int width, int height)
	{
		PhysicsEngine::PxInit();
		scene = new PhysicsEngine::MyScene();
		scene->Init();

		Renderer::BackgroundColor(PxVec3(150.f/255.f,150.f/255.f,150.f/255.f));
		Renderer::SetRenderDetail(40);
		Renderer::InitWindow(window_name, width, height);
		Renderer::Init();

		camera = new Camera(PxVec3(0.0f, 5.0f, 15.0f), PxVec3(0.f,-.1f,-1.f), 5.f);

		HUDInit();

		glutDisplayFunc(RenderScene);
		atexit(exitCallback);

		glutKeyboardFunc(KeyPress);
		glutSpecialFunc(KeySpecial);
		glutKeyboardUpFunc(KeyRelease);
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);
		motionCallback(0,0);
	}

	void HUDInit()
	{
		hud.AddLine(EMPTY, "");
		AddHUD(HELP, "../Assets/help_hud.txt");
		AddHUD(PAUSE, "../Assets/paused_hud.txt");

		hud.FontSize(0.018f);
		hud.Color(PxVec3(0.f,0.f,0.f));
	}

	void Start()
	{ 
		glutMainLoop(); 
	}

	void RenderScene()
	{
		KeyHold();

		Renderer::Start(camera->getEye(), camera->getDir());

		if ((render_mode == DEBUG) || (render_mode == BOTH))
			Renderer::Render(scene->Get()->getRenderBuffer());

		if ((render_mode == NORMAL) || (render_mode == BOTH))
		{
			std::vector<PxActor*> actors = scene->GetAllActors();
			if (actors.size())
				Renderer::Render(&actors[0], (PxU32)actors.size());
		}

		if (hud_show)
		{
			if (scene->Pause())
				hud.ActiveScreen(PAUSE);
			else
				hud.ActiveScreen(HELP);
		}
		else hud.ActiveScreen(EMPTY);

		hud.Render();

		Renderer::Finish();

		scene->Update(delta_time);
	}

	void CameraInput(int key)
	{
		switch (toupper(key))
		{
			case 'W': camera->MoveForward(delta_time);
				break;
			case 'S': camera->MoveBackward(delta_time);
				break;
			case 'A': camera->MoveLeft(delta_time);
				break;
			case 'D': camera->MoveRight(delta_time);
				break;
			case 'Q': camera->MoveUp(delta_time);
				break;
			case 'Z': camera->MoveDown(delta_time);
				break;
			default:
				break;
		}
	}

	void ForceInput(int key)
	{
		if (!scene->GetSelectedActor())
			return;

		switch (toupper(key))
		{
			case 'I': scene->GetSelectedActor()->addForce(PxVec3(0,0,-1)*gForceStrength);
				break;
			case 'K': scene->GetSelectedActor()->addForce(PxVec3(0,0,1)*gForceStrength);
				break;
			case 'J': scene->GetSelectedActor()->addForce(PxVec3(-1,0,0)*gForceStrength);
				break;
			case 'L': scene->GetSelectedActor()->addForce(PxVec3(1,0,0)*gForceStrength);
				break;
			case 'U': scene->GetSelectedActor()->addForce(PxVec3(0,1,0)*gForceStrength);
				break;
			case 'M': scene->GetSelectedActor()->addForce(PxVec3(0,-1,0)*gForceStrength);
				break;
			default:
				break;
		}
	}

	void KeyPress(unsigned char key, int x, int y)
	{
		if (key_state[key] == true)
			return;

		key_state[key] = true;

		if (key == 27)
			exit(0);

		switch (toupper(key))
		{
			case 'R':
				scene->Hit();
				break;
			default:
				break;
		}
	}

	void KeyHold()
	{
		for (int i = 0; i < MAX_KEYS; i++)
		{
			if (key_state[i])
			{
				CameraInput(i);
				ForceInput(i);
			}
		}
	}

	void KeySpecial(int key, int x, int y)
	{
		switch (key)
		{
		case GLUT_KEY_F5: hud_show = !hud_show;
			break;
		case GLUT_KEY_F6: Renderer::ShowShadows(!Renderer::ShowShadows());
			break;
		case GLUT_KEY_F7: ToggleRenderMode();
			break;
		case GLUT_KEY_F8: camera->Reset();
			break;

		case GLUT_KEY_F9: scene->SelectNextActor();
			break;
		case GLUT_KEY_F10: scene->Pause(!scene->Pause());
			break;
		case GLUT_KEY_F12: scene->Reset();
			break;
		default:
			break;
		}
	}

	void KeyRelease(unsigned char key, int x, int y)
	{
		key_state[key] = false;

		switch (toupper(key))
		{
			case 'R':
				scene->Hit();
				break;
			default:
				break;
		}
	}

	int mMouseX = 0;
	int mMouseY = 0;
	void motionCallback(int x, int y)
	{
		int dx = mMouseX - x;
		int dy = mMouseY - y;

		camera->Motion(dx, dy, delta_time);

		mMouseX = x;
		mMouseY = y;
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		mMouseX = x;
		mMouseY = y;
	}

	void exitCallback(void)
	{
		delete camera;
		delete scene;
		PhysicsEngine::PxRelease();
	}

	void ToggleRenderMode()
	{
		if (render_mode == NORMAL)
			render_mode = DEBUG;
		else if (render_mode == DEBUG)
			render_mode = BOTH;
		else if (render_mode == BOTH)
			render_mode = NORMAL;
	}

	void AddHUD(int screen_id, std::string directory)
	{
		std::ifstream txt(directory);
		if (txt.is_open())
		{
			std::string buf;
			while (txt.good())
			{
				std::getline(txt, buf);
				hud.AddLine(screen_id, buf);
			}
			txt.close();
		}
	}
}