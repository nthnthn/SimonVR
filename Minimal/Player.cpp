#include "Player.h"

Player::Player(int id) {
	playerID = id;
	head = new SkyBox(0);
	leftHand = new SkyBox(1);
	rightHand = new SkyBox(2);
}

Player::~Player() {}

void Player::draw(GLuint shaderProgram, const glm::mat4 &projection, const glm::mat4 &modelview, int clientID) {

	if (clientID != playerID) {
		head->draw(shaderProgram, projection, modelview);
	}
	leftHand->draw(shaderProgram, projection, modelview);
	rightHand->draw(shaderProgram, projection, modelview);
}

void Player::update(glm::mat4 headmat, glm::mat4 left, glm::mat4 right) {
	head->update(headmat);
	head->scale(.2f);
	leftHand->update(left);
	leftHand->scale(.1f);
	rightHand->update(right);
	rightHand->scale(.1f);
}