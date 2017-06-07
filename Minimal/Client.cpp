#include "Client.h"

Client::Client() {
	clientID = 0;
	player = new Player(clientID);
	addPlayer(player);
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