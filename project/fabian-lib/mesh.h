// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_MESH_H_
#define PGT_MESH_H_

#include "baseobject.h"
#include "vertex.h"
#include "material.h"

#include <array>
#include <vector>
#include <memory>

#include "../../glm/gtc/type_ptr.hpp"
#include "../../glm/gtx/norm.hpp" // glm::length2

inline bool is_valid_normal( const glm::vec3 & v, const float epsilon = 1e-5f )
{
	return std::isfinite( v.x ) && std::isfinite( v.y ) && std::isfinite( v.z ) &&
		//std::isnormal( v.x ) && std::isnormal( v.y ) && std::isnormal( v.z ) &&
		glm::length2( v ) > sqr( epsilon );
}

/*
It is possible to use either uint16_t or uint32_t for index buffer.

For aligned std::vector see
https://stackoverflow.com/questions/8456236/how-is-a-vectors-data-aligned
*/

class Triangle
{
public:
	/*alignas( 16 )*/ std::array<unsigned int, 3> indices;
};

// https://stackoverflow.com/questions/39402823/opengl-triangle-adjacency-calculation

class TriangleWithAdjacency
{
public:
	/*alignas( 16 )*/ std::array<unsigned int, 6> indices;
};

template <class F, class V>
class Mesh : public BaseObject
{
public:
	/* instantiates a named mesh */
	Mesh( const std::string & name ) : BaseObject( ObjectType::kTriangularMesh, name )
	{}

	/* adds a new face at the end of the index buffer */
	void push_back( const F & face )
	{
		faces_.push_back( face );
	}

	/* adds a new face at the end of the vertex buffer */
	void push_back( const V & vertex )
	{
		vertices_.push_back( vertex );
	}

	/* index buffer pointer */
	F const * const index_buffer() const
	{
		return faces_.data();
	}

	/* vertex buffer pointer */
	V const * const vertex_buffer() const
	{
		return vertices_.data();
	}

	/* number of faces */
	size_t face_count() const
	{
		return faces_.size();
	}	

	/* index buffer items count */
	size_t index_buffer_count() const
	{
		return face_count() * 3; // TODO fix fixed multiplier
	}

	/* index buffer size in bytes */
	size_t index_buffer_size() const
	{
		return face_count() * sizeof( F );
	}

	/* vertex buffer items count */
	size_t vertex_buffer_count() const
	{
		return vertices_.size();
	}

	/* vertex buffer size in bytes */
	size_t vertex_buffer_size() const
	{
		return vertex_buffer_count() * sizeof( V );
	}

	/* assigned material */
	std::shared_ptr<Material> material()
	{
		return material_;
	}

	/* assign material */
	void set_material( std::shared_ptr<Material> material )
	{
		material_ = material;
	}

	glm::mat4 transformation() const
	{
		//return ( const float * )( glm::value_ptr( transformation_ ) );
		return transformation_;
	}

	void set_transformation( const glm::mat4 & transformation )
	{
		transformation_ = transformation;
	}

	glm::vec3 pivot() const
	{
		return pivot_;
	}

	void set_pivot( const glm::vec3 & pivot )
	{
		pivot_ = pivot;
	}

	/* applying transform values essentially resets the values of object's location, rotation or scale, while visually keeping the object data in-place */
	void apply_transformation()
	{

	}

private:
	std::vector<F> faces_; /* index buffer */
	std::vector<V> vertices_; /* vertex buffer of vertices in model space */
	std::shared_ptr<Material> material_; /* mesh material */
	// TODO consider RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR
	glm::mat4 transformation_{ 1.0f }; /* model space to world space transformation 4x4 homogeneous matrix in column-major format */
	glm::vec3 pivot_{ 0.0f, 0.0f, 0.0f }; /* pivot point used as the center of transformation (rotation and scale) */
};

using TriangularMesh = Mesh<Triangle, Vertex>;
using TriangularMeshWithAdjacency = Mesh<TriangleWithAdjacency, Vertex>;

template<>
inline void TriangularMesh::apply_transformation()
{
	const glm::mat4 N = glm::transpose( glm::inverse( transformation_ ) ); // see https://www.cs.upc.edu/~robert/teaching/idi/normalsOpenGL.pdf

	for ( auto & vertex : vertices_ )
	{
		vertex.position -= pivot_;
		const glm::vec4 position = transformation_ * glm::vec4( vertex.position, 1.0f );
		vertex.position = position / position.w;
		vertex.position += pivot_;

		if ( is_valid_normal( vertex.normal ) )
		{
			const glm::vec4 normal = N * glm::vec4( vertex.normal, 0.0f );
			vertex.normal = glm::normalize( glm::vec3( normal ) );
		}

		if ( is_valid_normal( vertex.tangent ) )
		{
			const glm::vec4 tangent = N * glm::vec4( vertex.tangent, 0.0f );
			vertex.tangent = glm::normalize( glm::vec3( tangent ) );
		}
	}

	transformation_ = glm::mat4( 1.0f );
}

#endif // PGT_MESH_H_
