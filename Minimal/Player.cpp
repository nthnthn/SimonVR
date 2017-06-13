#include "Player.h"

Player::Player(int id) {
	playerID = id;
	head = new SkyBox(0);
	leftHand = new SkyBox(1);
	rightHand = new SkyBox(2);
	headNext = new SkyBox(0);
	leftHandNext = new SkyBox(1);
	rightHandNext = new SkyBox(2);
	isUpdating = false;
}

Player::~Player() {}

void Player::draw(GLuint shaderProgram, const glm::mat4 &projection, const glm::mat4 &modelview, int clientID, bool isRendering) {
	isUpdating = true;
	if (clientID != playerID) {
		head->draw(shaderProgram, projection, modelview);
	}
	leftHand->draw(shaderProgram, projection, modelview);
	rightHand->draw(shaderProgram, projection, modelview);
	isUpdating = false;
}

void Player::update(glm::mat4 headmat, glm::mat4 leftmat, glm::mat4 rightmat) {
	while (isUpdating) {
		//fprintf(stderr, "updating");
	}
	headNext->update(headmat);
	headNext->scale(.2f);
	leftHandNext->update(leftmat);
	leftHandNext->scale(.1f);
	rightHandNext->update(rightmat);
	rightHandNext->scale(.1f);
}

void Player::update() {
	head = headNext;
	leftHand = leftHandNext;
	rightHand = rightHandNext;
}