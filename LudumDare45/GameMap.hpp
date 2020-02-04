#pragma once

#include <vector>

#include <Enlivengine/Map/Tileset.hpp>
#include <Enlivengine/Map/Map.hpp>

#include "GameConfig.hpp"

class GameMap
{
public:
	GameMap();

	void load(en::tmx::MapPtr mapPtr);
	void render(sf::RenderTarget& target);

	en::U32 getMapId() const;
	const en::Vector2f& getSpawnPoint() const;

	void setCollision(const en::Vector2u& coords, bool collide);
	bool collide(const en::Vector2u& coords);

	en::U32 getTileId(const en::Vector2u& coords);
	void setTileId(const en::Vector2u& coords, en::U32 id);

	std::vector<en::Vector2u> getNeighbors(const en::Vector2u& coords, bool diag = false);
	en::Vector2u worldToCoords(const en::Vector2f& world);
	en::Vector2f coordsToWorld(const en::Vector2u& coords);

	void load(en::U32 mapID, const en::Vector2f& spawnPoint, en::tmx::TilesetPtr tileset = en::tmx::TilesetPtr(), const en::Vector2i& tileSize = en::Vector2i::zero);

private:
    en::tmx::MapPtr mMap;

	en::U32 mMapID;
	en::Vector2f mSpawnPoint;
	std::vector<bool> mCollisions;
};
