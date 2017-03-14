#ifndef hud_h
#define hud_h

#include "Renderer.h"
#include <string>
#include <list>

namespace VisualDebugger
{
	using namespace std;

	class HUDScreen
	{
		vector<string> content;

	public:
		int id;
		PxReal font_size;
		PxVec3 color;

		HUDScreen(int screen_id, const PxVec3& _color=PxVec3(1.f,1.f,1.f), const PxReal& _font_size=0.024f) :
			id(screen_id), color(_color), font_size(_font_size)
		{
		}

		void AddLine(string line)
		{
			content.push_back(line);
		}

		void Render()
		{
			for (unsigned int i = 0; i < content.size(); i++)
				Renderer::RenderText(content[i], PxVec2(0.0, 1.f-(i+1)*font_size), color, font_size);
		}

		void Clear()
		{
			content.clear();
		}
	};

	class HUD
	{
		int active_screen;
		vector<HUDScreen*> screens;

	public:
		~HUD()
		{
			for (unsigned int i = 0; i < screens.size(); i++)
				delete screens[i];
		}

		void AddLine(int screen_id, string line)
		{
			for (unsigned int i = 0; i < screens.size(); i++)
			{
				if (screens[i]->id == screen_id)
				{
					screens[i]->AddLine(line);
					return;
				}
			}

			screens.push_back(new HUDScreen(screen_id));
			screens.back()->AddLine(line);
		}

		void ActiveScreen(int value)
		{
			active_screen = value;
		}

		int ActiveScreen()
		{
			return active_screen;		
		}

		void Clear(int screen_id=-1)
		{
			if (screen_id == -1)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->Clear();
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->Clear();
						return;
					}
				}
			}
		}

		void FontSize(PxReal font_size, unsigned int screen_id=-1)
		{
			if (screen_id == -1)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->font_size = font_size;
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->font_size = font_size;
						return;
					}
				}
			}
		}

		void Color(PxVec3 color, unsigned int screen_id=-1)
		{
			if (screen_id == -1)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					screens[i]->color = color;
				}
			}
			else
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
					{
						screens[i]->color = color;
						return;
					}
				}
			}
		}

		void Render()
		{
			for (unsigned int i = 0; i < screens.size(); i++)
			{
				if (screens[i]->id == active_screen)
				{
					screens[i]->Render();
					return;
				}
			}
		}
	};
}

#endif