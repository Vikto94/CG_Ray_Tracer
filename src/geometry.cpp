#include "geometry.h"
#include <stdio.h>

void GroupObject::push_back(std::shared_ptr<Object> obj)
{
	m_scene.push_back(obj);
}

Intersector GroupObject::intersect(Ray ray)
{
	glm::vec4 vdir(ray.direction.x, ray.direction.y, ray.direction.z, 1);
	glm::vec4 vorigin(ray.origin.x, ray.origin.y, ray.origin.z, 1);
	vdir = m_mat * vdir;
	vorigin = m_mat * vorigin;
	Ray v;
	v.direction = glm::vec3(vdir.x, vdir.y, vdir.z);
	v.origin = glm::vec3(vorigin.x, vorigin.y, vorigin.z);
	Intersector res(v);
	for (unt i = 0; i < m_scene.size(); i++)
	{
		Intersector r = m_scene[i]->intersect(v);
		if (r.size())
			res.push_back(r[0]);
	}
	if (res.size())
	{
		glm::vec4 vec(res[0].point.x, res[0].point.y, res[0].point.z, 1.f);
		vec = m_inv * vec;
		Intersector r(ray);
		r.push_back(Intersection(glm::vec3(vec.x, vec.y, vec.z)));
		return r;
	}
	return res;
}

void IntersectionObject::push_back(std::shared_ptr<Object> obj)
{
	m_scene.push_back(obj);
}

Intersector IntersectionObject::intersect(Ray ray)
{
	glm::vec4 vdir(ray.direction.x, ray.direction.y, ray.direction.z, 1);
	glm::vec4 vorigin(ray.origin.x, ray.origin.y, ray.origin.z, 1);
	vdir = m_mat * vdir;
	vorigin = m_mat * vorigin;
	Ray v;
	v.direction = glm::vec3(vdir.x, vdir.y, vdir.z);
	v.origin = glm::vec3(vorigin.x, vorigin.y, vorigin.z);
	Intersector res(v);
	for (unt i = 0; i < m_scene.size(); i++)
	{
		Intersector r = m_scene[i]->intersect(v);
		if (r.size())
			res.push_back(r[0]);
	}
	if (res.size())
	{
		glm::vec4 vec(res[0].point.x, res[0].point.y, res[0].point.z, 1.f);
		vec = m_inv * vec;
		Intersector r(ray);
		r.push_back(Intersection(glm::vec3(vec.x, vec.y, vec.z)));
		return r;
	}
	return res;
}

bool Intersector::push_back(Intersection point)
{
	float k;
	if (abs(m_ray.direction.x) > abs(m_ray.direction.y) && abs(m_ray.direction.x) > abs(m_ray.direction.z))
		k = (point.point.x - m_ray.origin.x) / m_ray.direction.x;
	else if (abs(m_ray.direction.y) > abs(m_ray.direction.z))
		k = (point.point.y - m_ray.origin.y) / m_ray.direction.y;
	else
		k = (point.point.z - m_ray.origin.z) / m_ray.direction.z;

	if (k < 0)
		return false;

	if (m_intersections.empty())
	{
		point.param = k;
		m_intersections.push_back(point);
	}
	else
	{
		if (m_intersections[0].param > k)
		{
			m_intersections.pop_back();
			point.param = k;
			m_intersections.push_back(point);
		}
	}
	return true;
}

Intersector intersectPlane(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Ray ray) {
	Intersector res(ray);
	glm::vec3 edge1, edge2;
	glm::vec3 P, Q, T;
	glm::vec3 dir = ray.getDirection();
	glm::vec3 orig = ray.getOrigin();
	float det, inv_det, u, v;
	float t;
	edge1 = p1 - p0;
	edge2 = p2 - p0;
	P = glm::cross(dir, edge2);
	det = glm::dot(edge1, P);

	inv_det = 1.f / det;
	T = orig - p0;
	u = glm::dot(T, P) * inv_det;

	Q = glm::cross(T, edge1);
	v = glm::dot(dir, Q) * inv_det;

	t = glm::dot(edge2, Q) * inv_det;


	if (t > FLT_EPSILON) {
		Intersection inter;
		inter.param = t;
		inter.point = orig + t * dir;
		res.push_back(inter);
	}
	return res;
}

Intersector Plane::intersect(Ray ray) {
	return intersectPlane(p0, p1, p2, ray);
}

//Muller-Trumbore
Intersector Triangle::intersect(Ray ray) {
	Intersector res(ray);
	glm::vec3 edge1, edge2;
	glm::vec3 P, Q, T;
	glm::vec3 dir = ray.getDirection();
	glm::vec3 orig = ray.getOrigin();
	float det, inv_det, u, v;
	float t;
	edge1 = p1 - p0;
	edge2 = p2 - p0;
	P = glm::cross(dir, edge2);
	det = glm::dot(edge1, P);
	if (det > -FLT_EPSILON && det < FLT_EPSILON) {
		return res;
	}

	inv_det = 1.f / det;
	T = orig - p0;
	u = glm::dot(T, P) * inv_det;
	if (u < 0.f || u > 1.f) {
		return res;
	}

	Q = glm::cross(T, edge1);
	v = glm::dot(dir, Q) * inv_det;
	if (v < 0.f || u + v  > 1.f) {
		return res;
	}

	t = glm::dot(edge2, Q) * inv_det;

	if (t > FLT_EPSILON) {
		Intersection inter;
		inter.param = t;
		inter.point = orig + t * dir;
		res.push_back(inter);
	}
	return res;
}



Intersector Sphere::intersect(Ray ray) {
	Intersector res(ray);
	glm::vec3 dir = ray.getDirection();
	glm::vec3 orig = ray.getOrigin();
	float a = glm::dot(dir, dir);
	float b = glm::dot(dir, orig);
	float c = glm::dot(orig, orig) - radius * radius;

	float rad = b * b - a * c;
	if (rad < 0) {
		return res;
	}

	float x1 = (-b - sqrtf(rad)) / a;
	float x2 = (-b + sqrtf(rad)) / a;

	if (x1 > FLT_EPSILON) {
		Intersection inter;
		inter.param = x1;
		inter.point = orig + x1 * dir;
		res.push_back(inter);
	}

	if (x2 > FLT_EPSILON) {
		Intersection inter;
		inter.param = x2;
		inter.point = orig + x2 * dir;
		res.push_back(inter);
	}

	return res;
}

bool intcyl(const glm::vec3 &raybase, const glm::vec3 &raycos, float radius, glm::vec3 *in, glm::vec3 *out);

Intersector Cylinder::intersect(Ray ray) {
	Intersector res(ray), r1(ray), r2(ray);

	Intersection in, out;
	if (intcyl(ray.origin, ray.direction, radius, &(in.point), &(out.point)))
	{
		if (in.point.z <= height && in.point.z >= -height)
			res.push_back(in);
		if (out.point.z <= height && out.point.z >= -height)
			res.push_back(out);
		r1 = intersectPlane(glm::vec3(0, 0, height), glm::vec3(0, 1, height), glm::vec3(1, 0, height), ray);
		if (r1.size() != 0 && r1[0].point.x*r1[0].point.x + r1[0].point.y*r1[0].point.y <= radius*radius)
			res.push_back(r1[0]);
		r2 = intersectPlane(glm::vec3(0, 0, -height), glm::vec3(0, 1, -height), glm::vec3(1, 0, -height), ray);
		if (r2.size() != 0 && r2[0].point.x*r2[0].point.x + r2[0].point.y*r2[0].point.y <= radius*radius)
			res.push_back(r2[0]);
		return res;
	}

	return res;
}

bool intcon(const glm::vec3 &ori, const glm::vec3 &dir, float r_div_h, glm::vec3 *in, glm::vec3 *out);

Intersector Cone::intersect(Ray ray) {
	Intersector res(ray), r2(ray);

	Intersection in, out;
	if (intcon(ray.origin, ray.direction, radius / height, &(in.point), &(out.point)))
	{
		if (in.point.z <= 0 && in.point.z >= -height)
			res.push_back(in);
		if (out.point.z <= 0 && out.point.z >= -height)
			res.push_back(out);
		r2 = intersectPlane(glm::vec3(0, 0, -height), glm::vec3(0, 1, -height), glm::vec3(1, 0, -height), ray);
		if (r2.size() != 0 && r2[0].point.x*r2[0].point.x + r2[0].point.y*r2[0].point.y <= radius*radius)
			res.push_back(r2[0]);
		return res;
	}

	return res;
}


int   SolveP4(double *x, double a, double b, double c, double d);	// solve equation x^4 + a*x^3 + b*x^2 + c*x + d = 0 by Dekart-Euler method

Intersector Torus::intersect(Ray ray) {
	Intersector res(ray);
	glm::vec3 dir = glm::normalize(ray.direction);

	double sv = glm::dot(ray.origin, dir), a = 4 * sv;
	double r1_deg_2 = radius*radius, r2_deg_2 = tube_radius*tube_radius, s_deg_2 = glm::dot(ray.origin, ray.origin);
	double b = 2 * (s_deg_2 + r1_deg_2 - r2_deg_2) - 4 * r1_deg_2 * (1 - dir.z*dir.z) + 4 * sv * sv;
	double c = 8 * r1_deg_2 * ray.origin.z * dir.z + a * (s_deg_2 - r1_deg_2 - r2_deg_2);
	double d = pow(ray.origin.x, 4) + pow(ray.origin.y, 4) + pow(ray.origin.z, 4) + (r1_deg_2 - r2_deg_2)*(r1_deg_2 - r2_deg_2) +
		2 * (ray.origin.x*ray.origin.y + ray.origin.z*(r1_deg_2 - r2_deg_2) + 
		(pow(ray.origin.x, 2) + pow(ray.origin.y, 2))*(s_deg_2 - r1_deg_2 - r2_deg_2));

	double x[4];

	int n = SolveP4(x, a, b, c, d);
	switch (n)
	{
	case 4:
		res.push_back(Intersection(ray.origin + dir * float(x[3])));
		res.push_back(Intersection(ray.origin + dir * float(x[2])));
	case 2:
		res.push_back(Intersection(ray.origin + dir * float(x[1])));
		res.push_back(Intersection(ray.origin + dir * float(x[0])));
		break;
	}

	return res;
}


ObjModel::ObjModel(std::string obj_file)
{
	std::vector<glm::vec3> verts;
	FILE *f = fopen(obj_file.data(), "r");
	char c1, c2, c3;
	while (fscanf(f, "%c", &c1))
	{
		if (isspace(c1) && c1 != ' ' && c1 != '\t')
		{
			check:
			if (fscanf(f, "%c%c", &c2, &c3) < 2)
				return;
			if (c3 != ' ')
				continue;
			if (c2 == 'v')
			{
				float vx, vy, vz;
				unt n = fscanf(f, "%f%f%f", &vx, &vy, &vz);
				if (n >= 3)
				{
					verts.push_back(glm::vec3(vx, vy, vz));
				}
			}
			if (c2 == 'f')
			{
				unt v1, v2, v3, v4;
				unt n = fscanf(f, "%u", &v1);
				if (n >= 1)
				{
					while (fscanf(f, "%c", &c1))
						if (c1 == ' ')
							break;
					n = fscanf(f, "%u", &v2);
					if (n >= 1)
					{
						while (fscanf(f, "%c", &c1))
							if (c1 == ' ')
								break;
						n = fscanf(f, "%u", &v3);
						if (n >= 1)
						{
							m_triangles.push_back(Triangle(verts[v1-1], verts[v2-1], verts[v3-1]));
							while (fscanf(f, "%c", &c1))
								if (isspace(c1))
									if (c1 == ' ')
										break;
									else
										goto check;

							n = fscanf(f, "%u", &v4);
							if (n >= 1)
							{
								m_triangles.push_back(Triangle(verts[v1 - 1], verts[v4 - 1], verts[v3 - 1]));
							}
						}
					}
				}
			}
		}
	}
}

Intersector ObjModel::intersect(Ray ray)
{
	Intersector res(ray);
	for (unt i = 0; i < m_triangles.size(); i++)
	{
		Intersector r = m_triangles[i].intersect(ray);
		if (r.size())
			res.push_back(r[0]);
	}
	return res;
}

