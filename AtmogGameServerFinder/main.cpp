#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>

#include <SFML/Network.hpp>

const unsigned short serverPort = 3456;
const unsigned short sleepingTimeMS = 50;
const unsigned short timeOutS = 10;
const unsigned int intervalTimeS = 5 * 60;

enum class MessageIDs : unsigned int
{
	HostUpdate = 2101996,
	HostLeft = 31031999,
	ClientJoin = 15061995,
	ClientRandomJoin = 2041996,
	ServerInfo = 23011996,
	InvalidRequest = 0
};

struct GameServer
{
	GameServer() {}

	GameServer(const sf::IpAddress& hostAddress, unsigned short hostPort, unsigned int gameID, unsigned int partyID, bool privateConnection)
		: mLastPacketTime(sf::Time::Zero)
		, mHostAddress(hostAddress)
		, mHostPort(hostPort)
		, mGameID(gameID)
		, mPartyID(partyID)
		, mPrivateConnection(privateConnection)
	{
	}

	GameServer(const GameServer& other)
		: mLastPacketTime(other.mLastPacketTime)
		, mHostAddress(other.mHostAddress)
		, mHostPort(other.mHostPort)
		, mGameID(other.mGameID)
		, mPartyID(other.mPartyID)
		, mPrivateConnection(other.mPrivateConnection)
	{
	}

	sf::Time mLastPacketTime;
	sf::IpAddress mHostAddress;
	unsigned short mHostPort;
	unsigned int mGameID;
	unsigned int mPartyID;
	bool mPrivateConnection;
};

std::ofstream logFile;
char dateString[100];
char timeString[100];
void PrintCurrentDateTime()
{
	time_t curr_time;
	tm* curr_tm;
	time(&curr_time);
	curr_tm = localtime(&curr_time);

	strftime(dateString, 50, "%d/%m/%y", curr_tm);
	strftime(timeString, 50, "%T", curr_tm);

	if (logFile.is_open())
	{
		logFile << "[" << dateString << "](" << timeString << "): ";
	}
	std::cout << "[" << dateString << "](" << timeString << "): ";
}
void Log(const std::string& line)
{
	PrintCurrentDateTime();
	if (logFile.is_open())
	{
		logFile << line << std::endl;
	}
	std::cout << line << std::endl;
}

int main(int argc, char** argv)
{
	std::srand(std::time(0));

	unsigned short port = serverPort;
	if (argc >= 2)
	{
		port = (unsigned short)std::atoi(argv[1]);
	}
	sf::Time sleepingTime = sf::milliseconds(sleepingTimeMS);
	sf::Time timeOut = sf::seconds(timeOutS);

	logFile.open("logfile.txt");

	bool running = false;

	sf::UdpSocket socket;
	if (socket.bind(port) != sf::Socket::Done)
	{
		Log("Unable to bind socket on port : " + std::to_string(port));
		running = false;
	}
	else
	{
		Log("Socket bind on port : " + std::to_string(port));
		running = true;
	}
	socket.setBlocking(false);

	sf::Clock clockInterval;
	sf::Time intervalTime = sf::seconds(intervalTimeS);

	sf::Clock clock;
	unsigned int maxServersReached = 0;
	std::vector<GameServer> gameServers;
	while (running)
	{
		// Incoming packets
		sf::Packet receivePacket;
		sf::IpAddress remoteAddress;
		unsigned short remotePort;
		if (socket.receive(receivePacket, remoteAddress, remotePort) == sf::Socket::Done)
		{
			unsigned int gameID = 0;
			unsigned int messageID = 0;
			receivePacket >> gameID >> messageID;

			switch (static_cast<MessageIDs>(messageID))
			{
			case MessageIDs::HostUpdate:
			{
				unsigned int partyID = 0;
				bool privateConnection = true;
				receivePacket >> partyID >> privateConnection;

				int gameServerIndex = -1;
				for (size_t i = 0; i < gameServers.size(); ++i)
				{
					if (gameServers[i].mHostAddress == remoteAddress && gameServers[i].mGameID == gameID && gameServers[i].mPartyID == partyID)
					{
						gameServerIndex = i;
						break;
					}
				}

				if (gameServerIndex >= 0)
				{
					gameServers[gameServerIndex].mLastPacketTime = sf::Time::Zero;
				}
				else
				{
					gameServers.resize(gameServers.size() + 1);
					gameServers.back() = GameServer(remoteAddress, remotePort, gameID, partyID, privateConnection);
					maxServersReached = std::max(maxServersReached, (unsigned int)gameServers.size());
					Log(" -> New game server " + remoteAddress.toString() + ":" + std::to_string(remotePort) + " with PartyID:" + std::to_string(partyID));
					Log("Current servers : " + std::to_string(gameServers.size()));
					Log("Max servers : " + std::to_string(maxServersReached));
				}

			} break;
			case MessageIDs::HostLeft:
			{
				unsigned int partyID = 0;
				receivePacket >> partyID;

				int gameServerIndex = -1;
				for (size_t i = 0; i < gameServers.size(); ++i)
				{
					if (gameServers[i].mHostAddress == remoteAddress && gameServers[i].mGameID == gameID && gameServers[i].mPartyID == partyID)
					{
						gameServerIndex = i;
						break;
					}
				}

				if (gameServerIndex >= 0)
				{
					Log("-> Server left " + gameServers[gameServerIndex].mHostAddress.toString() + ":" + std::to_string(gameServers[gameServerIndex].mHostPort));
				}

			} break;
			case MessageIDs::ClientJoin:
			{
				unsigned int partyID = 0;
				receivePacket >> partyID;

				int gameServerIndex = -1;
				for (size_t i = 0; i < gameServers.size(); ++i)
				{
					if (gameServers[i].mGameID == gameID && gameServers[i].mPartyID == partyID)
					{
						gameServerIndex = i;
						break;
					}
				}

				if (gameServerIndex >= 0)
				{
					sf::Packet sendPacket;
					sendPacket << gameServers[gameServerIndex].mGameID;
					sendPacket << static_cast<unsigned int>(MessageIDs::ServerInfo);
					sendPacket << gameServers[gameServerIndex].mPartyID;
					sendPacket << gameServers[gameServerIndex].mHostAddress.toInteger();
					sendPacket << gameServers[gameServerIndex].mHostPort;

					Log("Client " + remoteAddress.toString() + ":" + std::to_string(remotePort) + " joining (" + std::to_string(gameID) + "/" + std::to_string(partyID) + ")");

					socket.setBlocking(true);
					socket.send(sendPacket, remoteAddress, remotePort);
					socket.setBlocking(false);
				}
				else
				{
					sf::Packet sendPacket;
					sendPacket << gameID;
					sendPacket << static_cast<unsigned int>(MessageIDs::InvalidRequest);

					socket.setBlocking(true);
					socket.send(sendPacket, remoteAddress, remotePort);
					socket.setBlocking(false);

					Log("Client " + remoteAddress.toString() + ":" + std::to_string(remotePort) + " entered invalid GameID or PartyID (" + std::to_string(gameID) + "/" + std::to_string(partyID) + ")");
				}

			} break;
			case MessageIDs::ClientRandomJoin:
			{
				std::vector<unsigned int> gameServerIndexes;
				for (size_t i = 0; i < gameServers.size(); ++i)
				{
					if (gameServers[i].mGameID == gameID && !gameServers[i].mPrivateConnection)
					{
						gameServerIndexes.push_back(i);
					}
				}

				if (gameServerIndexes.size() >= 0)
				{
					unsigned int randomIndex = static_cast<unsigned int>(rand() % static_cast<unsigned int>(gameServerIndexes.size()));
					unsigned int gameServerIndex = gameServerIndexes[randomIndex];

					sf::Packet sendPacket;
					sendPacket << gameServers[gameServerIndex].mGameID;
					sendPacket << static_cast<unsigned int>(MessageIDs::ServerInfo);
					sendPacket << gameServers[gameServerIndex].mPartyID;
					sendPacket << gameServers[gameServerIndex].mHostAddress.toInteger();
					sendPacket << gameServers[gameServerIndex].mHostPort;

					Log("Client " + remoteAddress.toString() + ":" + std::to_string(remotePort) + " randomly joining (" + std::to_string(gameServers[gameServerIndex].mGameID) + "/" + std::to_string(gameServers[gameServerIndex].mPartyID) + ")");

					socket.setBlocking(true);
					socket.send(sendPacket, remoteAddress, remotePort);
					socket.setBlocking(false);
				}
				else
				{
					sf::Packet sendPacket;
					sendPacket << gameID;
					sendPacket << static_cast<unsigned int>(MessageIDs::InvalidRequest);

					socket.setBlocking(true);
					socket.send(sendPacket, remoteAddress, remotePort);
					socket.setBlocking(false);

					Log("Currently no public game for GameID " + std::to_string(gameID) + " for client " + remoteAddress.toString() + ":" + std::to_string(remotePort));
				}

			} break;
			default: 
			{
				Log("Invalid MessageID (" + std::to_string(messageID) + ") from " + remoteAddress.toString());
			} break;
			}
		}

		// Update game servers last packet time
		for (size_t i = 0; i < gameServers.size();)
		{
			if (gameServers[i].mLastPacketTime >= timeOut)
			{
				Log("-> Server timed out " + gameServers[i].mHostAddress.toString());
				gameServers[i] = gameServers.back();
				gameServers.resize(gameServers.size() - 1);
			}
			else
			{
				gameServers[i].mLastPacketTime = clock.getElapsedTime();
				i++;
			}
		}
		clock.restart();

		// Poor CPU need to rest
		sf::sleep(sleepingTime);

		if (clockInterval.getElapsedTime() >= intervalTime)
		{
			Log("-");
			clockInterval.restart();
		}
	}

	return 0;
}
