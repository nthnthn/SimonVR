#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_

#include <GL\glew.h>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <vector>

class SkyBox
{
public:
	SkyBox(int);
	~SkyBox();
	void draw(GLuint shader, const glm::mat4 &, const glm::mat4 &);
	void drawColor(GLuint shader, const glm::mat4 &, const glm::mat4 &);
	void scale(float scalefactor);
	void translate(glm::vec3 transfactor);
	void setScale(float scalefactor);
	void update(glm::mat4 newToWorld);
	GLfloat color[4];
	void setColor(glm::vec4 newcolor);

private:
	GLuint textId;
	glm::mat4 toWorld;
	GLfloat angle;
	GLuint VBO, VBO2, VBO3, VAO, EBO;
	unsigned int numOfIndices;
	std::string left, right, up, down, back, front;
	std::vector<const GLchar *> faces;
	unsigned char* loadPPM(const char* filename, int& width, int& height);
	void scale(glm::vec3 scalarVector);
	bool isInitialized;
};

#endif