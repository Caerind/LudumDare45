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
		static bool CompressZlib(std::string& data);
		static bool DecompressZlib(std::string& data);

	private:
		static const std::string sBase64Table;

		static bool IsBase64(U8 c);
};

} // namespace en
