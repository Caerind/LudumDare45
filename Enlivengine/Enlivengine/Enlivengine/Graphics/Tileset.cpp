#include <Enlivengine/Graphics/Tileset.hpp>

#include <Enlivengine/System/ParserXml.hpp>
#include <filesystem>

namespace en
{

Tileset::Tileset() 
	: mTileSize()
	, mSpacing(0)
	, mMargin(0)
	, mTileCount(0)
	, mColumns(0)
	, mName()
	, mPath()
	, mImageSource()
	, mImageTransparent(Color::Transparent)
	, mTextureResourceID(InvalidResourceID)
	, mTexture()
	, mTextureChanged(true)
{
}

bool Tileset::LoadFromFile(const std::string& filename)
{
	ParserXml xml;
	if (!xml.loadFromFile(filename))
	{
		LogError(en::LogChannel::Graphics, 9, "Can't open tileset file at %s", filename.c_str());
		return false;
	}

	if (xml.readNode("tileset"))
	{
		xml.getAttribute("name", mName);
		xml.getAttribute("tilewidth", mTileSize.x);
		xml.getAttribute("tileheight", mTileSize.y);
		xml.getAttribute("tilecount", mTileCount);
		xml.getAttribute("columns", mColumns);
		xml.getAttribute("spacing", mSpacing);
		xml.getAttribute("margin", mMargin);

		mPath = std::filesystem::path(filename).remove_filename().string();

		if (xml.readNode("image"))
		{
			xml.getAttribute("source", mImageSource);
			
			if (xml.hasAttribute("trans"))
			{
				std::string transparentStr;
				xml.getAttribute("trans", transparentStr);
				mImageTransparent.fromString(transparentStr);
			}

			xml.closeNode();
		}

		xml.closeNode();
	}
	else
	{
		LogError(en::LogChannel::Graphics, 9, "Invalid tileset file at %s", filename.c_str());
		return false;
	}

	// TODO : Now find a way to deal with the Texture loading

	return true;
}

void Tileset::SetTileSize(const Vector2i& tileSize)
{
	mTileSize = tileSize;
}

void Tileset::SetSpacing(U32 spacing)
{
	mSpacing = spacing;
}

void Tileset::SetMargin(U32 margin)
{
	mMargin = margin;
}

void Tileset::SetTileCount(U32 tileCount)
{
	mTileCount = tileCount;
}

void Tileset::SetColumns(U32 columns)
{
	mColumns = columns;
}

void Tileset::SetName(const std::string& name)
{
	mName = name;
}

void Tileset::SetPath(const std::string& path)
{
	if (mPath != path)
	{
		mPath = path;
		mTextureChanged = true;
	}
}

void Tileset::SetImageSource(const std::string& source)
{
	if (mImageSource != source)
	{
		mImageSource = source;
		mTextureChanged = true;
	}
}

void Tileset::SetImageTransparent(const Color& transparent)
{
	if (mImageTransparent != transparent)
	{
		mImageTransparent = transparent;
		mTextureChanged = true;
	}
}

const Vector2i& Tileset::GetTileSize() const
{
	return mTileSize;
}

U32 Tileset::GetSpacing() const
{
	return mSpacing;
}

U32 Tileset::GetMargin() const
{
	return mMargin;
}

U32 Tileset::GetTileCount() const
{
	return mTileCount;
}

U32 Tileset::GetColumns() const
{
	return mColumns;
}

const std::string& Tileset::GetName() const
{
	return mName;
}

const std::string& Tileset::GetPath() const
{
	return mPath;
}

const std::string& Tileset::GetImageSource() const
{
	return mImageSource;
}

const Color& Tileset::GetImageTransparent() const
{
	return mImageTransparent;
}

void Tileset::SetTextureResourceID(ResourceID resourceID)
{
	if (mTextureResourceID != resourceID)
	{
		mTextureResourceID = resourceID;
		mTextureChanged = true;
	}
}

ResourceID Tileset::GetTextureResourceID() const
{
	return mTextureResourceID;
}

TexturePtr& Tileset::GetTexture()
{
	if (mTextureChanged)
	{
		const std::string filepath = mPath + mImageSource;
		if (mImageTransparent != Color::Transparent)
		{
			LogWarning(en::LogChannel::Graphics, 8, "%s : Transparent color for Tileset isn't supported yet -> Use alpha values", filepath.c_str());
		}

		mTexture = ResourceManager::GetInstance().Get<Texture>(mTextureResourceID);
		if (!mTexture.IsValid())
		{
			// TODO : The texture isn't loaded yet
			/*
			if (!mTexture->loadFromFile(filepath))
			{
				LogError(en::LogChannel::Graphics, 10, "Can't load tileset : %s", filename.c_str());
			}
			*/
		}
		
		mTextureChanged = false;
	}

	return mTexture;
}

Vector2f Tileset::ToPos(U32 tileId) const
{
	if (mColumns > 0)
	{
		return {
			(F32)((tileId % mColumns) * (mTileSize.x + mSpacing) + mMargin),
			(F32)((tileId / mColumns) * (mTileSize.y + mSpacing) + mMargin)
		};
	}
	return Vector2f::zero;
}

} // namespace en