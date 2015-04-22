#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include "utils.h"
#include <glm/gtx/matrix_transform_2d.hpp>

enum intersection_type { _2D_FIG, _3D_FIG };

struct Intersection {
	Intersection() {}
	Intersection(glm::vec3 point, intersection_type tp) :point(point), param(0.f), type(tp){}
	Intersection(glm::vec3 point, float param, intersection_type tp) :point(point), param(param), type(tp){}
	float param;
	glm::vec3 point;
	intersection_type type;
};


struct Intersector {
	std::vector<Intersection> m_intersections;
	Ray m_ray;
	Intersector(Ray &ray) : m_ray(ray) {}

	bool sort();
	bool push_back(Intersection &point);
	bool push_back(Intersector &points);
	void mul_on_mat(glm::mat4x4 &mat);
	unt size() { return m_intersections.size(); }
	Intersection &operator [] (unt pos){ return m_intersections[pos]; }

	bool csg_union(Intersector &in);
	bool csg_intersection(Intersector &in);
	bool csg_difference(Intersector &in);

	Intersection &getFirst();
};


class Object {
public:
	virtual Intersector intersect(Ray ray) = 0 {};
};

enum csg_type { CSG_NONE, CSG_UNION, CSG_INTERSECTION, CSG_DIFFERENCE };

class GroupObject : public Object  {
public:
	GroupObject() :m_mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), m_inv(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1){}
	virtual Intersector intersect(Ray ray);
	void push_back(std::shared_ptr<Object> obj);

	glm::mat4x4 m_mat;
	glm::mat4x4 m_inv;
	csg_type m_csg_type;

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