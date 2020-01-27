#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/Math/Vector2.hpp>

namespace en
{

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

	ObjectBase();

	virtual ObjectType GetObjectType() const = 0;

	U32 GetID() const;
	const Vector2f& GetPosition() const;

protected:
	U32 mID;
	Vector2f mPosition;
};

} // namespace en