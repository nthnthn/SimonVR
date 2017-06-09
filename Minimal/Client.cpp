#include "Client.h"

Client::Client() {
	clientID = 0;
	player = new Player(clientID);
	addPlayer(player);
	localPosition[3] = {0.0f, 0.0f, -1.0f, 0.0f};
}

Client::~Client() {}

int Client::getClientId() {
	return clientID;
}

void Client::addPlayer(Player * newPlayer) {
	players.push_back(newPlayer);
}

void Client::updateMe(glm::mat4 head, glm::mat4 left, glm::mat4 right) {
	player->update(head, left, right);
}

glm::mat4 Client::getLocalPosition() {
	return localPosition;
}