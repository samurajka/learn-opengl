// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_VERTEX_H_
#define PGT_VERTEX_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#pragma pack( push ) // resolves warning C4103
#include "../../glm/glm/vec4.hpp"
#pragma pack( pop )
#include "../../glm/glm/mat4x4.hpp"
#include "../../glm/ext/vector_uint1.hpp"

class Vertex
{
public:
	__declspec( align( 16 ) ) glm::vec3 position;
	__declspec( align( 16 ) ) glm::vec3 normal;
	__declspec( align( 16 ) ) glm::vec3 tangent;
	__declspec( align( 8 ) ) glm::vec2 tex_coord;
	__declspec( align( 8 ) ) glm::uvec1 mat_idx;

	static size_t size();
};

#endif // PGT_VERTEX_H_
