#include "Client.h"

#define DEF_BUFLEN 512
#define DEF_SERV_PORT 8080
#define DEF_CLIENT_PORT 0
#define HOSTNAME_LEN 1024
#define WAIT_TIME 1
#define INIT_MSG 1
#define UPDATE_MSG 0
#define BTN_PRSS_MSG 2
#define DEF_CLIENT_ID -1

int playerCount = 1;

struct posMessage {
	int id;
	int msgType;
	glm::mat4 head;
	glm::mat4 left;
	glm::mat4 right;
};

struct initMessage {
	int id;
	int msgType;
};

struct btnMessage {
	int id;
	int msgType;
	int button;
};

Client::Client() {
	clientID = DEF_CLIENT_ID;

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
	player2 = new Player(DEF_CLIENT_ID);
	player3 = new Player(DEF_CLIENT_ID);
	
	addPlayer(player);
	addPlayer(player2);
	addPlayer(player3);

	btnSequence = "";
	_beginthread(recvMessages, 0, this);
}

Client::~Client() {
	free(message);
}

void Client::setId() {
	while (clientID == DEF_CLIENT_ID) {
		initMessage msg;
		msg.id = clientID;
		msg.msgType = INIT_MSG;
		
		char buffer[DEF_BUFLEN + 1];
		int numBytesRcvd;

		struct sockaddr_storage fromAddr; // Source address of server
										  // Set length of from address structure (in-out parameter)
		socklen_t fromAddrLen = sizeof(fromAddr);

		// Send the string to the server
		int numBytes = sendto(sock, (char *)&msg, sizeof(initMessage), 0,
			(struct sockaddr *)&server, sizeof(struct sockaddr_in));
		if (numBytes < 0)
			fprintf(stderr, "sendto() failed");

		numBytesRcvd = recvfrom(getSock(), buffer, sizeof(initMessage), 0,
			(struct sockaddr *) &fromAddr, &fromAddrLen);

		if (numBytesRcvd > 0) {
			initMessage initMsg;
			memcpy(&initMsg, buffer, sizeof(initMessage));

			if (initMsg.msgType == 1) {
				clientID = initMsg.id;
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
	updateServer(head, left, right);
	for (std::vector<Player*>::iterator it = (&players)->begin(); it < (&players)->end(); it++) {
		(*it)->update();
	}
}

void Client::updateServer(glm::mat4 head, glm::mat4 left, glm::mat4 right) {
	if (waitCounter > WAIT_TIME) {
		sendMessages(head, left, right);
		waitCounter = 0;
	}
	else {
		waitCounter++;
	}

}

void Client::sendMessages(glm::mat4 head, glm::mat4 left, glm::mat4 right) {

	posMessage msg;

	msg.id = clientID;
	msg.msgType = UPDATE_MSG;
	msg.head = head;
	msg.left = left;
	msg.right = right;

	// Send the string to the server
	int numBytes = sendto(sock, (char *)&msg, sizeof(posMessage), 0,
		(struct sockaddr *)&server, sizeof(struct sockaddr_in));
	if (numBytes < 0)
		fprintf(stderr, "sendto() failed");

	/*
	if (buttonHit) {
		memcpy(message, , btnSequence.length() * sizeof(char));
		// Send the string to the server
		int numBytes = sendto(sock, message, sizeof(, 0,
			(struct sockaddr *)&server, sizeof(struct sockaddr_in));
		if (numBytes < 0)
			fprintf(stderr, "sendto() failed");
		else if (numBytes != (int)strLen)
			fprintf(stderr, "sendto() error sent unexpected number of bytes");
	}*/
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
		numBytesRcvd = recvfrom(client->getSock(), buffer, sizeof(posMessage), 0,
			(struct sockaddr *) &fromAddr, &fromAddrLen);
		if (numBytesRcvd > 0 && numBytesRcvd == sizeof(posMessage)) {
			//fprintf(stderr, "got message: %d\n", numBytesRcvd);
			client->processMessage(&buffer[0]);
		}
	}
	//fprintf(stderr, "im out: %d\n", numBytesRcvd);

}

void Client::processMessage(char *buffer) {
	initMessage temp;
	memcpy(&temp, buffer, sizeof(initMessage));

	if (temp.msgType == INIT_MSG) {

	}
	else if (temp.msgType == UPDATE_MSG) {
		posMessage msg;
		memcpy(&msg, buffer, sizeof(posMessage));

		if (msg.id != clientID) {
			bool isUpdated = false;
			for (std::vector<Player*>::iterator it = (&players)->begin(); it < (&players)->end(); it++) {
				if ((*it)->playerID == msg.id) {
					(*it)->update(msg.head, msg.left, msg.right);
					isUpdated = true;
				}
			}
			if (!isUpdated) {
				if (playerCount == 1) {
					player2->update(msg.head, msg.left, msg.right);
					player2->playerID = msg.id;
				}
				else if (playerCount == 2) {
					player3->update(msg.head, msg.left, msg.right);
					player3->playerID = msg.id;
				}
			}
		}

		/*if (msg.id == clientID) {
			glm::mat4 head, left, right;

			head = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * msg.head;
			left = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * msg.left;
			right = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * msg.right;
			testplayer->update(head, left, right);
		}*/
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
		addr->sin_addr.S_un.S_un_b.s_b4 = (unsigned char)62
			;
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

