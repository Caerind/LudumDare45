#include <Enlivengine/Map/ObjectGroup.hpp>

namespace en
{

ObjectGroup::ObjectGroup(Map& map)
	: LayerBase()
	, mMap(map)
	, mColor(Color::Transparent)
	, mObjects()
{
}

const Color& ObjectGroup::GetColor() const
{
	return mColor;
}

ObjectBase* ObjectGroup::GetObjectByIndex(U32 objectIndex)
{
	assert(objectIndex < GetObjectCount());
	return mObjects[objectIndex].get();
}

ObjectBase::ObjectType ObjectGroup::GetObjectTypeByIndex(U32 objectIndex) const
{
	assert(objectIndex < GetObjectCount());
	return mObjects[objectIndex]->GetObjectType();
}

ObjectBase* ObjectGroup::GetObjectByID(U32 objectID)
{
	for (ObjectBase::Ptr& object : mObjects)
	{
		if (object->GetID() == objectID)
		{
			return object.get();
		}
	}
	return nullptr;
}

ObjectBase::ObjectType ObjectGroup::GetObjectTypeByID(U32 objectID) const
{
	for (const ObjectBase::Ptr& object : mObjects)
	{
		if (object->GetID() == objectID)
		{
			return object->GetObjectType();
		}
	}
	assert(false);
	return ObjectBase::ObjectType::Rectangle; // TODO : First
}

U32 ObjectGroup::GetObjectCount() const
{
	return static_cast<U32>(mObjects.size());
}

bool ObjectGroup::Parse(ParserXml& parser)
{
	if (!LayerBase::Parse(parser))
	{
		return false;
	}

	std::string attribStr = "";
	parser.getAttribute("color", attribStr);
	if (attribStr.size() > 0)
	{
		if (attribStr[0] == '#')
		{
			mColor.fromString(attribStr.substr(1));
		}
		else
		{
			mColor.fromString(attribStr);
		}
	}

	// TODO : DrawOrder
	attribStr = "";
	parser.getAttribute("draworder", attribStr);
	if (attribStr == "")
	{

	}
	else if (attribStr == "index")
	{

	}
	else
	{

	}

	return true;
}

} // namespace en