#ifndef _CLIENT_H_
#define _CLIENT_H_


#include <vector>
#include <GL\glew.h>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Player.h"


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <process.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

class Client {
public:
	Client();
	~Client();
	int getClientId();
	void updateMe(glm::mat4 head, glm::mat4 left, glm::mat4 right);
	vector<Player*> players;
	glm::mat4 getLocalPosition();
	SOCKET getSock();
	void processMessage(char *buffer);

	//Player * testplayer;

private:
	int clientID;
	int redC = 0;
	int greenC = 0;
	int blueC = 0;
	int yellowC = 0;

	int serverRedC = -1;
	int serverGreenC = -1;
	int serverBlueC = -1;
	int serverYellowC = -1;

	void addPlayer(Player * newPlayer);	
	Player* player;
	glm::mat4 localPosition = glm::mat4(0.0f);
	void cleanUp(SOCKET sock);
	SOCKET getSocket(int family, int type);
	void setAddrInfo(sockaddr_in *addr, bool isClient);
	void bindSocket(SOCKET sock, sockaddr_in *server);
	void convertPlayerToString(glm::mat4 head, glm::mat4 left,
		glm::mat4 right, char* myStr);
	void updateServer();
	void sendMessages();
	void setId();

	WSADATA wsaData;
	int iResult;
	sockaddr_in client, server;
	SOCKET sock;
	char *message;
	bool buttonHit;
	string btnSequence;

	int waitCounter = 0;
};

void recvMessages(void *arg);

#endif