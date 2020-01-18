#include <Enlivengine/Graphics/Tileset.hpp>

namespace en
{

Tileset::Tileset() 
	: mFirstGid(1)
	, mTileSize()
	, mSpacing(0)
	, mMargin(0)
	, mTileCount(0)
	, mColumns(0)
	, mRelativePath("")
	, mImageSource()
	, mImageTransparent(Color::Transparent)
	, mTexture(nullptr)
	, mImageChanged(true)
{
}

void Tileset::setFirstGid(TileId id)
{
	mFirstGid = id;
}

void Tileset::setTileSize(const Vector2i& tileSize)
{
	mTileSize = tileSize;
}

void Tileset::setSpacing(U32 spacing)
{
	mSpacing = spacing;
}

void Tileset::setMargin(U32 margin)
{
	mMargin = margin;
}

void Tileset::setTileCount(U32 tileCount)
{
	mTileCount = tileCount;
}

void Tileset::setColumns(U32 columns)
{
	mColumns = columns;
}

void Tileset::setRelativePath(const std::string& path)
{
	if (mRelativePath != path)
	{
		mRelativePath = path;
		mImageChanged = true;
	}
}

void Tileset::setImageSource(const std::string& source)
{
	if (mImageSource != source)
	{
		mImageSource = source;
		mImageChanged = true;
	}
}

void Tileset::setImageTransparent(const Color& transparent)
{
	mImageTransparent = transparent;
}

TileId Tileset::getFirstGid() const
{
	return mFirstGid;
}

const Vector2i& Tileset::getTileSize() const
{
	return mTileSize;
}

U32 Tileset::getSpacing() const
{
	return mSpacing;
}

U32 Tileset::getMargin() const
{
	return mMargin;
}

U32 Tileset::getTileCount() const
{
	return mTileCount;
}

U32 Tileset::getColumns() const
{
	return mColumns;
}

const std::string& Tileset::getRelativePath() const
{
	return mRelativePath;
}

const std::string& Tileset::getImageSource() const
{
	return mImageSource;
}

const Color& Tileset::getImageTransparent() const
{
	return mImageTransparent;
}

sf::Texture& Tileset::getTexture()
{
	if (mImageChanged || mTexture == nullptr)
	{
		if (mTexture == nullptr)
		{
			mTexture = std::make_unique<sf::Texture>();
		}
		assert(mTexture != nullptr);

		const std::string filename = mRelativePath + mImageSource;
		if (mImageTransparent != Color::Transparent)
		{
			sf::Image image;
			if (!image.loadFromFile(filename))
			{
				LogError(en::LogChannel::Graphics, 10, "Can't load tileset : %s", filename.c_str());
			}
			image.createMaskFromColor(toSF(mImageTransparent));
			mTexture->loadFromImage(image);
		}
		else
		{
			if (!mTexture->loadFromFile(mRelativePath + mImageSource))
			{
				LogError(en::LogChannel::Graphics, 10, "Can't load tileset : %s", filename.c_str());
			}
		}
		
		mImageChanged = false;
	}
	assert(mTexture != nullptr);
	return *(mTexture.get());
}

bool Tileset::hasId(TileId gid)
{
	return (mFirstGid <= gid && gid < mFirstGid + mTileCount);
}

sf::Vector2f Tileset::toPos(TileId gid)
{
	if (!hasId(gid))
	{
		return sf::Vector2f();
	}
	gid -= mFirstGid;
	if (mColumns > 0)
	{
		return { (F32)((gid % mColumns) * (mTileSize.x + mSpacing) + mMargin), (F32)((gid / mColumns) * (mTileSize.y + mSpacing) + mMargin) };
	}
	return sf::Vector2f();
}

} // namespace en