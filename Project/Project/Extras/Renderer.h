#ifndef renderer_h
#define renderer_h

#include "PxPhysicsAPI.h"
#include "GLFontRenderer.h"
#include <GL/glut.h>
#include <string>

namespace VisualDebugger
{
	namespace Renderer
	{
		using namespace physx;

		void InitWindow(const char *name, int width, int height);

		void Init();

		void Start(const PxVec3& cameraEye, const PxVec3& cameraDir);

		void Render(PxActor** actors, const PxU32 numActors);

		void Render(const PxRenderBuffer& data, PxReal line_width=1.f);

		void RenderText(const std::string& text, const physx::PxVec2& location, 
			const PxVec3& color, PxReal size);

		void BackgroundColor(const PxVec3& background_color);

		void Finish();

		void SetRenderDetail(int value);

		void ShowShadows(bool value);

		bool ShowShadows();
	}
}

#endif