#ifndef _CLIENT_H_
#define _CLIENT_H_


#include <vector>
#include <GL\glew.h>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Player.h"

using namespace std;

class Client {
public:
	Client();
	~Client();
	int getClientId();
	void updateMe(glm::mat4 head, glm::mat4 left, glm::mat4 right);
	vector<Player*> players;
	glm::mat4 getLocalPosition();
private:
	int clientID;
	void addPlayer(Player * newPlayer);	
	Player* player;
	glm::mat4 localPosition = glm::mat4(0.0f);
};

#endif