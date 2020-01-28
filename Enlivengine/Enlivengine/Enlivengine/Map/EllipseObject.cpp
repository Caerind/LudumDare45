#include <Enlivengine/Map/EllipseObject.hpp>

namespace en
{
namespace tmx
{

EllipseObject::EllipseObject(ObjectGroup& objectGroup)
	: ObjectBase(objectGroup)
	, mSize(0.0f, 0.0f)
{
}

const Vector2f& EllipseObject::GetSize() const
{
	return mSize;
}

bool EllipseObject::Parse(ParserXml& parser)
{
	if (!ObjectBase::Parse(parser))
	{
		return false;
	}

	parser.getAttribute("width", mSize.x);
	parser.getAttribute("height", mSize.y);

	assert(parser.hasChild("ellipse"));

	return true;
}

} // namespace tmx
} // namespace en