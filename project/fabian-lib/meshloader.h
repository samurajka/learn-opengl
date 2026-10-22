// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_MESH_LOADER_H_
#define PGT_MESH_LOADER_H_

#include <filesystem>
#include <string>
#include <map>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // output data structure
#include <assimp/postprocess.h>     // post processing flags
#include <assimp/ObjMaterial.h>		// obj-specific material macros

#include "mesh.h"
#include "texture.h"

class MeshLoader
{
public:
	MeshLoader()
	{
		// create the default material for meshes without assigned material
		material_table_.push_back( std::make_shared<Material>( "DefaultMaterial" ) );
		material_proxy_cache_.insert( { material_table_[0]->name(), 0 } );
	}

	~MeshLoader() = default;

	template<class F = Triangle, class V = Vertex>
	inline void LoadTriangularMesh( const std::string & file_name, std::vector<std::shared_ptr<TriangularMesh>> & meshes, const glm::mat4 & transformation = glm::mat4( 1.0f ), const glm::vec3 & pivot = glm::vec3(),
		const unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType )
	{
		const bool rewrite_existing = false; // rewrite parameters of the existing material
		std::filesystem::path absolute_path;
		try
		{
			absolute_path = std::filesystem::canonical( file_name ).remove_filename();
		}
		catch ( const std::exception & )
		{
			printf( "Failed to load scene from '%s'.\n", file_name.c_str() );

			return;
		}

		Assimp::Importer importer; // everything will be cleaned up by the importer destructor

		printf( "Loading scene from '%s'...\n", file_name.c_str() );

		const aiScene * scene = importer.ReadFile( file_name, flags );

		if ( scene )
		{
			printf( "The scene loading has been completed.\n" );

			//printf( "Building the scene containing %d mesh(es), %d material(s), and %d texture(s)...\n",
			printf( "Building the scene containing %d mesh(es) and %d material(s)...\n",
				scene->mNumMeshes, scene->mNumMaterials/*, scene->mNumTextures*/ );

			if ( scene->HasMeshes() )
			{
				for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
				{
					// mesh represents a geometry with a single material (at least the default one)
					const auto mesh = scene->mMeshes[i];

					std::shared_ptr<TriangularMesh> custom_mesh = std::make_shared<TriangularMesh>( mesh->mName.C_Str() );

					custom_mesh->set_transformation( transformation );
					custom_mesh->set_pivot( pivot );

					int mat_idx = 0;

					if ( scene->HasMaterials() )
					{
						aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

						if ( material )
						{
							//std::shared_ptr<Material> custom_material = std::make_shared<Material>( material->GetName().C_Str() );							
							bool is_new = false;
							std::shared_ptr<Material> custom_material = material_proxy( material->GetName().C_Str(), is_new, mat_idx );

							custom_mesh->set_material( custom_material );

							if ( is_new || rewrite_existing )
							{
								// parameters

								aiColor3D color( 0.0f, 0.0f, 0.0f );

								material->Get( AI_MATKEY_COLOR_AMBIENT, color );
								//custom_material->ambient_color = Color3f::toLinear( Color3f( { color.r, color.g, color.b } ) );
								custom_material->ambient_color = Color3f( { color.r, color.g, color.b } );

								material->Get( AI_MATKEY_COLOR_DIFFUSE, color );
								//custom_material->diffuse_color = Color3f::toLinear( Color3f( { color.r, color.g, color.b } ) );
								custom_material->diffuse_color = Color3f( { color.r, color.g, color.b } );

								float glossiness = 0.0f;
								material->Get( AI_MATKEY_SHININESS, glossiness );
								custom_material->glossiness_value = glossiness;

								float metallic = 0.0f;
								material->Get( AI_MATKEY_METALLIC_FACTOR, metallic );
								custom_material->metallic_value = metallic;

								float roughness = 0.0f;
								material->Get( AI_MATKEY_ROUGHNESS_FACTOR, roughness );
								custom_material->roughness_value = roughness;

								material->Get( AI_MATKEY_COLOR_SPECULAR, color );
								//custom_material->specular_color = Color3f::toLinear( Color3f( { color.r, color.g, color.b } ) );
								custom_material->specular_color = Color3f( { color.r, color.g, color.b } );

								material->Get( AI_MATKEY_COLOR_EMISSIVE, color );
								custom_material->emissive_color = Color3f( { color.r, color.g, color.b } );

								material->Get( AI_MATKEY_COLOR_TRANSPARENT, color ); // AI_MATKEY_TRANSPARENCYFACTOR
								custom_material->transparent_color = Color3f( { color.r, color.g, color.b } );

								material->Get( AI_MATKEY_REFRACTI, custom_material->ior );

								material->Get( AI_MATKEY_DIELECTRIC_PRIORITY, custom_material->dielectric_priority );

								ai_int illum_model = 0;
								material->Get( AI_MATKEY_SHADING_MODEL, illum_model );
								custom_material->bxdf = BxDFType( illum_model );

								// textures

								aiString file_name;
								aiTextureMapping mapping;
								unsigned int uvIndex;
								float blend;
								aiTextureMapMode mode;

								material->GetTexture( aiTextureType_DIFFUSE, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->diffuse_map = texture_3u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								material->GetTexture( aiTextureType_SHININESS, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->glossiness_map = texture_3u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								material->GetTexture( aiTextureType_SPECULAR, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->specular_map = texture_3u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								if ( material->GetTexture( aiTextureType_DISPLACEMENT, 0, &file_name, &mapping, &uvIndex, &blend, 0, &mode ) == AI_SUCCESS )
								{
									if ( file_name.length )
									{
										bool is_new = false;
										custom_material->displacement_map = texture_1u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
										file_name.Clear();

										if ( material->Get( AI_MATKEY_OBJ_DISPSCALE_HEIGHT( 0 ), custom_material->displacement_scale ) == AI_SUCCESS )
										{
											//printf( "displacement_scale = %f\n", custom_material->displacement_scale );
										}
										if ( material->Get( AI_MATKEY_OBJ_DISPTESS_HEIGHT( 0 ), custom_material->tessellation_rate ) == AI_SUCCESS )
										{
											//printf( "tessellation_rate = %f\n", custom_material->tessellation_rate );
										}
									}
								}

								material->GetTexture( aiTextureType_NORMALS, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->normal_map = texture_3u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								material->GetTexture( aiTextureType_METALNESS, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->metallic_map = texture_1u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								material->GetTexture( aiTextureType_DIFFUSE_ROUGHNESS, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->roughness_map = texture_1u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}

								material->GetTexture( aiTextureType_OPACITY, 0, &file_name );
								if ( file_name.length )
								{
									bool is_new = false;
									custom_material->opacity_map = texture_1u_proxy( ( absolute_path / std::filesystem::path( std::string( file_name.C_Str() ) ) ).string(), is_new );
									file_name.Clear();
								}
							}
						}
					}

					// only triangular primitives are accepted
					if ( mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE )
					{
						if ( mesh->HasFaces() )
						{
							// fill per-vertex data
							for ( unsigned int j = 0; j < mesh->mNumVertices; ++j )
							{
								V vertex{};

								if ( mesh->HasPositions() )
								{
									vertex.position = glm::vec3( mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z );
								}

								if ( mesh->HasNormals() )
								{
									vertex.normal = glm::normalize( glm::vec3( mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z ) );
								}

								if ( mesh->HasTextureCoords( 0 ) )
								{
									vertex.tex_coord = glm::vec2( mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y );
								}

								if ( mesh->HasTangentsAndBitangents() )
								{
									vertex.tangent = glm::normalize( glm::vec3( mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z ) );
								}
								else
								{
									vertex.tangent = orthonormal( vertex.normal );
								}

								vertex.mat_idx = glm::uvec1( mat_idx );

								custom_mesh->push_back( vertex );
							}

							printf( "Mesh (%d) '%s' has material '%s' with mat_idx %d\n", i, mesh->mName.C_Str(),
								custom_mesh->material()->name().c_str(), mat_idx );

							// fill indices
							for ( unsigned int j = 0; j < mesh->mNumFaces; ++j )
							{
								const auto & face = mesh->mFaces[j];

								Triangle triangle{};

								for ( unsigned int k = 0; k < face.mNumIndices; ++k )
								{
									triangle.indices[k] = face.mIndices[k];
								}

								custom_mesh->push_back( triangle );
							}
						}
					}

					meshes.push_back( custom_mesh );
				}
			}

			printf( "The scene building has been completed.\n" );
		}
		else
		{
			printf( "The scene loading has failed.\n" );
		}
	}

	/* returns texture from the proxy cache or creates a new one */
	std::shared_ptr<Texture1u> texture_1u_proxy( const std::string & file_name, bool & is_new )
	{
		if ( texture_1u_proxy_cache_.count( file_name ) == 0 )
		{
			auto texture = std::make_shared<Texture1u>( file_name, true );
			if ( texture->width() > 0 && texture->height() > 0 )
			{
				texture_1u_proxy_cache_[file_name] = texture;
				is_new = true;
			}
			else
			{
				is_new = false;
				return nullptr;
			}
		}
		else
		{
			is_new = false;
		}

		return texture_1u_proxy_cache_.at( file_name );
	}

	/* returns texture from the proxy cache or creates a new one */
	std::shared_ptr<Texture3u> texture_3u_proxy( const std::string & file_name, bool & is_new )
	{
		if ( texture_3u_proxy_cache_.count( file_name ) == 0 )
		{
			texture_3u_proxy_cache_[file_name] = std::make_shared<Texture3u>( file_name, true );
			is_new = true;
		}
		else
		{
			is_new = false;
		}

		return texture_3u_proxy_cache_.at( file_name );
	}

	/* returns material from the proxy cache or creates a new one */
	std::shared_ptr<Material> material_proxy( const std::string & name, bool & is_new, int & mat_idx )
	{
		if ( material_proxy_cache_.count( name ) == 0 )
		{
			const auto material = std::make_shared<Material>( name );
			material_proxy_cache_[name] = int( material_table_.size() );
			material_table_.push_back( material );
			is_new = true;
		}
		else
		{
			is_new = false;
		}

		mat_idx = material_proxy_cache_.at( name );

		return material_table_[mat_idx];
	}

	/* number of loaded materials */
	size_t material_count() const
	{
		return material_proxy_cache_.size();
	}

	/* return the material table with materials stored in the material proxy cache; material index corresponds to vertex.mat_idx */
	std::vector<std::shared_ptr<Material>> & material_table()
	{
		return material_table_;
	}

private:
	std::map<std::string, std::shared_ptr<Texture1u>> texture_1u_proxy_cache_; /* 1u textures proxy cache */
	std::map<std::string, std::shared_ptr<Texture3u>> texture_3u_proxy_cache_; /* 3u textures proxy cache */
	//std::map<std::string, std::shared_ptr<Material>> material_proxy_cache_; /* materials proxy cache */
	std::map<std::string, int> material_proxy_cache_; /* materials proxy cache */
	std::vector<std::shared_ptr<Material>> material_table_; /* table of materials in the order of loading */
};

#endif // PGT_MESH_LOADER_H_
