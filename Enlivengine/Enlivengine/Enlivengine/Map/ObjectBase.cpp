#include <Enlivengine/Map/ObjectBase.hpp>

#include <Enlivengine/Map/ObjectGroup.hpp>

namespace en
{
namespace tmx
{

ObjectBase::ObjectBase(ObjectGroup& objectGroup)
	: mObjectGroup(objectGroup)
	, mID(0)
	, mPosition(0.0f, 0.0f)
{
}

ObjectGroup& ObjectBase::GetObjectGroup()
{
	return mObjectGroup;
}

const ObjectGroup& ObjectBase::GetObjectGroup() const
{
	return mObjectGroup;
}

U32 ObjectBase::GetID() const
{
	return mID;
}

const Vector2f& ObjectBase::GetPosition() const
{
	return mPosition;
}

bool ObjectBase::Parse(ParserXml& parser)
{
	parser.getAttribute("id", mID);
	parser.getAttribute("x", mPosition.x);
	parser.getAttribute("y", mPosition.y);
	return true;
}

} // namespace tmx
} // namespace en