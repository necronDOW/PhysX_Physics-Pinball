#ifndef visualdebugger_h
#define visualdebugger_h

#include <vector>
#include <fstream>
#include "MyPhysicsEngine.h"
#include "Extras\Camera.h"
#include "Extras\Renderer.h"
#include "Extras\HUD.h"

namespace VisualDebugger
{
	using namespace physx;

	enum RenderMode
	{
		DEBUG,
		NORMAL,
		BOTH
	};

	enum HUDState
	{
		EMPTY = 0,
		HELP = 1,
		PAUSE = 2
	};

	void Init(const char *window_name, int width=512, int height=512);
	void HUDInit();

	void Start();
	void RenderScene();

	void KeyPress(unsigned char key, int x, int y);
	void KeyHold();
	void KeySpecial(int key, int x, int y);
	void KeyRelease(unsigned char key, int x, int y);

	void motionCallback(int x, int y);
	void mouseCallback(int button, int state, int x, int y);
	void exitCallback(void);

	void ToggleRenderMode();

	void AddHUD(int screen_id, std::string directory);
}

#endif