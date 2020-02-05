#include <Enlivengine/System/Compression.hpp>

#include <cstring>

#include <miniz/miniz.h>

namespace en
{

const std::string Compression::sBase64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              "abcdefghijklmnopqrstuvwxyz"
                                              "0123456789+/";

bool Compression::Encode64(std::string& data)
{
    U32 count = 0;
    U8 input_bytes[3] = { '\0', '\0', '\0' };
    U8 byte_array[4];
    std::string result;
    for (U32 i = 0; i < data.size(); ++i)
    {
        input_bytes[count++] = data[i];
        if (count == 3 || (i == data.size() - 1))
        {
            byte_array[0] = input_bytes[0] >> 2;
            byte_array[1] = ((input_bytes[0] & 0x3) << 4) | (input_bytes[1] >> 4);
            byte_array[2] = ((input_bytes[1] & 0xf) << 2) | (input_bytes[2] >> 6);
            byte_array[3] = input_bytes[2] & 0x3f;
            std::memset(input_bytes, '\0', 3);
			for (U32 j = 0; j < count + 1; j++)
			{
				result += sBase64Table[byte_array[j]];
			}
			if (count != 3)
			{
				for (U32 j = count; j < 3; ++j)
				{
					result += '=';
				}
			}
			else
			{
				count = 0;
			}
        }
    }
    data = result;
    return true;
}

bool Compression::Decode64(std::string& data)
{
    U32 count = 0;
    U8 input_bytes[4] = { '\0', '\0', '\0', '\0' };
    U8 byte_array[3];
    std::string result;
    for (U32 i = 0; i < data.size(); ++i)
    {
		if (IsBase64(data[i]))
		{
			input_bytes[count++] = static_cast<U8>(sBase64Table.find(data[i]));
		}
        if (count == 4 || data[i] == '=')
        {
            byte_array[0] = (input_bytes[0] << 2) | ((input_bytes[1] & 0x30) >> 4);
            byte_array[1] = ((input_bytes[1] & 0xf) << 4) | ((input_bytes[2] & 0x3c) >> 2);
            byte_array[2] = ((input_bytes[2] & 0x3) << 6) | input_bytes[3];
            std::memset(input_bytes, '\0', 4);
			for (U32 j = 0; j < count - 1; j++)
			{
				result += byte_array[j];
			}
			if (count != 4)
			{
				break;
			}
			else
			{
				count = 0;
			}
        }
    }
    data = result;
    return true;
}

bool Compression::CompressZlib(std::string& data)
{
    ENLIVE_UNUSED(data);
    return true;
}

bool Compression::DecompressZlib(std::string& data)
{
    if (data.size() == 0)
        return false;

    std::string output;
    output.resize(256);

    mz_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (unsigned char*)data.data();
    stream.avail_in = (mz_uint32)data.size();
    stream.next_out = (unsigned char*)output.data();
    stream.avail_out = (mz_uint32)output.size();

    if (mz_inflateInit(&stream))
    {
        return false;
    }

    for(;;)
    {
        int status = mz_inflate(&stream, MZ_SYNC_FLUSH);

        if (!stream.avail_out)
        {
            U32 oldSize = static_cast<U32>(output.size());
            output.resize(oldSize * 2);
            stream.next_out = (unsigned char*)(output.data() + oldSize);
            stream.avail_out = (mz_uint32)oldSize;
            return false;
        }

        if (status == MZ_STREAM_END)
        {
            break;
        }
        else if (status != MZ_OK)
        {
            return false;
        }
    }
    if (mz_inflateEnd(&stream) != Z_OK)
    {
        return false;
    }
    data = output;
    return true;
}

bool Compression::IsBase64(U8 c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

} // namespace en
