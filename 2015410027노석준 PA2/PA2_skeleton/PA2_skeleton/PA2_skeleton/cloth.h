
#pragma once

#include "spring.h"
#include "Node.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>

class mass_cloth
{
public:

	std::vector<Node *> nodes;
	std::vector<mass_spring *> spring;
	std::vector<Node*> faces;
	std::vector<vec2*> texture;

	int			size_x, size_y, size_z;
	double		dx, dy, dz;
	double		structural_coef;
	double		shear_coef;
	double		bending_coef;
	int			iteration_n;
	int			drawMode;
	double		sp1 = 3.141592/2;
	double		sp = 20*sin(sp1);
	

	mass_cloth()
	{ 	 
	}
	~mass_cloth()
	{ 
		for (int i = 0; i < nodes.size(); i++){ delete nodes[i]; }
		for (int i = 0; i < spring.size(); i++){ delete spring[i]; }
		nodes.clear();
		spring.clear();
		faces.clear();
	}
	enum DrawModeEnum{
		DRAW_MASS_NODES,
		DRAW_SPRINGS,
		DRAW_FACES
	};
 
public:
	void init()
	{
		Node *xp[50][50];
		//Basic Implements 1. Init Nodes and Shear and Structural Springs
		//Additional Implements 1. Init Bending Spring
		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y; j++) {
				xp[i][j] = new Node(vec3(dx*i, dy*j-25, dz));
				nodes.push_back(xp[i][j]);
			}
		}

		xp[0][49]->isFixed = true;
		xp[49][49]->isFixed = true;

		//structual
		for (int i = 0; i < size_x-1; i++) {
			for (int j = 0; j < size_y; j++) {
				mass_spring *sp = new mass_spring(xp[i][j], xp[i+1][j]);
				sp->spring_coef = structural_coef;
				spring.push_back(sp);
			}
		}
		for (int i = 0; i < size_x ; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				mass_spring *sp = new mass_spring(xp[i][j], xp[i][j+1]);
				sp->spring_coef = structural_coef;
				spring.push_back(sp);
			}
		}

		//shear
		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y-1; j++) {
				mass_spring *sp = new mass_spring(xp[i][j], xp[i + 1][j+1]);
				sp->spring_coef = shear_coef;
				spring.push_back(sp);
			}
		}
		for (int i = 0; i < size_x-1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				mass_spring *sp = new mass_spring(xp[i][j+1], xp[i+1][j]);
				sp->spring_coef = shear_coef;
				spring.push_back(sp);
			}
		}

		//bending
		for (int i = 0; i < size_x - 2; i++) {
			for (int j = 0; j < size_y; j++) {
				mass_spring *sp = new mass_spring(xp[i][j], xp[i + 2][j]);
				sp->spring_coef = bending_coef;
				spring.push_back(sp);
			}
		}
		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y - 2; j++) {
				mass_spring *sp = new mass_spring(xp[i][j], xp[i][j + 2]);
				sp->spring_coef = bending_coef;
				spring.push_back(sp);
			}
		}
			//Basic Implements 3-1. Generate Faces
		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				faces.push_back(xp[i][j + 1]);
				faces.push_back(xp[i+1][j]);
				faces.push_back(xp[i+1][j+1]);
			}
		}
		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				faces.push_back(xp[i][j + 1]);
				faces.push_back(xp[i][j]);
				faces.push_back(xp[i + 1][j]);
			}
		}
			//Additional Implements 4-2. Initialize Texture Coordinates	

		

		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				texture.push_back(new vec2(i,j+1));
				texture.push_back(new vec2(i+1, j));
				texture.push_back(new vec2(i+1, j+1));
			}
		}
		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				texture.push_back(new vec2(i, j + 1));
				texture.push_back(new vec2(i , j));
				texture.push_back(new vec2(i + 1, j));
			}
		}
	}
	
	void computeNormal()
	{
		//Basic Implements 3-2. Compute Vertex Normal

		vec3 normal1[50][50];
		vec3 normal2[50][50];

		for (int i = 0; i < size_x - 1; i++) {
			for (int j = 0; j < size_y - 1; j++) {
				vec3 ad, ac, ab;
				ad = nodes[50 * i + j]->position - nodes[50 * (i + 1) + j]->position;
				ac = nodes[50 * (i + 1) + j]->position - nodes[50 * i + (j + 1)]->position;
				ab = nodes[50 * (i + 1) + (j + 1)]->position - nodes[50 * i + (j + 1)]->position;
				normal1[i][j] = (ab.Cross(ac)) / (ab.Cross(ac)).length();
				normal2[i][j] = (ac.Cross(ad)) / (ac.Cross(ad)).length();
			}
		}

		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y; j++) {
				if (i > 0 && j == 0) { nodes[50 * i + j]->normal = (normal2[i - 1][j] + normal1[i - 1][j] + normal2[i][j])/ (normal2[i - 1][j] + normal1[i - 1][j] + normal2[i][j]).length(); }
				else if (i == 0 && j > 0) { nodes[50 * i + j]->normal = (normal2[i][j - 1] + normal1[i][j - 1] + normal1[i][j])/ (normal2[i][j - 1] + normal1[i][j - 1] + normal1[i][j]).length(); }
				else if (i == 49 && j > 0) { nodes[50 * i + j]->normal = (normal1[i - 1][j - 1] + normal1[i - 1][j] + normal2[i - 1][j])/ (normal1[i - 1][j - 1] + normal1[i - 1][j] + normal2[i - 1][j]).length(); }
				else if (i > 0 && j == 49) { nodes[50 * i + j]->normal = (normal1[i - 1][j - 1] + normal1[i][j - 1] + normal2[i][j - 1])/ (normal1[i - 1][j - 1] + normal1[i][j - 1] + normal2[i][j - 1]).length(); }
				else if (i == 0 && j == 0) { nodes[50 * i + j]->normal = (normal2[i][j])/ (normal2[i][j]).length(); }
				else if (i == 0 && j == 49) { nodes[50 * i + j]->normal = (normal2[i][j-1]+ normal1[i][j-1])/ (normal2[i][j - 1] + normal1[i][j - 1]).length(); }
				else if (i == 49 && j == 0) { nodes[50 * i + j]->normal = (normal2[i-1][j]+ normal1[i - 1][j])/ (normal2[i - 1][j] + normal1[i - 1][j]).length(); }
				else if (i == 49 && j == 49) { nodes[50 * i + j]->normal = (normal1[i-1][j-1])/ (normal1[i - 1][j - 1]).length(); }
				else { nodes[50 * i + j]->normal = (normal1[i - 1][j - 1] + normal1[i][j - 1] + normal2[i][j - 1] + normal1[i - 1][j] + normal2[i - 1][j] + normal2[i][j])/ (normal1[i - 1][j - 1] + normal1[i][j - 1] + normal2[i][j - 1] + normal1[i - 1][j] + normal2[i - 1][j] + normal2[i][j]).length(); }
			}
		}
		
		
	}
	
	void add_force(vec3 additional_force)
	{		 
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->add_force(additional_force);
		}
	}

	void compute_force(double dt, vec3 gravity)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->add_force(gravity * nodes[i]->mass);
		}
		/* Compute Force for all springs */
		for (int i = 0; i < spring.size(); i++)
		{
			spring[i]->internal_force(dt);
		}
	}

	void integrate(double dt)
	{
		/* integrate Nodes*/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->integrate(dt);
		}
	}
	
	void collision_response(vec3 ground)
	{
		//Basic Implements 4. Collision Check with ground
		//Additional Implements 2. Collision Check with Sphere
		//Additional Implements 3. Collision Check with Mesh Object
		
			for (int i = 0; i < 50 * 50; i++) {
				vec3 p = nodes[i]->position - vec3(25, 0, 40 * sin(sp1));
				float abs = p.length();
				p = p / abs;

				if (nodes[i]->position.y < -15+0.2 && nodes[i]->velocity.y < 0) {
					nodes[i]->velocity += -2 * (nodes[i]->velocity.y)*vec3(0,1,0);
				}
				if (abs < 8+1.2 && p.dot(nodes[i]->velocity) < 0) {
					nodes[i]->position = vec3(25, 0, 40 * sin(sp1)) + p * (8+1.2);
					nodes[i]->velocity += -1 * (p.dot(nodes[i]->velocity))*p;
				}

			}

	}

	void draw();
};