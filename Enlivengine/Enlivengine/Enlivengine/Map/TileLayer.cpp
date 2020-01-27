#include <Enlivengine/Map/TileLayer.hpp>

#include <Enlivengine/System/Compression.hpp>
#include <Enlivengine/System/String.hpp>

#include <sstream>

namespace en
{

TileLayer::TileLayer(Map& map)
	: LayerBase()
	, mMap(map)
	, mSize(0, 0)
	, mEncoding(EncodingType::Base64)
	, mCompression(CompressionType::Zlib)
	, mVertexArray()
{
}

const Vector2u& TileLayer::GetSize() const
{
	return mSize;
}

TileLayer::EncodingType TileLayer::GetEncoding() const
{
	return mEncoding;
}

TileLayer::CompressionType TileLayer::GetCompression() const
{
	return mCompression;
}

void TileLayer::SetTile(const Vector2u& tileCoords, U32 tileID)
{
	// TODO : SetTile
	ENLIVE_UNUSED(tileCoords);
	ENLIVE_UNUSED(tileID);
}

U32 TileLayer::GetTile(const Vector2u& tileCoords) const
{
	// TODO : GetTile
	ENLIVE_UNUSED(tileCoords);
	return 0;
}

bool TileLayer::Parse(ParserXml& parser)
{
	if (!LayerBase::Parse(parser))
	{
		return false;
	}

	parser.getAttribute("width", mSize.x);
	parser.getAttribute("height", mSize.y);

	if (parser.readNode("data"))
	{
		std::string attribStr = "base64";
		parser.getAttribute("encoding", attribStr);
		if (attribStr == "base64")
		{
			mEncoding = EncodingType::Base64;
		}
		else if (attribStr == "csv")
		{
			mEncoding = EncodingType::CSV;
		}
		else if (attribStr == "xml")
		{
			mEncoding = EncodingType::XML;
		}
		else
		{
			LogError(en::LogChannel::Graphics, 7, "Unknown encoding %s", attribStr.c_str());
			return false;
		}

		attribStr = "";
		parser.getAttribute("compression", attribStr);
		if (attribStr == "zlib")
		{
			mCompression = CompressionType::Zlib;
		}
		else if (attribStr == "gzip")
		{
			mCompression = CompressionType::Gzip;
		}
		else if (attribStr == "")
		{
			mCompression = CompressionType::None;
		}
		else
		{
			LogError(en::LogChannel::Graphics, 7, "Unknown compression %s", attribStr.c_str());
			return false;
		}

		if (mEncoding == EncodingType::Base64)
		{
			ParseBase64(parser);
		}
		else if (mEncoding == EncodingType::CSV)
		{
			ParseCsv(parser);
		}
		else if (mEncoding == EncodingType::XML)
		{
			ParseXml(parser);
		}
		else
		{
			assert(false);
		}

		parser.closeNode();
	}

	return true;
}

bool TileLayer::ParseBase64(ParserXml& parser)
{
	std::string data;
	parser.getValue(data);
	trim(data);

	if (!Compression::Decode64(data))
	{
		LogError(en::LogChannel::Graphics, 8, "Can't decode Base64");
		return false;
	}

	bool decompression = false;
	switch (mCompression)
	{
	case CompressionType::Zlib: decompression = Compression::Decompress(data); break;
	case CompressionType::Gzip: decompression = false; break; // TODO : Gzip decompression
	case CompressionType::None: decompression = true; break;
	default: decompression = false; break;
	}
	if (!decompression)
	{
		LogError(en::LogChannel::Graphics, 8, "Can't decompress %d", mCompression);
		return false;
	}

	std::vector<U8> byteVector;
	byteVector.reserve(mSize.x * mSize.y * 4);
	for (std::string::iterator i = data.begin(); i != data.end(); ++i)
	{
		byteVector.push_back(*i);
	}

	Vector2u coords(0, 0);
	for (std::size_t i = 0; i < byteVector.size() - 3; i += 4)
	{
		U32 gid = byteVector[i] | byteVector[i + 1] << 8 | byteVector[i + 2] << 16 | byteVector[i + 3] << 24;

		// TODO : Read Flip Flag

		SetTile(coords, gid);
		coords.x = (coords.x + 1) % mSize.x;
		if (coords.x == 0)
		{
			coords.y++;
		}
	}

	return true;
}

bool TileLayer::ParseCsv(ParserXml& parser)
{
	std::string data;
	parser.getValue(data);

	std::stringstream ss(data);

	U32 gid;
	Vector2u coords(0, 0);
	while (ss >> gid)
	{
		if (ss.peek() == ',')
		{
			ss.ignore();
		}

		// TODO : Read Flip Flag

		SetTile(coords, gid);
		coords.x = (coords.x + 1) % mSize.x;
		if (coords.x == 0)
		{
			coords.y++;
		}
	}

	return true;
}

bool TileLayer::ParseXml(ParserXml& parser)
{
	if (parser.readNode("tile"))
	{
		Vector2u coords(0, 0);
		do
		{
			U32 gid = 0;
			parser.getAttribute("gid", gid);

			// TODO : Read Flip Flag

			SetTile(coords, gid);
			coords.x = (coords.x + 1) % mSize.x;
			if (coords.x == 0)
			{
				coords.y++;
			}

		} while (parser.nextSibling("tile"));
		parser.closeNode();

		return true;
	}
	else
	{
		return false;
	}
}

} // namespace en