// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_MATERIAL_H_
#define PGT_MATERIAL_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#pragma pack( push ) // resolves warning C4103
#include "../../glm/glm/vec3.hpp"
#pragma pack( pop )
#include "../../glm/glm/glm.hpp"

#include <memory>
#include <optional>

#include "baseobject.h"
#include "color.h"
#include "texture.h"

enum class BxDFType
{
	kNormal = 0,
	kLambert,
	kMirror,
	kGlass,
	kModifiedPhong,
	kCookTorrance,
};

class Material : public BaseObject
{
public:
	/* instantiates a named material */
	Material( const std::string & name ) : BaseObject( ObjectType::kMaterial, name )
	{
	}

	/* ambient color */
	Color3f ambient( std::optional<glm::vec2> uv ) const
	{
		return ambient_color.value_or( Color3f::black );
	}

	/* diffuse (albedo) color of specular-glossiness workflow */
	Color3f diffuse( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && diffuse_map )
		{
			return Color3f( diffuse_map->texel( uv.value() ) ) *
				diffuse_color.value_or( Color3f::white );
		}
		else
		{
			return diffuse_color.value_or( Color3f::gray50 );
		}
	}

	/* glossiness value of specular-glossiness workflow */
	float glossiness( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && glossiness_map )
		{
			return ( ( glossiness_map->texel( uv.value() ) ).luminance() / 255.0f ) *
				glossiness_value.value_or( 1.0f );
		}
		else
		{
			return glossiness_value.value_or( 0.5f );
		}
	}

	/* metallic value of metal/roughness workflow */
	float metallic( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && metallic_map )
		{
			return ( ( metallic_map->texel( uv.value() ) ).luminance() / 255.0f ) *
				metallic_value.value_or( 1.0f );
		}
		else
		{
			return metallic_value.value_or( 0.0f );
		}
	}

	/* roughness value of metal/roughness workflow */
	float roughness( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && roughness_map )
		{
			return ( ( roughness_map->texel( uv.value() ) ).luminance() / 255.0f ) *
				roughness_value.value_or( 1.0f );
		}
		else
		{
			return roughness_value.value_or( 0.5f );
		}
	}

	Color3f specular( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && specular_map )
		{
			return Color3f( specular_map->texel( uv.value() ) ) *
				specular_color.value_or( Color3f::white );
		}
		else
		{
			return specular_color.value_or( Color3f::gray50 );
		}
	}

	Color3f transparent( std::optional<glm::vec2> uv ) const
	{
		return transparent_color.value_or( Color3f::white );
	}

	float opacity( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && opacity_map )
		{
			return opacity_map->texel( uv.value() ).data[0];
		}
		else
		{
			return 1.0f;
		}
	}

	glm::vec3 normal( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && normal_map )
		{
			const Color3u texel = normal_map->texel( uv.value() );

			return glm::normalize( glm::vec3(
				texel.data[0] * ( 2.0f / 255.0f ) - 1.0f,
				texel.data[1] * ( 2.0f / 255.0f ) - 1.0f,
				texel.data[2] * ( 2.0f / 255.0f ) - 1.0f ) );
		}
		else
		{
			return glm::vec3( 0.0f, 0.0f, 1.0f );
		}
	}

	float displacement( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && displacement_map )
		{
			return ( displacement_map->texel( uv.value() ) ).luminance() * displacement_scale / 255.0f;
		}
		else
		{
			return 0.0f;
		}
	}

	Color3f emissive( std::optional<glm::vec2> uv ) const
	{
		if ( uv.has_value() && emissive_map )
		{
			return emissive_map->texel( uv.value() ) *
				emissive_color.value_or( Color3f::white );
		}
		else
		{
			return emissive_color.value_or( Color3f::black );
		}
	}

	std::optional<Color3f> ambient_color; /* linear ambient color */

	// specular/glossiness workflow
	std::optional<Color3f> diffuse_color; /* linear diffuse (albedo) color */
	std::shared_ptr<Texture3u> diffuse_map; /* srgb diffuse (albedo) texture */
	std::optional<float> glossiness_value; /* linear glossiness (shininess, smoothness) value */
	std::shared_ptr<Texture3u> glossiness_map; /* linear glossiness (shininess, smoothness) texture */
	std::optional<Color3f> specular_color; /* linear specular color */
	std::shared_ptr<Texture3u> specular_map; /* srgb specular texture */

	// metal/roughness workflow
	std::optional<Color3f> base_color; /* linear base color */
	std::shared_ptr<Texture3u> base_color_map; /* srgb base color texture */
	std::optional<float> roughness_value; /* linear roughness value */
	std::shared_ptr<Texture1u> roughness_map; /* linear roughness texture */
	std::optional<float> metallic_value; /* linear metallic (metallness) value */
	std::shared_ptr<Texture1u> metallic_map; /* linear metallic (metallness) texture */

	// common part
	float displacement_scale{ 1.0f }; /* scale of displacement */
	float tessellation_rate{ 0.0f }; /* uniform tessellation rate of the geometry */
	std::shared_ptr<Texture1u> displacement_map; /* linear displacement texture */
	std::shared_ptr<Texture3u> normal_map; /* linear normal texture */
	std::shared_ptr<Texture1u> opacity_map; /* linear opacity texture */

	float ior{ 0.0f }; /* index of refraction */
	static const float kIORDefault; /* index lomu vzduchu za normálního tlaku */
	std::optional<Color3f> transparent_color; /* linear (Napierian) attenuation coefficient */
	int dielectric_priority{ 0 }; /* dielectric priority - higher value means higher priority */

	std::optional<Color3f> emissive_color; /* linear emissive color */
	std::shared_ptr<Texture3f> emissive_map; /* linear emissive texture */

	BxDFType bxdf{ BxDFType::kLambert }; /* desired bxdf */

	// TODO: consider using only linear 3f textures

private:

};

#endif // PGT_MATERIAL_H_
