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
		SCORE = 1,	// Score screen
		HELP = 2,	// Modified help screen
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
			// The value field is a container for field accesses, this improves with efficiency when trying to modify a given field value
			// by storing the char and line index of the field, and a reference the the original string.
			struct ValueField
			{
				int lineNumber;
				int charIndex;
				string originalStr;

				ValueField(int _lineNumber, int _charIndex, string _originalStr)
					: lineNumber(_lineNumber), charIndex(_charIndex), originalStr(_originalStr) { }
			};

			// Storage for the modifiable fields within this HUD screen.
			std::vector<ValueField> fields;

			void EditField(int lineNumber, string input)
			{
				// Search through each modifiable field until a matching line index is found.
				for (int i = 0; i < fields.size(); i++)
				{
					if (lineNumber == fields[i].lineNumber)
					{
						// Extract the original string and char index from the chosen field.
						string oStr = fields[i].originalStr;
						int cI = fields[i].charIndex;

						// Use substr to seperate the front and end from the original string, relative to the char index.
						string front = oStr.substr(0, cI);
						string end = oStr.substr(cI + 1, oStr.length() - cI);

						// Insert the new field value between front and end, and pass the new string into the content buffer.
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
				// Override for add line which extends the original functionality to extract necessary information
				// for field generation. This identifies the '$' char from the input string and creates a field at its
				// given index.
				for (int i = 0; i < line.length(); i++)
				{
					if (line[i] == '$')
					{
						fields.push_back(ValueField(content.size(), i, line));
						line[i] = '0';
					}
				}

				// Perform base AddLine functionality and return the result.
				return HUDScreen::AddLine(line);
			}

			// Two meta-virtual functions for accepting integer and float values in the EditField method.
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

				// Check whether or not the screen selected should be a smart screen, this is only called when no
				// screen currently exists for add line execution. Smart screens (HUDScreen_Extended) are handled differently
				// in the update function.
				if (smartScreen)
					screens.push_back(new HUDScreen_Extended(screen_id));
				else screens.push_back(new HUDScreen(screen_id));

				screens.back()->AddLine(line);
			}

			void EditLine(int screen_id, int lineNumber, int value)
			{
				// HUD accessor function to allow easier interactivity with the HUDScreen_Extended::EditLine implementation.
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

			int NextScreen()
			{
				// This function simply cycles to the next screen, catching out-of-bounds errors by looping back 
				// around to index 0. This will also skip the PAUSE screen as this should only be displayed when
				// the simulation is paused.
				int next = (active_screen == screens.size()) ? 0 : active_screen + 1;

				if (next == PAUSE)
					return (++next == screens.size()) ? 0 : next;
				else return next;
			}
		};
}

#endif