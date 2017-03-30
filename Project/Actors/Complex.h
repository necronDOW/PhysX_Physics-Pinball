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
				// Create a convex mesh using the vertex information provided.
				CreateShape(PxConvexMeshGeometry(CookMesh(verts)), density);
			}

			static PxConvexMesh* CookMesh(const std::vector<PxVec3>& verts, PxU16 vertexLimit = 256, PxConvexFlags flag = PxConvexFlag::eCOMPUTE_CONVEX)
			{
				// Compile a mesh description object using the data provided. This creates the necessary parameters for 'cooking' a convex mesh.
				PxConvexMeshDesc mesh_desc;
				mesh_desc.points.count = (PxU32)verts.size();
				mesh_desc.points.stride = sizeof(PxVec3);
				mesh_desc.points.data = &verts.front();
				mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
				mesh_desc.vertexLimit = 256;

				// Stream the mesh description into a memory output stream. This allows the translation of meshdesc to data which is used to create the convex mesh.
				PxDefaultMemoryOutputStream stream;

				if (!GetCooking()->cookConvexMesh(mesh_desc, stream))
					throw new Exception("ConvexMesh::CookMesh, cooking failed.");

				PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

				// Return the result of creating the mesh, in the form of a PxConvexMesh.
				return GetPhysics()->createConvexMesh(input);
			}
	};

	class TriangleMesh : public StaticActor
	{
		public:
			TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose = PxTransform(PxIdentity))
				: StaticActor(pose)
			{
				// Compile a mesh description object using the data provided. This creates the necessary parameters to create a 'triangle' mesh which differs
				// from convex by allowing for concaved surfaces.
				PxTriangleMeshDesc mesh_desc;
				mesh_desc.points.count = (PxU32)verts.size();
				mesh_desc.points.stride = sizeof(PxVec3);
				mesh_desc.points.data = &verts.front();
				mesh_desc.triangles.count = (PxU32)trigs.size();
				mesh_desc.triangles.stride = 3 * sizeof(PxU32);
				mesh_desc.triangles.data = &trigs.front();

				// Create the shape a triangle geometry.
				CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
			}

			static PxTriangleMesh* CookMesh(const PxTriangleMeshDesc& mesh_desc)
			{
				// Stream the provided mesh description into a memory steam. By doing this, the meshdesc is transformed into data which can be directly applied within PxPhysics.
				PxDefaultMemoryOutputStream stream;

				if (!GetCooking()->cookTriangleMesh(mesh_desc, stream))
					throw new Exception("TriangleMesh::CookMesh, cooking failed.");

				PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

				// Return the result of creating the mesh, in the form of a PxTriangleMesh.
				return GetPhysics()->createTriangleMesh(input);
			}
	};

	class CurvedWall : public StaticActor
	{
		public:
			CurvedWall(const PxTransform& pose = PxTransform(PxIdentity), float scale = 1.f, int divisions = 0, float bendFactor = .1f, float height = .1f, float thickness = .05f)
				: StaticActor(pose)
			{
				// The initial size of the walls is 2 ^ divisions + 1, resulting in a recursive split for N walls. e.g. N=1, N=3, N=5, N=9 ...
				int size = pow(2, divisions) + 1;

				// Create a vector to store each 'connecting edge' of the walls, populated with the first top-most and bottom-most elements.
				PxVec3* points = new PxVec3[size];
				points[0] = PxVec3(-1, 0, 0) * scale;
				points[size-1] = PxVec3(1, 0, 0) * scale;

				// Execute the split recursive function until the wall has been divided enough times.
				Split(points, 0, size - 1, divisions, PxClamp(bendFactor, .0f, .4f));

				// Create all of the box geometry using the created points vector.
				for (int i = 0; i < size - 1; i++)
				{
					// The length is non-deterministic due to the bend factor variable, meaning that each section can vary in length.
					PxVec3 len = points[i + 1] - points[i];
					PxVec3 mid = len / 2.f;

					CreateShape(PxBoxGeometry(PxVec3(mid.magnitude(), thickness, height)), 1.f);
					GetShape(i)->setLocalPose(PxTransform(points[i] + mid, PxQuat(atan2(len.y, len.x), PxVec3(0, 0, 1))));
				}
			}

			void SetMaterial(PxMaterial* mat)
			{
				// Set the material of all shapes within this object to be equal to the provided physics material.
				// This sets parameters such as friction and resistution for the overall wall.
				std::vector<PxShape*> shapes = GetShapes();
				for (int i = 0; i < shapes.size(); i++)
					Material(mat, i);
			}
		
		private:
			void Split(PxVec3*& arr, int indexA, int indexB, int count, float bendFactor)
			{
				// This recursive function enables the wall to be split exponentially and quickly using a basic divide and conquer-like algorithm.
				if (count != 0)
				{
					PxVec3 len = arr[indexB] - arr[indexA];
					PxVec3 mid = arr[indexA] + (len / 2.f);

					int indexNP = (indexA + indexB) / 2;
					arr[indexNP] = mid + len.cross(PxVec3(0, 0, 1)) * bendFactor;

					// Divide the bend factor by 2 to ensure for a coherent and smooth transition across the wall.
					bendFactor /= 2.f;
					Split(arr, indexA, indexNP, --count, bendFactor);
					Split(arr, indexNP, indexB, count, bendFactor);
				}
			}
	};

	class Polygon : public StaticActor
	{
		protected:
			PxTransform _transform;
			std::vector<PxVec3> _corners;

		public:
			Polygon(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1))
				: StaticActor(pose)
			{
				// Set the _transform member variable to the provided pose.
				_transform = pose;

				// Set the initial angle varience and angle which will be used when placing connecting edges in 3D space.
				float angleVarience = (PxPi * 2.f) / (float)edgeCount;
				float angle = (edgeCount % 2 != 0) ? angleVarience : angleVarience / 2.f;

				// This call will provide a series of plotted values around a set point, each rotating beyond the last by angleVarience.
				_corners = Mathv::Plot(PxVec3(0.f, 1.f, 0.f), angle, angleVarience, scale);

				// Create the 3D geometry connecting the plotted points. Thie creates a 'hollow' structure whereby the walls of the polygon are of a given thickness.
				for (int i = 0; i < edgeCount; i++)
				{
					PxVec3 end = (i < edgeCount - 1) ? _corners[i + 1] : _corners[0];
					PxVec3 len = end - _corners[i];
					float edgeLength = len.magnitude() / 2.f;

					CreateShape(PxBoxGeometry(PxVec3(edgeLength, thickness, thickness * scale.z)), 1.f);
					GetShape(i)->setLocalPose(PxTransform(_corners[i] + (len / 2.f), PxQuat(atan2(len.y, len.x), PxVec3(0, 0, 1))));
				}
			}

			void Materials(PxMaterial* material)
			{
				// Loops through each shape in this object and apply the physics material provided.
				int shapeCount = GetShapes().size();
				for (int i = 0; i < shapeCount; i++)
					Material(material, i);
			}

			void SetColor(PxVec3 rgb)
			{
				// Set the colour of all walls within this 3D geometry.
				for (int i = 0; i < _corners.size(); i++)
					Color(rgb, i);
			}

		private:
			float Edge(float radius, float angleVarience)
			{
				// Used to calculate the length of each wall, this call is only necessary once per geometry.
				PxVec3 start = PxVec3(0.f, radius, 0.f);
				PxVec3 next = Mathv::Rotate(start, angleVarience, PxVec3(1, 0, 0));

				return (next - start).magnitude();
			}
	};

	class CappedPolygon : public Polygon
	{
		public:
			// Enumerator illustrating the capmode of the CappedPolygon:
			//    # None = The platform will not be created.
			//    # Opaque = The platform will be created and visible.
			//    # Transparent = The platform will be created but not shown.
			enum CapMode
			{
				None,
				Opaque,
				Transparent
			};

			CappedPolygon(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1), CapMode top = None, CapMode bottom = None)
				: Polygon(pose, edgeCount, thickness, scale)
			{
				// Add a lower and upper cap using the provided function to set the y offset accordingly.
				Cap(top, thickness * scale.z + thickness, edgeCount, thickness);
				Cap(bottom, -thickness * scale.z - thickness, edgeCount + 1, thickness);
			}

			void SetColor(PxVec3 wallsRGB, PxVec3 platRGB)
			{
				// Make a call to the base set colour method to apply the necessary wall colours.
				Polygon::SetColor(wallsRGB);

				// Set the colour of all cap-based geometry within this object.
				for (int i = 0; i < 2; i++)
					Color(platRGB, _corners.size() + i);
			}

		private:
			void Cap(CapMode mode, float yOffset, int shapeIndex, float thickness)
			{
				// If the capmode is none, simply return and execute no further.
				if (mode == None)
					return;

				// Set initial parameters for storing vector information and also provide starting values for top-most-0 and bottom-most-0.
				PxU32 vCount = _corners.size() * 2 + 2;
				PxVec3* v = new PxVec3[vCount];
				int midIndex = vCount / 2;

				v[0] = PxVec3(0.f, 0.f, yOffset + thickness);
				v[midIndex] = PxVec3(0.f, 0.f, yOffset - thickness);

				for (int i = 1; i < midIndex; i++)
				{
					// Use the previously created corner data to approximate new vertices for the cap geometry.
					v[i] = _corners[i-1] + v[0];
					v[midIndex+i] = _corners[i-1] + v[midIndex];
				}

				// Create a new mesh geometry using the calculated vertex data, this will form one of the two caps that make up the CappedPolygon.
				CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(vector<PxVec3>(&v[0], &v[vCount-1]))), 1.f);

				// If the mode is transparent, then set the visualization for the new  cap to false.
				if (mode == Transparent)
					GetShape(shapeIndex)->setFlag(PxShapeFlag::eVISUALIZATION, false);
			}
	};

	class Platform : public CappedPolygon
	{
		// This class extends CappedPolgygon to provide easy placement and alignment of objects with respect to the platform. This is especially useful due to the significant
		// number of entities within a pinball board or "platform".

		protected:
			PxVec3 _halfExtents;

		public:
			Platform(const PxTransform& pose = PxTransform(PxIdentity), int edgeCount = 4, float thickness = .1f, PxVec3 scale = PxVec3(1))
				: CappedPolygon(pose, edgeCount, thickness, scale, CapMode::Transparent, CapMode::Opaque)
			{
				// The half extents of this polygon.
				_halfExtents = _corners[0].abs();
			}

			PxTransform RelativeTransform(PxVec2 offset, float yOffset = 0.f)
			{
				// Calculate a position relative to the platform object, when provided with x and y coordinates.
				PxVec3 halfExtents2D = PxVec3(_halfExtents.x, _halfExtents.y, 1.f);
				PxVec3 offsetV3 = PxVec3(offset.x, offset.y, yOffset);

				return PxTransform(_transform.p + Mathv::Multiply(_transform.q, offsetV3.multiply(halfExtents2D)), _transform.q);
			}
	};

	class Wedge : public DynamicActor
	{
		public:
			Wedge(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f, PxVec3 scale = PxVec3(1.f))
				: DynamicActor(pose)
			{
				// Initialize a set number of vertices which form the general shape of wedge-like geometry.
				PxVec3 wedge_verts[6] = { PxVec3(.5f, 1, 0), PxVec3(.5f, 0, .5f), PxVec3(.5f, 0, -.5f), PxVec3(-.5f, 1, 0), PxVec3(-.5f, 0, .5f), PxVec3(-.5f, 0, -.5f) };

				// Multiply each of the wedge vertices by the provided scale.
				if (scale != PxVec3(1.f))
				{
					for (int i = 0; i < 6; i++)
						wedge_verts[i] = wedge_verts[i].multiply(scale);
				}

				// Create geometry using the wedge vertices, this can produce a variety of results when combined with the scale as aforementioned.
				CreateShape(PxConvexMeshGeometry(ConvexMesh::CookMesh(vector<PxVec3>(&wedge_verts[0], &wedge_verts[6]))), density);
			}
	};
}

#endif