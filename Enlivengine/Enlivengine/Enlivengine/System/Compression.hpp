#pragma once

#include <string>
#include <Enlivengine/System/PrimitiveTypes.hpp>

namespace en
{

class Compression
{
	public:
		static bool Encode64(std::string& data);
		static bool Decode64(std::string& data);
		static bool Compress(std::string& data);
		static bool Decompress(std::string& data);

		static bool Compress64(std::string& data);
		static bool Decompress64(std::string& data);

	private:
		static const std::string sBase64Table;

		static bool IsBase64(U8 c);
};

} // namespace en
