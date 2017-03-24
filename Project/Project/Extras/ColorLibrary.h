#ifndef colorlibrary_h
#define colorlibrary_h

#include <vector>
#include <iostream>
#include "PxPhysicsAPI.h"

using physx::PxVec3;

class LColor
{
	struct ColorData
	{
		const char* id;
		const PxVec3 color;

		ColorData(const char* _id, const PxVec3 _color)
			: id(_id), color(_color) { }
	};

	/// SINGLETON
	public:
		static LColor& Get()
		{
			if (_instance == nullptr)
				_instance = new LColor();
			return *_instance;
		}

	private:
		static LColor* _instance;
		
		LColor() { }
		LColor(const LColor& other) { }
		~LColor() { }
	/// SINGLETON

	public:
		const PxVec3 Fetch(const char* id, float r = 0.f, float g = 0.f, float b = 0.f);

	private:
		std::vector<ColorData*> _colors = { 
			new ColorData("white", PxVec3(1.f)), 
			new ColorData("gray-20", PxVec3(.2f)), 
			new ColorData("soft-red", PxVec3(1.f,.3f,.3f)),
			new ColorData("soft-green", PxVec3(.3f,1.f,.3f)),
			new ColorData("soft-blue", PxVec3(.3f,.3f,1.f)),
			new ColorData("soft-orange", PxVec3(1.f,.65f,.3f)),
			new ColorData("soft-purple", PxVec3(.65f,.3f,1.f))
		};
};

#endif