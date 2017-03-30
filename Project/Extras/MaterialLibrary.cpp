#include "MaterialLibrary.h"

MaterialLibrary* MaterialLibrary::_instance = nullptr;

MaterialLibrary::~MaterialLibrary()
{
	// Delete all materials stored in memory.
	for (int i = 0; i < _materials.size(); i++)
		delete _materials[i];

	delete _instance;
}

PxMaterial* MaterialLibrary::New(string id, float sf, float df, float cr)
{
	// Add a new material with the provided values, this is indexed within the materials array with an index of its id.
	if (id == "")
	{
		std::cerr << "Material (sf=" << sf << ", df=" << df << ", cr=" << cr << ") creation failed, invalid ID!" << std::endl;
		return nullptr;
	}

	PxMaterial* check = Get(id);
	if (check)
		return check;

	std::cout << "Material (id=" << id << ", sf=" << sf << ", df=" << df << ", cr=" << cr << ") added." << std::endl;

	_materials.push_back(new Material(id, sf, df, cr));
	return _materials.back()->material();
}

PxMaterial* MaterialLibrary::New(string id, int& outIndex, float sf, float df, float cr)
{
	// Retrieve a material using integer indexing instead of strings, this is faster.
	PxMaterial* retrieved = New(id, sf, df, cr);

	if (retrieved)
		outIndex = _materials.size() - 1;

	return retrieved;
}

PxMaterial* MaterialLibrary::Get(string id)
{
	// Get a material if it exists within the material library.
	for (int i = 0; i < _materials.size(); i++)
	{
		if (_materials[i]->id() == id)
			return _materials[i]->material();
	}

	std::cerr << "Material not found (id=" << id << ")." << std::endl;
	return nullptr;
}

PxMaterial* MaterialLibrary::Get(int index)
{
	// Get a material at index within the array, if the index is valid. This is faster than string comparison.
	if (index >= 0 && index < _materials.size())
		return _materials[index]->material();

	std::cerr << "Material not found (index=" << index << ")." << std::endl;
	return nullptr;
}