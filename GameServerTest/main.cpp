#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>
#include <atomic>

#include <SFML/Network.hpp>

constexpr unsigned short g_DefaultPort = 3456;

class Server
{
public:
	Server()
		: mSocket()
		, mSocketPort(g_DefaultPort)
		, mRunning(false)
	{
	}

	bool Run()
	{
		if (!Start())
		{
			return false;
		}

		while (IsRunning())
		{
			std::cout << "Ticks" << std::endl;

			sf::Packet receivedPacket;
			sf::IpAddress remoteAddress;
			unsigned short remotePort;
			while (mSocket.receive(receivedPacket, remoteAddress, remotePort) == sf::Socket::Done)
			{
				unsigned int a;
				receivedPacket >> a;
				std::cout << "Received \"" << a << "\" from " << remoteAddress.toString() << ":" << remotePort << std::endl;

				sf::Packet sendPacket;
				sendPacket << a + 1;
				mSocket.send(sendPacket, remoteAddress, remotePort);
				std::cout << "Send \"" << a + 1 << "\" to " << remoteAddress.toString() << ":" << remotePort << std::endl;
			}

			sf::sleep(sf::seconds(1.0f));
		}

		return true;
	}

	unsigned short GetSocketPort() const { return mSocketPort; }
	void SetSocketPort(unsigned short socketPort) { mSocketPort = socketPort; }

	bool IsRunning() const { return mRunning; }

private:
	bool Start()
	{
		std::cout << "Starting" << std::endl;
		mSocket.setBlocking(true);
		if (mSocket.bind(mSocketPort) != sf::Socket::Done)
		{
			std::cout << "Can't bind port : " << mSocketPort << std::endl;
			return false;
		}
		mSocket.setBlocking(false);
		mRunning = true;
		std::cout << "Started" << std::endl;
		return true;
	}

	void Stop()
	{
		std::cout << "Stopping" << std::endl;
		mRunning = false;
		std::cout << "Stopped" << std::endl;
	}

private:
	sf::UdpSocket mSocket;
	unsigned short mSocketPort;
	bool mRunning;
};

int main(int argc, char** argv)
{
	Server server;
	server.SetSocketPort((argc >= 2) ? static_cast<unsigned short>(std::atoi(argv[1])) : g_DefaultPort);
	server.Run();
	return 0;
}
