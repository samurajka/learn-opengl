// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_BASE_OBJECT_H_
#define PGT_BASE_OBJECT_H_

#include <string>

enum ObjectType
{
	//kUnknown,
	kMaterial,
	kBxDF,
	kTriangularMesh,
};

class BaseObject
{
public:
	BaseObject(const ObjectType type, const std::string name);

	ObjectType type() const;
	std::string name() const;

	bool operator==( const BaseObject & other ) const;

private:
	ObjectType type_; /* object type */
	std::string name_; /* object name */	
};

#endif // PGT_BASE_OBJECT_H_
