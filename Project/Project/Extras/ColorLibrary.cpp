#include "ColorLibrary.h"

LColor* LColor::_instance = nullptr;

const PxVec3 LColor::Fetch(const char* id, float r, float g, float b)
{
	// Fetch a colour based on a provided string ID or create a new colour with the provided ID.
	if (id == "")
	{
		std::cerr << "Color (rgb=" << r << "," << g << "," << b << ") fetch failed, invalid ID!" << std::endl;
		return PxVec3(0.f);
	}

	int size = _colors.size();
	for (int i = 0; i < size; i++)
	{
		if (_colors[i]->id == id)
			return _colors[i]->color;
	}

	std::cout << "Color (id=" << id << ", rgb=" << r << "," << g << "," << b << ") added." << std::endl;

	// This allows exact colours to be stored and re-used.
	_colors.push_back(new ColorData(id, PxVec3(r, g, b)));
	return _colors[size - 1]->color;
}