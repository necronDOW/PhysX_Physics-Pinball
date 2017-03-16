#ifndef complexactors_h
#define complexactors_h

#include "../PhysicsEngine.h"
#include "../Extras//Helper.h"

namespace PhysicsEngine
{
	class ConvexMesh : public DynamicActor
	{
		public:
			ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose = PxTransform(PxIdentity), PxReal density = 1.f)
				: DynamicActor(pose)
			{
				CreateShape(PxConvexMeshGeometry(CookMesh(verts)), density);
			}

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

			static PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
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
		protected:
			int cornerSize;
			PxVec3* cornerData;
			PxVec3 _center;

		public:
			Polygon(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1))
				: StaticActor(pose)
			{
				cornerSize = edgeCount;
				cornerData = new PxVec3[cornerSize];

				PxVec3 radiusVec = PxVec3(0.f, 1.f, 0.f);
				float angleVarience = (PxPi * 2.f) / (float)edgeCount;
				float angle = (edgeCount % 2 != 0) ? angleVarience : angleVarience / 2.f;

				for (int i = 0; i < edgeCount; i++)
				{
					cornerData[i] = Mathv::Rotate(radiusVec, angle, PxVec3(1, 0, 0)).multiply(scale);
					angle += angleVarience;
				}

				for (int i = 0; i < edgeCount; i++)
				{
					PxVec3 end = (i < edgeCount - 1) ? cornerData[i + 1] : cornerData[0];
					PxVec3 len = end - cornerData[i];
					float edgeLength = len.magnitude() / 2.f;

					CreateShape(PxBoxGeometry(PxVec3(edgeLength, thickness, thickness * scale.z)), 1.f);
					GetShape(i)->setLocalPose(PxTransform(cornerData[i] + (len / 2.f), PxQuat(atan2(len.y, len.x), PxVec3(0, 0, 1))));
				}

				_center = PxVec3(0);
				for (int i = 0; i < cornerSize; i++)
					_center += cornerData[i];
				_center = pose.p + (_center / cornerSize);
			}

			void Materials(PxMaterial* material)
			{
				int shapeCount = GetShapes().size();

				for (int i = 0; i < shapeCount; i++)
					Material(material, i);
			}

			PxVec3 center() { return _center; }

		private:
			float Edge(float radius, float angleVarience)
			{
				PxVec3 start = PxVec3(0.f, radius, 0.f);
				PxVec3 next = Mathv::Rotate(start, angleVarience, PxVec3(1, 0, 0));

				return (next - start).magnitude();
			}
	};

	class CappedPolygon : public Polygon
	{
		public:
			enum CapMode
			{
				None,
				Opaque,
				Transparent
			};

			CappedPolygon(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1), CapMode top = None, CapMode bottom = None)
				: Polygon(pose, edgeCount, thickness, scale)
			{
				Cap(top, thickness * scale.z + thickness, edgeCount, thickness);
				Cap(bottom, -thickness * scale.z - thickness, edgeCount + 1, thickness);
			}

		private:
			void Cap(CapMode mode, float yOffset, int shapeIndex, float thickness)
			{
				if (mode == None)
					return;

				PxU32 vCount = cornerSize * 2 + 2;
				PxVec3* v = new PxVec3[vCount];
				int midIndex = vCount / 2;

				v[0] = PxVec3(0.f, 0.f, yOffset + thickness);
				v[midIndex] = PxVec3(0.f, 0.f, yOffset - thickness);

				for (int i = 1; i < midIndex; i++)
				{
					v[i] = cornerData[i-1] + v[0];
					v[midIndex+i] = cornerData[i-1] + v[midIndex];
				}

				CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(vector<PxVec3>(&v[0], &v[vCount-1]))), 1.f);

				if (mode == Transparent)
					GetShape(shapeIndex)->setFlag(PxShapeFlag::eVISUALIZATION, false);
			}
	};

	class Platform : public CappedPolygon
	{
		protected:
			PxVec3 _halfExtents;
			PxTransform _transform;

		public:
			Platform(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1))
				: CappedPolygon(pose, edgeCount, thickness, scale, CapMode::Transparent, CapMode::Opaque)
			{
				_transform = pose;

				PxReal rad = pose.q.getAngle();
				_halfExtents = Mathv::Rotate(cornerData[0].abs(), rad, PxVec3(0, 0, 1));
			}
			
			PxTransform RelativeTransform(PxVec2 offset)
			{
				PxVec3 localPosition = _halfExtents.multiply(PxVec3(offset.x, offset.y, -offset.y));
				return PxTransform(_transform.p + localPosition);
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