#pragma once

#include <Enlivengine/Map/ObjectBase.hpp>

namespace en
{
namespace tmx
{

class EllipseObject : public ObjectBase
{
public:
	EllipseObject(ObjectGroup& objectGroup);

	virtual ObjectType GetObjectType() const
	{
		return ObjectBase::ObjectType::Ellipse;
	}

	const Vector2f& GetSize() const;

protected:
	friend class ObjectGroup;
	bool Parse(ParserXml& parser);

	Vector2f mSize;
};

} // namespace tmx
} // namespace en