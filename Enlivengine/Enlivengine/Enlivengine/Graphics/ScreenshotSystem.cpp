#include <Enlivengine/Graphics/ScreenshotSystem.hpp>

#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/System/DateTime.hpp>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <sstream>

namespace en
{

ScreenshotSystem::ScreenshotSystem()
	: mScreenshotPath("")
{
}

bool ScreenshotSystem::screenshot(const sf::RenderWindow& window)
{
	sf::Texture texture;
	texture.create(window.getSize().x, window.getSize().y);
	texture.update(window);

	return saveTexture(texture);
}

const std::string& ScreenshotSystem::getScreenshotPath() const
{
	return mScreenshotPath;
}

void ScreenshotSystem::setScreenshotPath(const std::string& screenshotPath)
{
	mScreenshotPath = screenshotPath;
}

bool ScreenshotSystem::saveTexture(const sf::Texture& texture) const
{
	const std::string filename = getScreenshotPath() + getScreenshotName() + getScreenshotFormat();
	if (texture.copyToImage().saveToFile(filename))
	{
		LogInfo(en::LogChannel::Graphics, 5, "Screenshot saved to : %s", filename.c_str());
		return true;
	}
	else
	{
		LogError(en::LogChannel::Graphics, 8, "Can't save screenshot to : %s", filename.c_str());
		return false;
	}
}

std::string ScreenshotSystem::getScreenshotName()
{
	DateTime date;
	I32 y, m, d;
	I32 H, M, S;
	date.getDate(y, m, d);
	date.getTime(H, M, S);
	std::ostringstream oss;
	oss << y << "-" << m << "-" << d << "_" << H << "-" << M << "-" << S;
	return oss.str();
}

std::string ScreenshotSystem::getScreenshotFormat()
{
	return std::string(".png");
}

} // namespace en