#include "GameMap.hpp"

#include <Enlivengine/System/Compression.hpp>
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/System/ParserXml.hpp>
#include <Enlivengine/Math/Random.hpp>
#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>
#include <Enlivengine/Application/PathManager.hpp>

#include "EntityPrefab.hpp"
#include "EditorComponents.hpp"
#include "GameSingleton.hpp"

GameMap::GameMap()
{
}

void GameMap::load(en::tmx::MapPtr mapPtr)
{
	mMap = mapPtr;
}

void GameMap::render(sf::RenderTarget& target)
{
	if (mMap.IsValid())
	{
		mMap.Get().Render(target);
	}
}

en::U32 GameMap::getMapId() const
{
	return mMapID;
}

const en::Vector2f& GameMap::getSpawnPoint() const
{
	return mSpawnPoint;
}

void GameMap::setCollision(const en::Vector2u& coords, bool collide)
{

}

bool GameMap::collide(const en::Vector2u& coords)
{
	return false;
}

en::U32 GameMap::getTileId(const en::Vector2u& coords)
{
	if (mMap.IsValid())
	{
		return 0;
	}
	else
	{
		return 0;
	}
}

void GameMap::setTileId(const en::Vector2u& coords, en::U32 id)
{
	if (mMap.IsValid())
	{
	}
}

std::vector<en::Vector2u> GameMap::getNeighbors(const en::Vector2u& coords, bool diag /*= false*/)
{
    if (mMap.IsValid())
    {
        return mMap.Get().GetNeighbors(coords, diag);
    }
    else
    {
        return std::vector<en::Vector2u>();
    }
}

en::Vector2u GameMap::worldToCoords(const en::Vector2f& world)
{
    if (mMap.IsValid())
    {
        return mMap.Get().WorldToCoords(world);
    }
    else
    {
        return en::Vector2u(0, 0);
    }
}

en::Vector2f GameMap::coordsToWorld(const en::Vector2u& coords)
{
    if (mMap.IsValid())
    {
        return mMap.Get().CoordsToWorld(coords);
    }
    else
    {
        return en::Vector2f(0.0f, 0.0f);
    }
}

void GameMap::load(en::U32 mapID, const en::Vector2f& spawnPoint, en::tmx::TilesetPtr tileset /*= en::tmx::TilesetPtr()*/, const en::Vector2i& tileSize /*= en::Vector2i::zero*/)
{
    /*

    if (xml.readNode("layer"))
    {
        if (xml.readNode("data"))
        {
            std::string code;
            xml.getValue(code);
            en::Vector2i coords{ en::Vector2i::zero };
            std::string data;
            std::stringstream ss;
            ss << code;
            ss >> data;
            if (!en::Compression::Decompress64(data))
            {
                return;
            }
            std::vector<en::U8> byteVector;
            byteVector.reserve(max * 4);
            for (std::string::iterator i = data.begin(); i != data.end(); ++i)
            {
                byteVector.push_back(*i);
            }
            for (en::U32 i = 0; i < byteVector.size() - 3; i += 4)
            {
                en::U32 gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;

                if (mTileset.IsValid() && gid > mTileset.Get().GetColumns())
                {
                    mCollisions[coords.x + coords.y * size.x] = true;
                }

                setTileId(coords, gid);

                coords.x = (coords.x + 1) % size.x;
                if (coords.x == 0)
                {
                    coords.y++;
                }
            }

            xml.closeNode();
        }

        if (xml.nextSibling("layer"))
        {
            if (xml.readNode("data"))
            {
                std::string code;
                xml.getValue(code);
                en::Vector2i coords{ en::Vector2i::zero };
                std::string data;
                std::stringstream ss;
                ss << code;
                ss >> data;
                if (!en::Compression::Decompress64(data))
                {
                    return;
                }
                std::vector<en::U8> byteVector;
                byteVector.reserve(max * 4);
                for (std::string::iterator i = data.begin(); i != data.end(); ++i)
                {
                    byteVector.push_back(*i);
                }

                for (en::U32 i = 0; i < byteVector.size() - 3; i += 4)
                {
                    en::U32 gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;

                    if (gid != 0 && mTileset.IsValid())
                    {
                        const en::Vector2f worldPos = coordsToWorld(coords);
                        EntityPrefab::createProps(GameSingleton::world, worldPos.x, worldPos.y, gid, mTileset);
                        mCollisions[coords.x + coords.y * size.x] = true;
                    }

                    coords.x = (coords.x + 1) % size.x;
                    if (coords.x == 0)
                    {
                        coords.y++;
                    }
                }

                xml.closeNode();
            }

            xml.closeNode();
        }
        else
        {
            xml.closeNode();
        }
    }

    updateGeometry();
    updateRender();

    */
}