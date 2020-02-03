#pragma once

#include <vector>

#include <Enlivengine/Map/Tileset.hpp>
#include <Enlivengine/Map/Map.hpp>

#include "GameConfig.hpp"

class GameMap
{
public:
	GameMap();

	std::vector<en::Vector2u> getNeighbors(const en::Vector2u& coords, bool diag = false);
	en::Vector2u worldToCoords(const en::Vector2f& world);
	en::Vector2f coordsToWorld(const en::Vector2u& coords);

	void load(en::U32 mapID, const en::Vector2f& spawnPoint, en::tmx::TilesetPtr tileset = en::tmx::TilesetPtr(), const en::Vector2i& tileSize = en::Vector2i::zero);

	en::U32 getTileId(const en::Vector2i& coords);
	void setTileId(const en::Vector2i& coords, en::U32 id);

	en::tmx::TilesetPtr getTileset() const;
	void setTileset(en::tmx::TilesetPtr tileset);

	en::U32 getMapId() const { return mMapID; }
	const en::Vector2f& getSpawnPoint() const { return mSpawnPoint; }

	void setCollision(const en::Vector2i& coords, bool collide);
	bool collide(const en::Vector2i& coords);

	virtual void render(sf::RenderTarget& target);

private:
	std::vector<bool> mCollisions;

    en::tmx::MapPtr mMap;
	en::tmx::TilesetPtr mTileset;

	en::U32 mMapID;
	en::Vector2f mSpawnPoint;
};
