#include <Enlivengine/System/Compression.hpp>

#include <doctest/doctest.h>

#include <iostream>

DOCTEST_TEST_CASE("Compression determinism")
{
	const std::string myStr = "AZERTYUIOPQSDFGHJKLMwxcvbn1234567890";
	std::cout << myStr << std::endl;

	std::string myStrEncode = myStr;
	bool encode64Valid = en::Compression::Encode64(myStrEncode);
	std::cout << myStrEncode << std::endl;
	DOCTEST_CHECK(myStr != myStrEncode);
	bool decode64Valid = en::Compression::Decode64(myStrEncode);
	DOCTEST_CHECK(encode64Valid);
	DOCTEST_CHECK(decode64Valid);
	DOCTEST_CHECK(myStr == myStrEncode);

	std::string myStrCompress = myStr;
	bool compressValid = en::Compression::CompressZlib(myStrCompress);
	std::cout << myStrCompress << std::endl;
	DOCTEST_CHECK(myStr != myStrCompress);
	bool decompressValid = en::Compression::DecompressZlib(myStrCompress);
	DOCTEST_CHECK(compressValid);
	DOCTEST_CHECK(decompressValid);
	DOCTEST_CHECK(myStr == myStrCompress);
}

DOCTEST_TEST_CASE("Encode64")
{
	const std::string inputStr = "AzertyuiopQsdfghjklmWxcvbn1234567890";
	const std::string outputStr = "QXplcnR5dWlvcFFzZGZnaGprbG1XeGN2Ym4xMjM0NTY3ODkw";
	std::string input = inputStr;
	bool encode64Valid = en::Compression::Encode64(input);
	DOCTEST_CHECK(encode64Valid);
	DOCTEST_CHECK(outputStr == input);
}

DOCTEST_TEST_CASE("Decode64")
{
}

DOCTEST_TEST_CASE("Zlib Compression")
{
	const std::string inputStr = "AzertyuiopQsdfghjklmWxcvbn1234567890";
	const std::string outputStr = "eJxzrEotKqkszcwvCCxOSUvPyMrOyQ2vSC5LyjM0MjYxNTO3sDQAAAeaDM0=";
	std::string input = inputStr;
	bool zlibCompression = en::Compression::CompressZlib(input);
	DOCTEST_CHECK(zlibCompression);
	DOCTEST_CHECK(outputStr == input);
}

DOCTEST_TEST_CASE("Zlib Decompression")
{

}