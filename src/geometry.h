#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include "utils.h"
#include <glm/gtx/matrix_transform_2d.hpp>

struct Intersection {
	Intersection() {}
	Intersection(glm::vec3 point) :point(point), param(0.f){}
	Intersection(glm::vec3 point, float param) :point(point), param(param){}
	float param;
	glm::vec3 point;
};


struct Intersector {
	std::vector<Intersection> m_intersections;
	Ray m_ray;
	Intersector(Ray ray) : m_ray(ray) {}

	bool push_back(Intersection point);
	unt size() { return m_intersections.size(); }
	Intersection &operator [] (unt pos){ return m_intersections[pos]; }
};


class Object {
public:
	virtual Intersector intersect(Ray ray) = 0 {};
};


class GroupObject : public Object  {
public:
	GroupObject() :m_mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), m_inv(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1){}
	virtual Intersector intersect(Ray ray);
	void push_back(std::shared_ptr<Object> obj);

	glm::mat4x4 m_mat;
	glm::mat4x4 m_inv;

private:
	std::vector<std::shared_ptr<Object>> m_scene;
};


class IntersectionObject : public Object  {
public:
	IntersectionObject() : m_mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), m_inv(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1){}
	virtual Intersector intersect(Ray ray);
	void push_back(std::shared_ptr<Object> obj);
	glm::mat4x4 m_mat;
	glm::mat4x4 m_inv;

private:
	std::vector<std::shared_ptr<Object>> m_scene;
};


class Plane : public Object {
public:
	Plane(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2) : p1(p1), p2(p2), p0(p0) {}
	virtual Intersector intersect(Ray ray);
private:
	glm::vec3 p0, p1, p2;
};


class Triangle : public Object {
public:
	Triangle(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2) : p1(p1), p2(p2), p0(p0) {}
	virtual Intersector intersect(Ray ray);
private:
	glm::vec3 p0, p1, p2;
};

class Sphere : public Object {
public:
	Sphere(float r) : radius(r) {};
	virtual Intersector intersect(Ray ray);
private:
	float radius;
};



class Cylinder : public Object {
public:
	Cylinder(float r, float h) : radius(r), height(h*0.5f) {};
	virtual Intersector intersect(Ray ray);
private:
	float radius, height;
};

class Cone : public Object {
public:
	Cone(float r, float h) : radius(r), height(h) {};
	virtual Intersector intersect(Ray ray);
private:
	float radius, height;
};

class Torus : public Object {
public:
	Torus(float r, float rt) : radius(r), tube_radius(rt) {};
	virtual Intersector intersect(Ray ray);
private:
	float radius, tube_radius;
};

class ObjModel : public Object {
public:
	ObjModel(std::string obj_file);

	Intersector intersect(Ray ray);
private:
	std::vector<Triangle> m_triangles;
};


#endif