#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>

#include <SFML/Network.hpp>

const sf::IpAddress g_DefaultServerAddress("2.4.113.64");
constexpr unsigned short g_DefaultServerPort(3456);

class Client
{
public:
	Client()
		: mSocket()
		, mServerAddress(g_DefaultServerAddress)
		, mServerPort(g_DefaultServerPort)
		, mRunning(false)
		, mClientID(static_cast<unsigned int>(rand() % 10000))
		, mDelayClock()
	{
		std::cout << "ClientID: " << mClientID << std::endl;
	}

	bool Start()
	{
		std::cout << "Starting..." << std::endl;
		mSocket.setBlocking(true);
		if (mSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
		{
			std::cout << "Can't find an available port" << std::endl;
			return false;
		}
		mSocket.setBlocking(false);
		mRunning = true;
		std::cout << "Bind on port " << mSocket.getLocalPort() << std::endl;
		std::cout << "Started" << std::endl;
		return true;
	}

	void Update()
	{
		if (IsRunning())
		{
			if (mDelayClock.getElapsedTime() >= sf::seconds(0.9f))
			{
				mDelayClock.restart();

				sf::Packet sendPacket;
				sendPacket << mClientID;
				mSocket.send(sendPacket, mServerAddress, mServerPort);

				std::cout << "Send packet to the server" << std::endl;
			}

			sf::Packet receivedPacket;
			sf::IpAddress remoteAddress;
			unsigned short remotePort;
			while (mSocket.receive(receivedPacket, remoteAddress, remotePort) == sf::Socket::Done)
			{
				unsigned int response;
				receivedPacket >> response;

				std::cout << "Server returned : " << response << std::endl;
			}
		}
	}

	void Stop()
	{
		std::cout << "Stopping..." << std::endl;
		mRunning = false;
		std::cout << "Stopped" << std::endl;
	}

	const sf::IpAddress& GetServerAddress() const { return mServerAddress; }
	void SetServerAddress(const sf::IpAddress& serverAddress) { mServerAddress = serverAddress; }
	unsigned short GetServerPort() const { return mServerPort; }
	void SetServerPort(unsigned short serverPort) { mServerPort = serverPort; }

	bool IsRunning() const { return mRunning; }

private:
	sf::UdpSocket mSocket;
	sf::IpAddress mServerAddress;
	unsigned short mServerPort;
	bool mRunning;
	unsigned int mClientID;
	sf::Clock mDelayClock;
};

int main(int argc, char** argv)
{
	std::srand(time(0));

	Client client;
	//client.SetServerAddress();
	//client.SetServerPort();
	if (!client.Start())
	{
		return -1;
	}

	sf::Clock clock;
	while (clock.getElapsedTime() < sf::seconds(20.0f))
	{
		client.Update();
		sf::sleep(sf::seconds(0.1f));
	}

	if (client.IsRunning())
	{
		client.Stop();
	}

	return 0;
}
