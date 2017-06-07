#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <GL\glew.h>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Skybox.h"

class Player {
	public:
		Player(int);
		~Player();
		int playerID;
		void draw(GLuint shaderProgram, const glm::mat4 &projection, const glm::mat4 &modelview, int clientID);
		void update(glm::mat4 headmat, glm::mat4 left, glm::mat4 right);


	private:
		SkyBox *head;
		SkyBox *leftHand;
		SkyBox *rightHand;

};

#endif