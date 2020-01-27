#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/Math/Vector2.hpp>

#include <Enlivengine/System/ParserXml.hpp>

#include <memory>
#include <string>

namespace en
{

class Map;

class LayerBase
{
public:
	enum class LayerType
	{
		TileLayer,
		ObjectGroup,
		ImageLayer,
		LayerGroup
	};

	using Ptr = std::unique_ptr<LayerBase>;

	LayerBase();

	virtual LayerType GetLayerType() const = 0;

	U32 GetID() const;
	const std::string& GetName() const;
	const Vector2f& GetOffset() const;
	F32 GetOpacity() const;
	bool IsVisible() const;
	bool IsLocked() const;

protected:
	bool Parse(ParserXml& parser);

	U32 mID;
	std::string mName;
	Vector2f mOffset;
	F32 mOpacity;
	bool mVisible;
	bool mLocked;
};

} // namespace en