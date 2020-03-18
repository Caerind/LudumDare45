#pragma once

#include <type_traits>

#if defined(ENLIVE_STRINGIFY_EX)
	#undef ENLIVE_STRINGIFY_EX
#endif
#define ENLIVE_STRINGIFY_EX(x) #x

#if defined(ENLIVE_STRINGIFY)
	#undef ENLIVE_STRINGIFY
#endif
#define ENLIVE_STRINGIFY(x) ENLIVE_STRINGIFY_EX(x)

#if defined(ENLIVE_CONCAT_EX)
	#undef ENLIVE_CONCAT_EX
#endif
#define ENLIVE_CONCAT_EX(a, b) a ## b

#if defined(ENLIVE_CONCAT)
	#undef ENLIVE_CONCAT
#endif
#define ENLIVE_CONCAT(a, b) ENLIVE_CONCAT_EX(a, b)

#if defined(ENLIVE_VERSION_ENCODE)
	#undef ENLIVE_VERSION_ENCODE
#endif
#define ENLIVE_VERSION_ENCODE(major, minor, revision) (((major) * 1000000) + ((minor) * 1000) + (revision))

#if defined(ENLIVE_VERSION_DECODE_MAJOR)
	#undef ENLIVE_VERSION_DECODE_MAJOR
#endif
#define ENLIVE_VERSION_DECODE_MAJOR(version) ((version) / 1000000)

#if defined(ENLIVE_VERSION_DECODE_MINOR)
	#undef ENLIVE_VERSION_DECODE_MINOR
#endif
#define ENLIVE_VERSION_DECODE_MINOR(version) (((version) % 1000000) / 1000)

#if defined(ENLIVE_VERSION_DECODE_REVISION)
	#undef ENLIVE_VERSION_DECODE_REVISION
#endif
#define ENLIVE_VERSION_DECODE_REVISION(version) ((version) % 1000)

#if defined(ENLIVE_UNUSED)
#undef ENLIVE_UNUSED
#endif
#define ENLIVE_UNUSED(var) (void(var))

#if defined(ENLIVE_ASSERT_BASE_OF)
#undef ENLIVE_ASSERT_BASE_OF
#endif
#define ENLIVE_ASSERT_BASE_OF(Base, Derived) static_assert(std::is_base_of<Base, Derived>::value, #Derived " must be a descendant of " #Base);

#if defined(ENLIVE_ARRAY_SIZE)
#undef ENLIVE_ARRAY_SIZE
#endif
#define ENLIVE_ARRAY_SIZE(arr) static_cast<en::U32>(sizeof(arr)/sizeof(arr[0]))

#if defined(ENLIVE_SIZE_OF)
#undef ENLIVE_SIZE_OF
#endif
#define ENLIVE_SIZE_OF(type) static_cast<en::U32>(sizeof(type))

#if defined(ENLIVE_ALIGN_OF)
#undef ENLIVE_ALIGN_OF
#endif
#define ENLIVE_ALIGN_OF(type) static_cast<en::U32>(alignof(type))

#if defined(ENLIVE_OFFSET_OF)
#undef ENLIVE_OFFSET_OF
#endif
#define ENLIVE_OFFSET_OF(type, member) static_cast<en::U32>(offsetof(type, member))

#if defined(ENLIVE_SIZE_OF_MEMBER)
#undef ENLIVE_SIZE_OF_MEMBER
#endif
#define ENLIVE_SIZE_OF_MEMBER(type, member) static_cast<en::U32>(en::GetSizeOfMember(&type::member))
namespace en
{
	template <typename T, typename MemberType>
	constexpr std::size_t GetSizeOfMember(MemberType T::* member)
	{
		return sizeof(MemberType);
	}
} // namespace en