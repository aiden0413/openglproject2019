
#pragma once
 
#include <iostream>
#include "Node.h"
using namespace std;

class mass_spring{
public:
	double		spring_coef;
	double		damping_coef;
	Node	*p1;
	Node	*p2;
	double		initial_length;
 
public:
 
	mass_spring(Node *p1, Node *p2)
	{
		damping_coef = 5.0;
		this->p1 = p1;
		this->p2 = p2;
		init();
	}

	void init()
	{
		vec3 S_length = (p2->position - p1->position);
		initial_length = S_length.length();
	}

	void internal_force(double dt)
	{
		//Basic Implements 2-1. Compute Spring Force
		//add hook_force and damping force
		
		vec3 dir = p2->position - p1->position;
		float abs = dir.length();
		dir = dir / abs;
		vec3 force2 = (-1 * spring_coef * (1 - abs) * dir) + (damping_coef * dir.dot(p2->velocity - p1->velocity) * dir);
		vec3 force1 = force2 * (-1);
		p1->add_force(force2);
		p2->add_force(force1);
	}
	void draw();

};