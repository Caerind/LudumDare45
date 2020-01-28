#pragma once

#include <vector>
#include <SFML/Graphics/VertexArray.hpp>

#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>
#include <Enlivengine/Map/Tileset.hpp>
#include <Enlivengine/Graphics/View.hpp>

#include "GameConfig.hpp"

class GameMap
{
public:
	GameMap();

	std::vector<en::Vector2i> getNeighboors(const en::Vector2i& coords, bool diag = false);
	en::Vector2i worldToCoords(const en::Vector2f& world);
	en::Vector2f coordsToWorld(const en::Vector2i& coords);

	void load(en::U32 mapID, const en::Vector2f& spawnPoint, en::tmx::TilesetPtr tileset = en::tmx::TilesetPtr(), const en::Vector2i& tileSize = en::Vector2i::zero);

	std::string getCode();
	bool loadFromCode(const std::string& code);

	en::U32 getTileId(const en::Vector2i& coords);
	void setTileId(const en::Vector2i& coords, en::U32 id);

	const std::string& getName() const;
	void setName(const std::string& name);

	en::tmx::TilesetPtr getTileset() const;
	void setTileset(en::tmx::TilesetPtr tileset);

	const en::Vector2i& getSize() const;
	void setSize(const en::Vector2i& size);

	const en::Vector2i& getTileSize() const;
	void setTileSize(const en::Vector2i& tileSize);

	en::U32 getMapId() const { return mMapID; }
	const en::Vector2f& getSpawnPoint() const { return mSpawnPoint; }

	void setCollision(const en::Vector2i& coords, bool collide);
	bool collide(const en::Vector2i& coords);

	virtual void render(sf::RenderTarget& target);

	void updateGeometry();
	bool isGeometryUpdated() const;
	void ensureUpdateGeometry();

	void updateRender();
	bool isRenderUpdated() const;
	void ensureUpdateRender();

private:
	sf::VertexArray mVertices;

	bool mGeometryUpdated;
	bool mRenderUpdated;

	std::vector<en::U32> mTileGrid;
	std::vector<bool> mCollisions;

	std::string mName;
	en::tmx::TilesetPtr mTileset;
	en::Vector2i mSize;
	en::Vector2i mTileSize;

	en::U32 mMapID;
	en::Vector2f mSpawnPoint;
};
