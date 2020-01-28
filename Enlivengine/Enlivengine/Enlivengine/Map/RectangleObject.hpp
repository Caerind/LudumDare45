#pragma once

#include <Enlivengine/Map/ObjectBase.hpp>

namespace en
{
namespace tmx
{

class RectangleObject : public ObjectBase
{
public:
	RectangleObject(ObjectGroup& objectGroup);

	virtual ObjectType GetObjectType() const
	{
		return ObjectBase::ObjectType::Rectangle;
	}

	const Vector2f& GetSize() const;

private:
	friend class ObjectGroup;
	bool Parse(ParserXml& parser);

	Vector2f mSize;
};

} // namespace tmx
} // namespace en