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

	/// SINGLETON
	/// Private singleton members, this hides the constructor, destructor and copy constructor.
	private:
		static MaterialLibrary* _instance;
		MaterialLibrary() {}
		MaterialLibrary(const MaterialLibrary* o) { }
		~MaterialLibrary();
	/// SINGLETON

	vector<Material*> _materials;

	/// SINGLETON
	/// Public accessor for getting a singleton instance, this creates an instance if none already exists and
	/// stores it as _instance, before returning this.
	public:
		static MaterialLibrary& Instance()
		{
			if (_instance == nullptr)
				_instance = new MaterialLibrary();
			return *_instance;
		}
	/// SINGLETON

	public:
		PxMaterial* New(string id, float sf = .0f, float df = .0f, float cr = .0f);
		PxMaterial* New(string id, int& outIndex, float sf = .0f, float df = .0f, float cr = .0f);

		PxMaterial* Get(string id);
		PxMaterial* Get(int index);
};

#endif