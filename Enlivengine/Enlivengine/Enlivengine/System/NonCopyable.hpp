#pragma once

namespace en
{

class NonCopyable
{
private:
    NonCopyable() = delete;
    NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

} // namespace en