#pragma once

#include <string>

namespace sf
{
	class RenderWindow;
	class Texture;
}

namespace en
{

class ScreenshotSystem
{
public:
	ScreenshotSystem();

	bool screenshot(const sf::RenderWindow& window);

	const std::string& getScreenshotPath() const;
	void setScreenshotPath(const std::string& screenshotPath);

private:
	bool saveTexture(const sf::Texture& texture) const;

	static std::string getScreenshotName();
	static std::string getScreenshotFormat();
	
private:
	std::string mScreenshotPath;
};

} // namespace en