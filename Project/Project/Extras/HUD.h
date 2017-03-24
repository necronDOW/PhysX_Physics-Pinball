#ifndef hud_h
#define hud_h

#include <string>
#include <list>
#include <vector>

#include "PxPhysicsAPI.h"
#include "Renderer.h"

namespace VisualDebugger
{
	using namespace std;
	using namespace physx;

	enum HUDState
	{
		EMPTY = 0,
		SCORE = 1,
		HELP = 2,
		PAUSE = 3
	};

	class HUDScreen
	{
		protected:
			vector<string> content;

		public:
			int id;
			PxReal font_size;
			PxVec3 color;

			HUDScreen(int screen_id, const PxVec3& _color=PxVec3(1.f,1.f,1.f), const PxReal& _font_size=0.024f) :
				id(screen_id), color(_color), font_size(_font_size)
			{
			}

			virtual int AddLine(string line)
			{
				content.push_back(line);
				return content.size() - 1;
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
	class HUDScreen_Extended : public HUDScreen
	{
		private:
			struct ValueField
			{
				int lineNumber;
				int charIndex;
				string originalStr;

				ValueField(int _lineNumber, int _charIndex, string _originalStr)
					: lineNumber(_lineNumber), charIndex(_charIndex), originalStr(_originalStr) { }
			};

			std::vector<ValueField> fields;

			void EditField(int lineNumber, string input)
			{
				for (int i = 0; i < fields.size(); i++)
				{
					if (lineNumber == fields[i].lineNumber)
					{
						string oStr = fields[i].originalStr;
						int cI = fields[i].charIndex;

						string front = oStr.substr(0, cI);
						string end = oStr.substr(cI + 1, oStr.length() - cI);

						content[lineNumber] = front + input + end;
					}
				}
			}

		public:
			HUDScreen_Extended(int screen_id, const PxVec3& _color = PxVec3(1.f, 1.f, 1.f), const PxReal& _font_size = 0.024f)
				: HUDScreen(screen_id, _color, _font_size)
			{

			}

			int AddLine(string line) override
			{
				for (int i = 0; i < line.length(); i++)
				{
					if (line[i] == '$')
					{
						fields.push_back(ValueField(content.size(), i, line));
						line[i] = '0';
					}
				}

				return HUDScreen::AddLine(line);
			}

			void EditField(int lineNumber, int value) { EditField(lineNumber, to_string(value)); }
			void EditField(int lineNumber, float value) { EditField(lineNumber, to_string(value)); }
	};

	class HUD
	{
		private:
			int active_screen;
			vector<HUDScreen*> screens;

		public:
			~HUD()
			{
				for (unsigned int i = 0; i < screens.size(); i++)
					delete screens[i];
			}

			void AddLine(int screen_id, string line, bool smartScreen = false)
			{
				int screenIndex = FindScreen(screen_id);
				if (screenIndex != -1)
				{
					screens[screenIndex]->AddLine(line);
					return;
				}

				if (smartScreen)
					screens.push_back(new HUDScreen_Extended(screen_id));
				else screens.push_back(new HUDScreen(screen_id));

				screens.back()->AddLine(line);
			}

			void EditLine(int screen_id, int lineNumber, int value)
			{
				if (typeid(*screens[screen_id]) == typeid(HUDScreen_Extended));
					((HUDScreen_Extended*)screens[screen_id])->EditField(lineNumber, value);
			}

			int FindScreen(int screen_id)
			{
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->id == screen_id)
						return i;
				}

				return -1;
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
					int screenIndex = FindScreen(screen_id);
					if (screenIndex != -1)
					{
						screens[screenIndex]->Clear();
						return;
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
					int screenIndex = FindScreen(screen_id);
					if (screenIndex != -1)
					{
						screens[screenIndex]->font_size = font_size;
						return;
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