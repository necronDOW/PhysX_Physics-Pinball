#ifndef userdata_h
#define userdata_h

#include "PxPhysicsAPI.h"

class UserData
{
	public:
		physx::PxVec3* color;
		physx::PxClothMeshDesc* cloth_mesh_desc;

		UserData(physx::PxVec3* _color=0, physx::PxClothMeshDesc* _cloth_mesh_desc=0) :
			color(_color), cloth_mesh_desc(_cloth_mesh_desc) {}
};

#endif