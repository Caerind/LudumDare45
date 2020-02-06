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

#include <Enlivengine/Map/ObjectGroup.hpp>
#include <Enlivengine/Map/PointObject.hpp>

GameMap::GameMap()
{
}

void GameMap::load(en::tmx::MapPtr mapPtr)
{
	mMap = mapPtr;
    if (mMap.IsValid())
    {
        en::tmx::Map& map = mMap.Get();
        for (en::U32 i = 0; i < map.GetLayerCount(); ++i)
        {
            en::tmx::LayerBase* layer = map.GetLayerByIndex(i);
            if (layer != nullptr && layer->GetLayerType() == en::tmx::LayerBase::LayerType::ObjectGroup && layer->GetName() == "Objects")
            {
                en::tmx::ObjectGroup& objects = map.GetLayerByIndexAs<en::tmx::ObjectGroup>(i);
                for (en::U32 j = 0; j < objects.GetObjectCount(); ++j)
                {
                    en::tmx::ObjectBase* object = objects.GetObjectByIndex(j);
                    if (object != nullptr && object->GetObjectType() == en::tmx::ObjectBase::ObjectType::Point && object->GetName() == "Spawn")
                    {
                        mSpawnPoint = object->GetPosition();
                    }
                }
            }
        }
    }
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