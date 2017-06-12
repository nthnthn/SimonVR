#include "Client.h"

#define DEF_BUFLEN 512
#define DEF_SERV_PORT 8080
#define DEF_CLIENT_PORT 0
#define HOSTNAME_LEN 1024
#define WAIT_TIME 1
#define INIT_MSG 1
#define UPDATE_MSG 0
#define BTN_PRSS_MSG 2

Client::Client() {
	clientID = 9;

	/***********SOCKET STUFF *********************/


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
		exit(0);
	}

	// Set up the socket and server/client info
	memset((void *)&server, '\0', sizeof(struct sockaddr_in));
	setAddrInfo(&server, false);

	memset((void *)&client, '\0', sizeof(struct sockaddr_in));
	setAddrInfo(&client, true);

	sock = getSocket(AF_INET, SOCK_DGRAM);

	message = (char *)malloc(DEF_BUFLEN * sizeof(char));

	setId();

	/***********END OF SOCKET STUFF *********************/

	player = new Player(clientID);
	addPlayer(player);

	btnSequence = "";
	_beginthread(recvMessages, 0, this);
}

Client::~Client() {
	free(message);
}

void Client::setId() {
	while (clientID == 9) {
		string init = to_string(clientID) + " " + to_string(INIT_MSG);
		init += '\0';
		int numBytesRcvd;
		char buffer[DEF_BUFLEN + 1];

		struct sockaddr_storage fromAddr; // Source address of server
										  // Set length of from address structure (in-out parameter)
		socklen_t fromAddrLen = sizeof(fromAddr);

		//memcpy(message, (char *)&init[0], btnSequence.length() * sizeof(char));
		// Send the string to the server
		int numBytes = sendto(sock, &init[0], strlen(&init[0]), 0,
			(struct sockaddr *)&server, sizeof(struct sockaddr_in));
		if (numBytes < 0)
			fprintf(stderr, "sendto() failed");

		numBytesRcvd = recvfrom(getSock(), buffer, DEF_BUFLEN, 0,
			(struct sockaddr *) &fromAddr, &fromAddrLen);

		if (numBytesRcvd > 0) {
			stringstream ss;
			ss << buffer;

			int playerID;
			ss >> playerID;

			int check;
			ss >> check;

			if (check == 1) {
				clientID = playerID;
			}
		}
		Sleep(1000);
	}

	fprintf(stderr, "%d", clientID);
}

int Client::getClientId() {
	return clientID;
}

SOCKET Client::getSock() {
	return sock;
}

void Client::addPlayer(Player * newPlayer) {
	players.push_back(newPlayer);
}

void Client::updateMe(glm::mat4 head, glm::mat4 left, glm::mat4 right) {
	player->update(head, left, right);
	convertPlayerToString(head, left, right, message);
	updateServer();
}

void Client::updateServer() {
	if (waitCounter > WAIT_TIME) {
		sendMessages();
		waitCounter = 0;
	}
	else {
		waitCounter++;
	}

}

void Client::sendMessages() {
	size_t strLen = strlen(message);
	if (strLen > DEF_BUFLEN) // Check input length
		fprintf(stderr, "string too long");

	// Send the string to the server
	int numBytes = sendto(sock, message, strLen, 0,
		(struct sockaddr *)&server, sizeof(struct sockaddr_in));
	if (numBytes < 0)
		fprintf(stderr, "sendto() failed");
	else if (numBytes != (int)strLen)
		fprintf(stderr, "sendto() error sent unexpected number of bytes");

	if (buttonHit) {
		memcpy(message, &btnSequence[0], btnSequence.length() * sizeof(char));
		// Send the string to the server
		int numBytes = sendto(sock, message, strLen, 0,
			(struct sockaddr *)&server, sizeof(struct sockaddr_in));
		if (numBytes < 0)
			fprintf(stderr, "sendto() failed");
		else if (numBytes != (int)strLen)
			fprintf(stderr, "sendto() error sent unexpected number of bytes");
	}
}

void recvMessages(void *arg) {
	Client *client = (Client *)arg;
	
	struct sockaddr_storage fromAddr; // Source address of server
									  // Set length of from address structure (in-out parameter)
	socklen_t fromAddrLen = sizeof(fromAddr);

	int numBytesRcvd;
	char buffer[DEF_BUFLEN + 1];
	
	// When numBytesRcvd == 0 that means 0 bytes were received, therefore
	// no more messages
	while(true) {
		numBytesRcvd = recvfrom(client->getSock(), buffer, DEF_BUFLEN, 0,
			(struct sockaddr *) &fromAddr, &fromAddrLen);
		if (numBytesRcvd > 0) {
			//fprintf(stderr, "got message: %d\n", numBytesRcvd);
			client->processMessage(&buffer[0]);
		}
	}
	//fprintf(stderr, "im out: %d\n", numBytesRcvd);

}

void Client::processMessage(char *buffer) {
	//fputs(buffer, stdout);
	stringstream ss;
	ss << buffer;

	int playerID;
	ss >> playerID;

	if (playerID == clientID) return;

	int messageType;
	ss >> messageType;

	if (messageType == INIT_MSG) {
		return;
	}

	glm::mat4 headmat;
	glm::mat4 leftmat;
	glm::mat4 rightmat;
	glm::mat4 *mat;
	for (std::vector<Player*>::iterator it = players.begin(); it < players.end(); it++) {
		if ((*it)->playerID == playerID) {
			//Found the player to update
			for (int i = 0; i < 3; i++) {
				if (i == 0) {
					mat = &headmat;
				}
				else if (i == 1) {
					mat = &leftmat;
				}
				else {
					mat = &rightmat;
				}

				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < 4; k++) {
						ss >> (*mat)[j][k];
					}
				}
			}
			(*it)->update(headmat, leftmat, rightmat);
		}
		else { //Player to update not found
			Player* newPlayer = new Player(playerID);
			addPlayer(newPlayer);
			return;
		}
	}
}

glm::mat4 Client::getLocalPosition() {
	return localPosition;
}

SOCKET Client::getSocket(int family, int type) {
	SOCKET sock = socket(family, type, 0);
	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "Unable to create socket");
		WSACleanup();
		exit(0);
	}
	return sock;
}

void Client::setAddrInfo(sockaddr_in *addr, bool isClient) {
	addr->sin_family = AF_INET;
	int port = (isClient) ? DEF_CLIENT_PORT : DEF_SERV_PORT;
	addr->sin_port = htons(port);
	char hostName[HOSTNAME_LEN];
	struct hostent *currHost;

	if (isClient) {
		gethostname(hostName, sizeof(hostName));
		currHost = gethostbyname(hostName);
		if (!currHost) {
			fprintf(stderr, "Unable to get hostname");
			WSACleanup();
			exit(0);
		}

		addr->sin_addr.S_un.S_un_b.s_b1 = currHost->h_addr_list[0][0];
		addr->sin_addr.S_un.S_un_b.s_b2 = currHost->h_addr_list[0][1];
		addr->sin_addr.S_un.S_un_b.s_b3 = currHost->h_addr_list[0][2];
		addr->sin_addr.S_un.S_un_b.s_b4 = currHost->h_addr_list[0][3];
	}
	else {
		addr->sin_addr.S_un.S_un_b.s_b1 = (unsigned char)128;
		addr->sin_addr.S_un.S_un_b.s_b2 = (unsigned char)54;
		addr->sin_addr.S_un.S_un_b.s_b3 = (unsigned char)70;
		addr->sin_addr.S_un.S_un_b.s_b4 = (unsigned char)63;
	}
}

void Client::bindSocket(SOCKET sock, sockaddr_in *server) {
	int bnd = bind(sock, (const sockaddr *)server, sizeof((*server)
		));
	if (bnd == -1) {
		fprintf(stderr, "Unable to bind to socket");
		cleanUp(sock);
	}
}

void Client::cleanUp(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
	exit(0);
}

void Client::convertPlayerToString(glm::mat4 head, glm::mat4 left, glm::mat4 right, char* myStr) {
	std::string str = to_string(clientID) + " 0 ";
	glm::mat4 matrix;
	for (int l = 0; l < 3; l++) {
		switch (l) {
		case 0:
			matrix = head;
			//str += "";
			break;
		case 1:
			matrix = left;
			str += " ";
			break;
		case 2:
			matrix = right;
			str += " ";
			break;
		default: break;
		}

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::string num = std::to_string(matrix[i][j]);
				for (int k = 0; k < 6; k++) {
					str += num[k];
				}
				if (i != 3 || j != 3) {
					str += " ";
				}
			}
		}
		//str += ",";
	}
	//str += "";
	str += '\0';
	memcpy(myStr, &str[0], str.length() * sizeof(char));
}
