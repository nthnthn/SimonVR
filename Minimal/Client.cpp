#include "Client.h"

#define DEF_BUFLEN 512
#define DEF_SERV_PORT 8080
#define DEF_CLIENT_PORT 0
#define HOSTNAME_LEN 1024

Client::Client() {
	clientID = 0;
	player = new Player(clientID);
	addPlayer(player);
	localPosition[3] = {0.0f, 0.0f, -1.0f, 0.0f};

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

	/***********END OF SOCKET STUFF *********************/

	btnSequence = "";

}

Client::~Client() {
	free(message);
}

int Client::getClientId() {
	return clientID;
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
	sendMessages();
	recvMessages();
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

void Client::recvMessages() {
	struct sockaddr_storage fromAddr; // Source address of server
									  // Set length of from address structure (in-out parameter)
	socklen_t fromAddrLen = sizeof(fromAddr);

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
		addr->sin_addr.S_un.S_un_b.s_b1 = (unsigned char)127;
		addr->sin_addr.S_un.S_un_b.s_b2 = (unsigned char)0;
		addr->sin_addr.S_un.S_un_b.s_b3 = (unsigned char)0;
		addr->sin_addr.S_un.S_un_b.s_b4 = (unsigned char)1;
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
	std::string str = "id:" + to_string(clientID) + "{";
	glm::mat4 matrix;
	for (int l = 0; l < 3; l++) {
		switch (l) {
		case 0:
			matrix = head;
			str += "h{";
			break;
		case 1:
			matrix = left;
			str += "l{";
			break;
		case 2:
			matrix = right;
			str += "r{";
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
					str += ",";
				}
			}
		}
		str += "}";
	}
	str += "}";
	str += '\0';
	memcpy(myStr, &str[0], str.length() * sizeof(char));
}
