#pragma once

#include <Enlivengine/System/Time.hpp>
#include <Enlivengine/Application/ResourceManager.hpp>
#include <Enlivengine/Graphics/SFMLResources.hpp>
#include <Enlivengine/Math/Rect.hpp>

namespace en
{

class Animation : public Resource<Animation>
{
	public:
		struct Frame
		{
			Frame();
			Frame(const Rectu& rect, Time duration);

			Rectu rect;
			Time duration;
		};

		struct Clip
		{
			enum class Direction
			{
				Forward,
				Reverse,
				PingPong
			};

			Clip();
			Clip(const std::string& name, U32 from, U32 to, Direction direction);

			std::string name;
			U32 hashedName;
			U32 from;
			U32 to;
			Direction direction;
		};

	public:
		Animation();

		bool LoadFromFile(const std::string& filename);

		void AddFrame(const Animation::Frame& frame = Animation::Frame());
		void AddFrame(const Rectu& rect, Time duration);
		U32 GetFrameCount() const;
		Animation::Frame& GetFrame(U32 index);
		const Animation::Frame& GetFrame(U32 index) const;
		void RemoveFrame(U32 index);
		void RemoveAllFrames();
		
		void AddClip(const Animation::Clip& clip = Animation::Clip());
		void AddClip(const std::string& name, U32 from, U32 to, Animation::Clip::Direction direction);
		U32 GetClipCount() const;
		Animation::Clip& GetClip(U32 index);
		const Animation::Clip& GetClip(U32 index) const;
		void RemoveClip(U32 index);
		void RemoveAllClips();

		TexturePtr GetTexture();
		const TexturePtr& GetTexture() const;

	private:
		std::vector<Animation::Frame> mFrames;
		std::vector<Animation::Clip> mClips;
		TexturePtr mTexture;
};

using AnimationPtr = ResourcePtr<Animation>;

class AnimationLoader
{
public:
	AnimationLoader() = delete;
	~AnimationLoader() = delete;

	static ResourceLoader<Animation> FromFile(const std::string& filename)
	{
		return ResourceLoader<Animation>([&filename](Animation& r)
		{
			const bool result = r.LoadFromFile(filename);
			r.mFilename = (result) ? filename : "";
			return result;
		});
	}
};

} // namespace en
