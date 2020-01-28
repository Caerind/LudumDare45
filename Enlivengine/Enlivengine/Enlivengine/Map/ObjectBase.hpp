#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/Math/Vector2.hpp>

#include <Enlivengine/System/ParserXml.hpp>

#include <memory>
#include <string>

namespace en
{
namespace tmx
{

class ObjectGroup;

class ObjectBase
{
public:
	enum class ObjectType
	{
		Rectangle,
		Point,
		Ellipse,
		Polygon
	};

	using Ptr = std::unique_ptr<ObjectBase>;

	ObjectBase(ObjectGroup& objectGroup);

	virtual ObjectType GetObjectType() const = 0;

	ObjectGroup& GetObjectGroup();
	const ObjectGroup& GetObjectGroup() const;

	U32 GetID() const;
	const Vector2f& GetPosition() const;

protected:
	bool Parse(ParserXml& parser);

	ObjectGroup& mObjectGroup;

	U32 mID;
	Vector2f mPosition;
};

} // namespace tmx
} // namespace en