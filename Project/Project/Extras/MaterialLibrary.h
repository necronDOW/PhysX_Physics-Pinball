#ifndef materiallibrary_h
#define materiallibrary_h

#include <iostream>
#include <string>
#include <vector>
#include "../PhysicsEngine.h"

using std::string;
using std::vector;
using physx::PxMaterial;

class MaterialLibrary
{
	struct Material
	{
		private:
			string _id;
			PxMaterial* _material;

		public:
			Material(string id, float sf, float df, float cr)
			{
				_id = id;
				_material = PhysicsEngine::GetPhysics()->createMaterial(sf, df, cr);
			}

			string id() { return _id; }
			PxMaterial* material() { return _material; }
	};

	private:
		/// SINGLETON
		MaterialLibrary() {}
		MaterialLibrary(const MaterialLibrary* o) { }
		~MaterialLibrary();
		static MaterialLibrary* _instance;
		/// SINGLETON

		vector<Material*> _materials;

	public:
		/// SINGLETON
		static MaterialLibrary& Instance()
		{
			if (_instance == nullptr)
				_instance = new MaterialLibrary();
			return *_instance;
		}
		/// SINGLETON

		PxMaterial* New(string id, float sf = .0f, float df = .0f, float cr = .0f);
		PxMaterial* New(string id, int& outIndex, float sf = .0f, float df = .0f, float cr = .0f);

		PxMaterial* Get(string id);
		PxMaterial* Get(int index);
};

#endif