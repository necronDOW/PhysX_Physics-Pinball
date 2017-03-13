#ifndef complexactors_h
#define complexactors_h

#include "../PhysicsEngine.h"

namespace PhysicsEngine
{
	class ConvexMesh : public DynamicActor
	{
		public:
			//constructor
			ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose = PxTransform(PxIdentity), PxReal density = 1.f)
				: DynamicActor(pose)
			{
				CreateShape(PxConvexMeshGeometry(CookMesh(verts)), density);
			}

			//mesh cooking (preparation)
			static PxConvexMesh* CookMesh(const std::vector<PxVec3>& verts, PxU16 vertexLimit = 256, PxConvexFlags flag = PxConvexFlag::eCOMPUTE_CONVEX)
			{
				PxConvexMeshDesc mesh_desc;
				mesh_desc.points.count = (PxU32)verts.size();
				mesh_desc.points.stride = sizeof(PxVec3);
				mesh_desc.points.data = &verts.front();
				mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
				mesh_desc.vertexLimit = 256;

				PxDefaultMemoryOutputStream stream;

				if (!GetCooking()->cookConvexMesh(mesh_desc, stream))
					throw new Exception("ConvexMesh::CookMesh, cooking failed.");

				PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

				return GetPhysics()->createConvexMesh(input);
			}
	};

	class TriangleMesh : public StaticActor
	{
		public:
			//constructor
			TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose = PxTransform(PxIdentity))
				: StaticActor(pose)
			{
				PxTriangleMeshDesc mesh_desc;
				mesh_desc.points.count = (PxU32)verts.size();
				mesh_desc.points.stride = sizeof(PxVec3);
				mesh_desc.points.data = &verts.front();
				mesh_desc.triangles.count = (PxU32)trigs.size();
				mesh_desc.triangles.stride = 3 * sizeof(PxU32);
				mesh_desc.triangles.data = &trigs.front();

				CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
			}

			//mesh cooking (preparation)
			PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
			{
				PxDefaultMemoryOutputStream stream;

				if (!GetCooking()->cookTriangleMesh(mesh_desc, stream))
					throw new Exception("TriangleMesh::CookMesh, cooking failed.");

				PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

				return GetPhysics()->createTriangleMesh(input);
			}
	};

	class Polygon : public StaticActor
	{
		public:
			Polygon(const PxTransform& pose = PxTransform(PxIdentity), float radius = 1.f, int edgeCount = 4, PxVec2 thickness = PxVec2(.1f, .1f), PxVec3 scalar = PxVec3(1), PxReal density = 1.f, bool flatBottom = true)
				: StaticActor(pose)
			{
				PxVec3 radiusVec = PxVec3(0.f, radius, 0.f);
				float angleVarience = (PxPi * 2.f) / (float)edgeCount;
				float angle = (edgeCount % 2 != 0 && flatBottom) ? angleVarience : angleVarience / 2.f;
				PxVec3* corners = new PxVec3[edgeCount];

				for (int i = 0; i < edgeCount; i++)
				{
					corners[i] = Rotate(radiusVec, angle).multiply(scalar);
					angle += angleVarience;
				}

				for (int i = 0; i < edgeCount; i++)
				{
					PxVec3 end = (i < edgeCount - 1) ? corners[i + 1] : corners[0];
					PxVec3 len = end - corners[i];
					float edgeLength = len.magnitude() / 2.f;

					CreateShape(PxBoxGeometry(PxVec3(edgeLength, thickness.y, thickness.x)), density);
					GetShape(i)->setLocalPose(PxTransform(corners[i] + (len / 2.f), PxQuat(atan2(len.y, len.x), PxVec3(0, 0, 1))));
				}
			}

			/// DEPRECATED : Non-scalar solution.
			/*Polygon(const PxTransform& pose = PxTransform(PxIdentity), float radius = 1.f, int edgeCount = 4, PxVec2 thickness = PxVec2(.1f, .1f), PxReal density = 1.f, bool flatBottom = true)
				: StaticActor(pose)
			{
				PxVec3 inradius = PxVec3(0.f, radius * cos(PxPi / (float)edgeCount), 0.f);
				float angleVarience = (PxPi * 2.f) / (float)edgeCount;
				float edgeLength = Edge(radius, angleVarience) / 2.f;
				float angle = (edgeCount % 2 != 0 && flatBottom) ? angleVarience / 2.f : 0.f;

				for (int i = 0; i < edgeCount; i++)
				{
					CreateShape(PxBoxGeometry(PxVec3(edgeLength, thickness.y, thickness.x)), density);
					GetShape(i)->setLocalPose(PxTransform(Rotate(inradius, angle), PxQuat(angle, PxVec3(0, 0, 1))));

					angle += angleVarience;
				}
			}*/

		private:
			PxVec3 Rotate(PxVec3 original, PxReal rad)
			{
				PxReal cO = cos(rad);
				PxReal sO = sin(rad);

				return PxVec3(cO*original.x - sO*original.y, sO*original.x + cO*original.y, original.z);
			}

			float Edge(float radius, float angleVarience)
			{
				PxVec3 start = PxVec3(0.f, radius, 0.f);
				PxVec3 next = Rotate(start, angleVarience);

				return (next - start).magnitude();
			}
	};

	class Wedge : public DynamicActor
	{
		public:
			Wedge(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f, PxVec3 scale = PxVec3(1.f))
				: DynamicActor(pose)
			{
				PxVec3 wedge_verts[6] = { PxVec3(.5f, 1, 0), PxVec3(.5f, 0, .5f), PxVec3(.5f, 0, -.5f), PxVec3(-.5f, 1, 0), PxVec3(-.5f, 0, .5f), PxVec3(-.5f, 0, -.5f) };

				if (scale != PxVec3(1.f))
				{
					for (int i = 0; i < 6; i++)
						wedge_verts[i] = wedge_verts[i].multiply(scale);
				}

				CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(vector<PxVec3>(&wedge_verts[0], &wedge_verts[6]))), density);
			}
	};
}

#endif