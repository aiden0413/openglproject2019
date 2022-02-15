#include "Node.h"

#include <glew.h>
#include <GL/glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GL/stb_image.h>
#include "cloth.h"
using namespace std;
unsigned int texture;

void Node::draw()
{
	glDisable(GL_LIGHTING);
	glColor3f(0.97, 0.95, 0.15);
	glPointSize(2.0);

	glBegin(GL_POINTS);	
	glVertex3f(getPosX(), getPosY(), getPosZ());
	glEnd();
	glEnable(GL_LIGHTING);
}

void mass_spring::draw()
{
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);

 	glBegin(GL_LINES);
	glVertex3f(p1->position.x, p1->position.y, p1->position.z);
	glVertex3f(p2->position.x, p2->position.y, p2->position.z);
  	glEnd();	 
	glEnable(GL_LIGHTING);

}

void initTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char *data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void mass_cloth::draw()
{	
	switch (drawMode)
	{
	case DRAW_MASS_NODES:
		glDisable(GL_LIGHTING);
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->draw();
		glEnable(GL_LIGHTING);
		break;
	case DRAW_SPRINGS:
		glDisable(GL_LIGHTING);
		for (int i = 0; i < spring.size(); i++)
			spring[i]->draw();
		glEnable(GL_LIGHTING);
		break;
	case DRAW_FACES:
		//Basic Implements 3-3. Draw Call for Cloth
		//Additional Implements 4-3. Texture Coordinate Mapping
		initTexture();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);
		glColor3f(1.0, 1.0, 1.0);
		for (int i = 0; i < faces.size(); i++) {
			glNormal3f(faces[i]->normal.x, faces[i]->normal.y, faces[i]->normal.z);
			glTexCoord2f(texture[i]->x/50, texture[i]->y/50);
			glVertex3f(faces[i]->position.x, faces[i]->position.y, faces[i]->position.z);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		break;
	default:
		break;
	}
	glPopMatrix();
}

