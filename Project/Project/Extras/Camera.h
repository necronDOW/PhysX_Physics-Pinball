#ifndef camera_h
#define camera_h

#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"

namespace VisualDebugger
{
	using namespace physx;

	class Camera
	{
	private:
		PxVec3	eye, eye_init;
		PxVec3	dir, dir_init;
		PxReal speed, speed_init;

	public:
		Camera(const PxVec3& _eye, const PxVec3& _dir, const PxReal _speed);

		void Reset();

		void Motion(int dx, int dy, PxReal delta_time);
		void AnalogMove(float x, float y);

		void MoveForward(PxReal delta_time);
		void MoveBackward(PxReal delta_time);
		void MoveLeft(PxReal delta_time);
		void MoveRight(PxReal delta_time);
		void MoveUp(PxReal delta_time);
		void MoveDown(PxReal delta_time);

		PxVec3 getEye() const;
		PxVec3 getDir() const;
		PxTransform	getTransform() const;
	};
}

#endif