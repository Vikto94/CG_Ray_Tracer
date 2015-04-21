#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <glm/glm.hpp>

typedef unsigned int unt;
typedef unsigned char unc;

struct Color
{
	Color();
	Color(unc r, unc g, unc b) : red(r), green(g), blue(b) {};
	unc red;
	unc green;
	unc blue;
};

struct Ray {
public:
	glm::vec3 getOrigin() {
		return origin;
	}
	glm::vec3 getDirection() {
		return direction;
	}
	void setOrigin(glm::vec3 vec) {
		origin = vec;
	}
	void setDirection(glm::vec3 vec) {
		direction = vec;
	}

	glm::vec3 origin;
	glm::vec3 direction;
};


float len(glm::vec3 vec);


#endif