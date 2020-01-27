#include <Enlivengine/Map/ObjectBase.hpp>

namespace en
{

ObjectBase::ObjectBase()
	: mID(0)
	, mPosition(0.0f, 0.0f)
{
}

U32 ObjectBase::GetID() const
{
	return mID;
}

const Vector2f& ObjectBase::GetPosition() const
{
	return mPosition;
}

} // namespace en