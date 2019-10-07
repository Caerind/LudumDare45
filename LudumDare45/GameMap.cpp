#include "GameMap.hpp"

#include <Enlivengine/System/Compression.hpp>
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/System/ParserXml.hpp>
#include <Enlivengine/Math/Random.hpp>
#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>

#include "EntityPrefab.hpp"
#include "EditorComponents.hpp"
#include "GameSingleton.hpp"

GameMap::GameMap()
{
	mVertices.setPrimitiveType(sf::Quads);
}

std::vector<en::Vector2i> GameMap::getNeighboors(const en::Vector2i& coords, bool diag /*= false*/)
{
	std::vector<en::Vector2i> n;
	n.reserve(diag ? 8 : 4);
	n.push_back({ coords.x, coords.y - 1 });
	n.push_back({ coords.x, coords.y + 1 });
	n.push_back({ coords.x - 1, coords.y });
	n.push_back({ coords.x + 1, coords.y });
	if (diag)
	{
		n.push_back({ coords.x + 1, coords.y - 1 });
		n.push_back({ coords.x + 1, coords.y + 1 });
		n.push_back({ coords.x - 1, coords.y + 1 });
		n.push_back({ coords.x - 1, coords.y - 1 });
	}
	return n;
}

en::Vector2i GameMap::worldToCoords(const en::Vector2f& world)
{
	// TODO : 0 -0 issue
	en::Vector2i c;
	c.x = ((en::I32)world.x) / mTileSize.x;
	c.y = ((en::I32)world.y) / mTileSize.y;
	return c;
}

en::Vector2f GameMap::coordsToWorld(const en::Vector2i& coords)
{
	en::Vector2f p;
	p.x = (en::F32)(coords.x * mTileSize.x);
	p.y = (en::F32)(coords.y * mTileSize.y);
	return p;
}

void GameMap::load(en::U32 mapID, const en::Vector2f& spawnPoint, en::Tileset* tileset /*= nullptr*/, const en::Vector2i& tileSize /*= en::Vector2i::zero*/)
{
	mTileGrid.clear();
	mCollisions.clear();

	mMapID = mapID;
	mSpawnPoint = spawnPoint;
	mTileset = tileset;
	mTileSize = tileSize;

	const std::string mapPath = "Assets/Maps/";

	en::ParserXml xml;
	std::string filename = mapPath + "map-" + std::to_string(mMapID) + ".tmx";
	if (!xml.loadFromFile(filename))
	{
		LogError(en::LogChannel::Global, 10, "Cant read map : %s", filename.c_str());
		return;
	}
	if (!xml.readNode("map"))
	{
		LogError(en::LogChannel::Global, 10, ("Cant read map xml"));
		return;
	}

	en::Vector2i size;
	xml.getAttribute("width", size.x);
	xml.getAttribute("height", size.y);
	mSize = size;

	en::U32 max = size.x * size.y;
	mTileGrid.resize(max * 4); // TODO : Why 4 ?
	mCollisions.resize(max);
	for (en::U32 i = 0; i < max; i++)
	{
		mCollisions[i] = false;
	}
	ensureUpdateGeometry();

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
			if (!en::Compression::decompress64(data))
			{
				return;
			}
			std::vector<en::U8> byteVector;
			byteVector.reserve(size.x * size.y * 4);
			for (std::string::iterator i = data.begin(); i != data.end(); ++i)
			{
				byteVector.push_back(*i);
			}
			for (en::U32 i = 0; i < byteVector.size() - 3; i += 4)
			{
				en::TileId gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;

				if (gid > mTileset->getColumns())
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
				if (!en::Compression::decompress64(data))
				{
					return;
				}
				std::vector<en::U8> byteVector;
				byteVector.reserve(size.x * size.y * 4);
				for (std::string::iterator i = data.begin(); i != data.end(); ++i)
				{
					byteVector.push_back(*i);
				}

				for (en::U32 i = 0; i < byteVector.size() - 3; i += 4)
				{
					en::TileId gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;

					if (gid != 0)
					{
						const en::Vector2f worldPos = coordsToWorld(coords);
						EntityPrefab::createProps(GameSingleton::world, worldPos.x, worldPos.y, gid, *mTileset);
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
}

std::string GameMap::getCode()
{
	std::string data;
	data.reserve(mSize.x * mSize.y * 4);
	ensureUpdateGeometry();
	en::Vector2i coords;
	for (coords.y = 0; coords.y < mSize.y; coords.y++)
	{
		for (coords.x = 0; coords.x < mSize.x; coords.x++)
		{
			en::U32 id = mTileGrid[coords.x + coords.y * mSize.x];
			data.push_back((en::U8)(id));
			data.push_back((en::U8)(id >> 8));
			data.push_back((en::U8)(id >> 16));
			data.push_back((en::U8)(id >> 24));
		}
	}
	if (!en::Compression::compress64(data))
	{
		LogError(en::LogChannel::Global, 10, "Can't read compress map data");
		return "";
	}
	return data;
}

bool GameMap::loadFromCode(const std::string& code)
{
	en::Vector2i coords{ en::Vector2i::zero };
	const en::U32 FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	const en::U32 FLIPPED_VERTICALLY_FLAG = 0x40000000;
	const en::U32 FLIPPED_DIAGONALLY_FLAG = 0x20000000;
	std::string data;
	std::stringstream ss;
	ss << code;
	ss >> data;
	if (!en::Compression::decompress64(data))
	{
		return false;
	}
	std::vector<en::U8> byteVector;
	byteVector.reserve(mSize.x * mSize.y * 4);
	for (std::string::iterator i = data.begin(); i != data.end(); ++i)
	{
		byteVector.push_back(*i);
	}
	ensureUpdateGeometry();
	for (en::U32 i = 0; i < byteVector.size() - 3; i += 4)
	{
		en::U32 gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;
		gid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

		en::U32 index(coords.x + coords.y * mSize.x);
		mTileGrid[index] = gid;
		if (mTileset != nullptr)
		{
			sf::Vertex* vertex(&mVertices[index * 4]);
			sf::Vector2f pos(mTileset->toPos(gid));
			en::Vector2f texSize(mTileset->getTileSize());
			vertex[0].texCoords = pos;
			vertex[1].texCoords = sf::Vector2f(pos.x + texSize.x, pos.y);
			vertex[2].texCoords = sf::Vector2f(pos.x + texSize.x, pos.y + texSize.y);
			vertex[3].texCoords = sf::Vector2f(pos.x, pos.y + texSize.y);
		}

		coords.x = (coords.x + 1) % mSize.x;
		if (coords.x == 0)
		{
			coords.y++;
		}
	}
	return true;
}

en::U32 GameMap::getTileId(const en::Vector2i& coords)
{
	if (0 <= coords.x && 0 <= coords.y && coords.x < mSize.x && coords.y < mSize.y)
	{
		ensureUpdateGeometry();
		en::U32 index(coords.x + coords.y * mSize.x);
		if (index < mTileGrid.size())
		{
			return mTileGrid[index];
		}
	}
	return 0;
}

void GameMap::setTileId(const en::Vector2i& coords, en::U32 id)
{
	if (0 <= coords.x && 0 <= coords.y && coords.x < mSize.x && coords.y < mSize.y)
	{
		ensureUpdateGeometry();
		en::U32 index(coords.x + coords.y * mSize.x);
		mTileGrid[index] = id;
		if (mTileset != nullptr)
		{
			sf::Vertex* vertex(&mVertices[index * 4]);
			sf::Vector2f pos(mTileset->toPos(id));
			en::Vector2f texSize(mTileset->getTileSize());
			vertex[0].texCoords = pos;
			vertex[1].texCoords = sf::Vector2f(pos.x + texSize.x, pos.y);
			vertex[2].texCoords = sf::Vector2f(pos.x + texSize.x, pos.y + texSize.y);
			vertex[3].texCoords = sf::Vector2f(pos.x, pos.y + texSize.y);
		}
	}
}

const std::string& GameMap::getName() const
{
	return mName;
}

void GameMap::setName(const std::string& name)
{
	mName = name;
}

en::Tileset* GameMap::getTileset() const
{
	return mTileset;
}

void GameMap::setTileset(en::Tileset* tileset)
{
	if (mTileset != tileset)
	{
		if ((tileset != nullptr && mTileset == nullptr) || (tileset != nullptr && mTileset != nullptr && tileset->getTileSize() != mTileset->getTileSize()))
		{
			mGeometryUpdated = false;
		}
		mTileset = tileset;
	}
}

const en::Vector2i& GameMap::getSize() const
{
	return mSize;
}

void GameMap::setSize(const en::Vector2i& size)
{
	if (mSize != size)
	{
		mSize = size;
		mGeometryUpdated = false;
	}
}

const en::Vector2i& GameMap::getTileSize() const
{
	return mTileSize;
}

void GameMap::setTileSize(const en::Vector2i& tileSize)
{
	if (mTileSize != tileSize)
	{
		mTileSize = tileSize;
		mGeometryUpdated = false;
	}
}

void GameMap::setCollision(const en::Vector2i& coords, bool collide)
{
	if (coords.x >= 0 && coords.y >= 0 && coords.x < getSize().x && coords.y < getSize().y)
	{
		mCollisions[coords.x + coords.y * getSize().x] = collide;
	}
}

bool GameMap::collide(const en::Vector2i& coords)
{
	if (coords.x >= 0 && coords.y >= 0 && coords.x < getSize().x && coords.y < getSize().y)
	{
		return mCollisions[coords.x + coords.y * getSize().x];
	}
	return true;
}

void GameMap::render(sf::RenderTarget& target)
{
	if (mTileset != nullptr)
	{
		ensureUpdateRender();
		sf::RenderStates states;
		states.texture = &mTileset->getTexture();
		target.draw(mVertices, states);
	}
}

void GameMap::updateGeometry()
{
	if (mTileset == nullptr || mSize.x == 0 || mSize.y == 0 || mTileSize.x == 0 || mTileSize.y == 0)
	{
		return;
	}
	sf::Vector2f texSize(toSF(en::Vector2f(mTileset->getTileSize())));
	mVertices.resize(mSize.x * mSize.y * 4);
	mTileGrid.resize(mSize.x * mSize.y * 4); // TODO : Keep tile id already set in order
	en::Vector2i coords;
	for (coords.x = 0; coords.x < mSize.x; coords.x++)
	{
		for (coords.y = 0; coords.y < mSize.y; coords.y++)
		{
			// Get the position of the vertex
			en::Vector2 pos = coordsToWorld(coords);
			sf::Vertex* vertex = &mVertices[(coords.x + coords.y * mSize.x) * 4];
			en::F32 delta = texSize.y - (en::F32)mTileSize.y;
			vertex[0].position = sf::Vector2f(pos.x, pos.y - delta);
			vertex[1].position = sf::Vector2f(pos.x + mTileSize.x, pos.y - delta);
			vertex[2].position = sf::Vector2f(pos.x + mTileSize.x, pos.y + mTileSize.y);
			vertex[3].position = sf::Vector2f(pos.x, pos.y + mTileSize.y);
		}
	}
	mGeometryUpdated = true;
}

bool GameMap::isGeometryUpdated() const
{
	return mGeometryUpdated;
}

void GameMap::ensureUpdateGeometry()
{
	if (!mGeometryUpdated)
	{
		updateGeometry();
	}
}

void GameMap::updateRender()
{
	sf::Color color = sf::Color(255, 255, 255);
	en::U32 size = mVertices.getVertexCount();
	for (en::U32 i = 0; i < size; i++)
	{
		mVertices[i].color = color;
	}

	mRenderUpdated = true;
}

bool GameMap::isRenderUpdated() const
{
	return mRenderUpdated;
}

void GameMap::ensureUpdateRender()
{
	ensureUpdateGeometry();
	if (!mRenderUpdated)
	{
		updateRender();
	}
}
