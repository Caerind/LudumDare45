#include <Enlivengine/Map/Map.hpp>

#include <Enlivengine/System/ParserXml.hpp>
#include <filesystem>

#include <Enlivengine/Map/TileLayer.hpp>
#include <Enlivengine/Map/ObjectGroup.hpp>

namespace en
{
namespace tmx
{
	
Map::Map()
	: mName("")
	, mSize(0,0)
	, mTileSize(0,0)
	, mOrientation(Orientation::Orthogonal)
	, mRenderOrder(RenderOrder::RightDown)
	, mBackgroundColor(Color::Transparent)
	, mStaggerAxis(StaggerAxis::Y)
	, mStaggerIndex(StaggerIndex::Odd)
	, mHexSideLength(0)
	, mNextLayerID(0)
	, mNextObjectID(0)
{
}

bool Map::LoadFromFile(const std::string& filename)
{
	ParserXml xml;
	if (!xml.loadFromFile(filename))
	{
		LogError(en::LogChannel::Graphics, 9, "Can't open file at %s", filename.c_str());
		return false;
	}

	if (xml.readNode("map"))
	{
		xml.getAttribute("name", mName);

		std::string attribStr;
		
		attribStr = "orthogonal";
		xml.getAttribute("orientation", attribStr);
		if (attribStr == "orthogonal")
		{
			mOrientation = Orientation::Orthogonal;
		}
		else if (attribStr == "isometric")
		{
			mOrientation = Orientation::Isometric;
		}
		else if (attribStr == "staggered")
		{
			mOrientation = Orientation::Staggered;
		}
		else if (attribStr == "hexagonal")
		{
			mOrientation = Orientation::Hexagonal;
		}
		else
		{
			LogError(en::LogChannel::Graphics, 9, "%s maps aren't supported yet", attribStr.c_str());
			return false;
		}

		attribStr = "right-down";
		xml.getAttribute("renderorder", attribStr);
		if (attribStr == "right-down")
		{
			mRenderOrder = RenderOrder::RightDown;
		}
		else if (attribStr == "right-up")
		{
			mRenderOrder = RenderOrder::RightUp;
		}
		else if (attribStr == "left-down")
		{
			mRenderOrder = RenderOrder::LeftDown;
		}
		else if (attribStr == "left-up")
		{
			mRenderOrder = RenderOrder::LeftUp;
		}
		else
		{
			LogWarning(en::LogChannel::Graphics, 7, "Invalid renderorder %s", attribStr.c_str());
		}

		xml.getAttribute("width", mSize.x);
		xml.getAttribute("height", mSize.y);

		xml.getAttribute("tilewidth", mTileSize.x);
		xml.getAttribute("tileheight", mTileSize.y);

		xml.getAttribute("hexsidelength", mHexSideLength);
		if (mOrientation == Orientation::Hexagonal && mHexSideLength <= 0)
		{
			LogWarning(en::LogChannel::Graphics, 7, "Invalid hexsidelength %s", attribStr.c_str());
		}

		attribStr = "y";
		xml.getAttribute("staggeraxis", attribStr);
		if (attribStr == "y")
		{
			mStaggerAxis = StaggerAxis::Y;
		}
		else if (attribStr == "x")
		{
			mStaggerAxis = StaggerAxis::X;
		}
		else
		{
			if (mOrientation == Orientation::Staggered || mOrientation == Orientation::Hexagonal)
			{
				LogWarning(en::LogChannel::Graphics, 7, "Invalid staggeraxis %s", attribStr.c_str());
			}
		}

		attribStr = "odd";
		xml.getAttribute("staggerindex", attribStr);
		if (attribStr == "odd")
		{
			mStaggerIndex = StaggerIndex::Odd;
		}
		else if (attribStr == "even")
		{
			mStaggerIndex = StaggerIndex::Even;
		}
		else
		{
			if (mOrientation == Orientation::Staggered || mOrientation == Orientation::Hexagonal)
			{
				LogWarning(en::LogChannel::Graphics, 7, "Invalid staggerindex %s", attribStr.c_str());
			}
		}

		attribStr = "";
		xml.getAttribute("backgroundcolor", attribStr);
		if (attribStr.size() > 0)
		{
			if (attribStr[0] == '#')
			{
				mBackgroundColor.fromString(attribStr.substr(1));
			}
			else
			{
				mBackgroundColor.fromString(attribStr);
			}
		}

		I32 infinite = 0;
		xml.getAttribute("infinite", infinite);
		if (infinite > 0)
		{
			// TODO : Infinite maps
			LogError(en::LogChannel::Graphics, 9, "Infinite maps aren't supported yet");
			return false;
		}

		xml.getAttribute("nextlayerid", mNextLayerID);
		xml.getAttribute("nextobjectid", mNextObjectID);

		if (xml.readFirstNode())
		{
			do 
			{
				std::string nodeName = xml.getNodeName();
				if (nodeName == "tileset")
				{
					TilesetMapData tilesetData;
					tilesetData.tileset = TilesetPtr();
					tilesetData.firstGid = 0;

					bool validTileset = true;

					xml.getAttribute("firstgid", tilesetData.firstGid);
					if (tilesetData.firstGid <= 0)
					{
						validTileset = false;
						LogWarning(en::LogChannel::Graphics, 7, "Invalid firstGid");
					}

					std::string source = "";
					xml.getAttribute("source", source);
					if (source.size() > 0)
					{
						const std::string currentPath = std::filesystem::path(filename).remove_filename().string();
						source = currentPath + source;

						tilesetData.tileset = ResourceManager::GetInstance().GetFromFilename<Tileset>(source);
						if (!tilesetData.tileset.IsValid())
						{
							const std::string identifier = mName + "-tileset" + std::to_string(mTilesets.size());
							tilesetData.tileset = ResourceManager::GetInstance().Create<Tileset>(identifier, TilesetLoader::FromFile(source));
							if (!tilesetData.tileset.IsValid())
							{
								validTileset = false;
								LogError(en::LogChannel::Graphics, 8, "Can't load tileset %s", source.c_str());
							}
						}
					}
					else
					{
						validTileset = false;
						LogError(en::LogChannel::Graphics, 8, "Tileset inside maps aren't supported yet");
						// TODO : Tilesets inside maps
					}

					if (validTileset)
					{
						mTilesets.push_back(tilesetData);
					}
				}
				else if (nodeName == "layer")
				{
					std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>(*this);
					if (layer != nullptr && layer->Parse(xml))
					{
						mLayers.emplace_back(std::move(layer));
					}
				}
				else if (nodeName == "objectgroup")
				{
					std::unique_ptr<ObjectGroup> objectgroup = std::make_unique<ObjectGroup>(*this);
					if (objectgroup != nullptr && objectgroup->Parse(xml))
					{
						mLayers.emplace_back(std::move(objectgroup));
					}
				}
				else if (nodeName == "imagelayer")
				{
					// TODO : ImageLayer
				}
				else if (nodeName == "group")
				{
					// TODO : Group
				}
				else if (nodeName == "properties")
				{
					// TODO : Properties
				}
				else
				{
					LogError(en::LogChannel::Graphics, 8, "Unknown layer type %s", nodeName.c_str());
				}

			} while (xml.nextSibling());
			xml.closeNode();
		}
	}
	else
	{
		LogError(en::LogChannel::Graphics, 9, "Invalid map file at %s", filename.c_str());
		return false;
	}

	return true;
}

const std::string& Map::GetName() const
{
	return mName;
}

const Vector2u& Map::GetSize() const
{
	return mSize;
}

const Vector2u& Map::GetTileSize() const
{
	return mTileSize;
}

Map::Orientation Map::GetOrientation() const
{
	return mOrientation;
}

Map::RenderOrder Map::GetRenderOrder() const
{
	return mRenderOrder;
}

const Color& Map::GetBackgroundColor() const
{
	return mBackgroundColor;
}

Map::StaggerAxis Map::GetStaggerAxis() const
{
	return mStaggerAxis;
}

Map::StaggerIndex Map::GetStaggerIndex() const
{
	return mStaggerIndex;
}

U32 Map::GetHexSideLength() const
{
	return mHexSideLength;
}

U32 Map::GetTilesetIndexFromGID(U32 gid) const
{
	assert(GetTilesetCount() > 0);
	assert(gid > 0);
	const U32 tilesetCount = GetTilesetCount();
	for (U32 tilesetIndex = 0; tilesetIndex < tilesetCount; ++tilesetIndex)
	{
		const U32 firstGid = GetTilesetFirstGid(tilesetIndex);
		if (firstGid <= gid)
		{
			gid -= firstGid;
			const TilesetPtr& tilesetPtr = mTilesets[tilesetIndex].tileset;
			if (tilesetPtr.IsValid())
			{
				// TODO : < or <= ?
				if (gid < tilesetPtr.Get().GetTileCount())
				{
					return tilesetIndex;
				}
			}
		}
	}
	assert(false);
	return U32_Max;
}

TilesetPtr Map::GetTileset(U32 tilesetIndex) const
{
	assert(tilesetIndex < GetTilesetCount());
	return mTilesets[tilesetIndex].tileset;
}

U32 Map::GetTilesetFirstGid(U32 tilesetIndex) const
{
	assert(tilesetIndex < GetTilesetCount());
	return mTilesets[tilesetIndex].firstGid;
}

U32 Map::GetTilesetCount() const
{
	return static_cast<U32>(mTilesets.size());
}

LayerBase* Map::GetLayerByIndex(U32 layerIndex)
{
	assert(layerIndex < GetLayerCount());
	return mLayers[layerIndex].get();
}

LayerBase::LayerType Map::GetLayerTypeByIndex(U32 layerIndex) const
{
	assert(layerIndex < GetLayerCount());
	return mLayers[layerIndex]->GetLayerType();
}

LayerBase* Map::GetLayerByID(U32 layerID)
{
	for (LayerBase::Ptr& layer : mLayers)
	{
		if (layer->GetID() == layerID)
		{
			return layer.get();
		}
	}
	return nullptr;
}

LayerBase::LayerType Map::GetLayerTypeByID(U32 layerID) const
{
	for (const LayerBase::Ptr& layer : mLayers)
	{
		if (layer->GetID() == layerID)
		{
			return layer->GetLayerType();
		}
	}
	assert(false);
	return LayerBase::LayerType::TileLayer;
}

U32 Map::GetLayerCount() const
{
	return static_cast<U32>(mLayers.size());
}

Vector2f Map::CoordsToWorld(const Vector2u& tileCoords) const
{
	// TODO : CoordsToWorld
	ENLIVE_UNUSED(tileCoords);
	return Vector2f(0.0f, 0.0f);
}

Vector2u Map::WorldToCoords(const Vector2f& worldPos) const
{
	// TODO : WorldToCoords
	ENLIVE_UNUSED(worldPos);
	return Vector2u(0, 0);
}

} // namespace tmx
} // namespace en