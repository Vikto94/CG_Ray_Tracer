#include "raytracer.h"
#include <glm/gtc/matrix_transform.hpp>

static const float pi_div_180 = 3.1415926535 / 180.f;

void RayTracer::ScanCamera(FILE *f)
{
	char buf[1001];
	float fov_x, fov_y;
	float pos_x, pos_y, pos_z;
	float dir_x, dir_y, dir_z;

	for (unt i = 0; i < 4; i++)
	{
		if (!fgets(buf, 1000, f))
			return;
		std::string row(buf);
		unt pos = 0;
		if ((pos = row.find("fov_x:")) != std::string::npos)
		{
			pos = row.find(":") + 1;
			fov_x = std::stof(row.substr(pos, row.size() - pos)) / 2;
		}
		else if ((pos = row.find("fov_y:")) != std::string::npos)
		{
			pos = row.find(":") + 1;
			fov_y = std::stof(row.substr(pos, row.size() - pos)) / 2;
		}
		else if ((pos = row.find("position:")) != std::string::npos)
		{
			unt pos_start_x = row.find("x:") + 2, pos_end_x = row.find(",");
			unt pos_start_y = row.find("y:") + 2, pos_end_y = row.substr(pos_start_y, row.size() - pos_start_y).find(",");
			unt pos_start_z = row.find("z:") + 2, pos_end_z = row.find("}");
			pos_x = std::stof(row.substr(pos_start_x, pos_end_x));
			pos_y = std::stof(row.substr(pos_start_y, pos_end_y));
			pos_z = std::stof(row.substr(pos_start_z, pos_end_z));
		}
		else if ((pos = row.find("orientation:")) != std::string::npos)
		{
			unt pos_start_x = row.find("h:") + 2, pos_end_x = row.find(",");
			unt pos_start_y = row.find("p:") + 2, pos_end_y = row.substr(pos_start_y, row.size() - pos_start_y).find(",");
			unt pos_start_z = row.find("r:") + 2, pos_end_z = row.find("}");
			dir_x = std::stof(row.substr(pos_start_x, pos_end_x)) * pi_div_180;
			dir_y = std::stof(row.substr(pos_start_y, pos_end_y)) * pi_div_180;
			dir_z = std::stof(row.substr(pos_start_z, pos_end_z)) * pi_div_180;
		}
	}
	
	float tg_y = tan(fov_y * pi_div_180);
	float tg_x = tan(fov_x * pi_div_180);
	float dist_x = width / tg_x;
	float dist_y = height / tg_y;
	dist = (dist_x < dist_y) ? dist_x : dist_y;
	width_shift = (width - dist * tg_x) * 0.5;
	height_shift = (height - dist * tg_y) * 0.5;
	width = dist * tg_x;
	height = dist * tg_y;

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-pos_x, -pos_y, -pos_z));
	view = glm::rotate(view, -dir_x, glm::vec3(1, 0, 0));
	view = glm::rotate(view, -dir_y, glm::vec3(0, 1, 0));
	view = glm::rotate(view, -dir_z, glm::vec3(0, 0, 1));
	m_scene.m_mat = view;
	m_scene.m_inv = inverse(m_scene.m_mat);
}

std::shared_ptr<Object> RayTracer::ParseNode(FILE *f, unt indent, csg_type tp)
{
	std::shared_ptr<GroupObject> node(new GroupObject());
	node->m_csg_type = tp;
	static bool flag = false;
	static char buf[1001];
	while (true)
	{
		if (!flag)
		{
			if (!fgets(buf, 1000, f))
				return node;
			if (buf[0] == '#')
				continue;
		}
		std::string row(buf);
		if (row.find("-") < indent)
		{
			flag = true;
			return node;
		}

		flag = false;
		if (row.find("- lcs:") != std::string::npos)
		{
			unt pos_start_x = row.find("x:") + 2, pos_end_x = row.find(",");
			unt pos_start_y = row.find("y:") + 2, pos_end_y = row.substr(pos_start_y, row.size() - pos_start_y).find(",");
			unt pos_start_z = row.find("z:") + 2, pos_end_z = row.substr(pos_start_z, row.size() - pos_start_z).find(",");
			unt pos_start_h = row.find("h:") + 2, pos_end_h = row.substr(pos_start_h, row.size() - pos_start_h).find(",");
			unt pos_start_p = row.find("p:") + 2, pos_end_p = row.substr(pos_start_p, row.size() - pos_start_p).find(",");
			unt pos_start_r = row.find("r:") + 2, pos_end_r = row.substr(pos_start_r, row.size() - pos_start_r).find(",");
			unt pos_start_sx = row.find("sx:") + 3, pos_end_sx = row.substr(pos_start_sx, row.size() - pos_start_sx).find(",");
			unt pos_start_sy = row.find("sy:") + 3, pos_end_sy = row.substr(pos_start_sy, row.size() - pos_start_sy).find(",");
			unt pos_start_sz = row.find("sz:") + 3, pos_end_sz = row.find("}");
			float x = std::stof(row.substr(pos_start_x, pos_end_x));
			float y = std::stof(row.substr(pos_start_y, pos_end_y));
			float z = std::stof(row.substr(pos_start_z, pos_end_z));
			float h = std::stof(row.substr(pos_start_h, pos_end_h)) * pi_div_180;
			float p = std::stof(row.substr(pos_start_p, pos_end_p)) * pi_div_180;
			float r = std::stof(row.substr(pos_start_r, pos_end_r)) * pi_div_180;
			float sx = 1 / std::stof(row.substr(pos_start_sx, pos_end_sx));
			float sy = 1 / std::stof(row.substr(pos_start_sy, pos_end_sy));
			float sz = 1 / std::stof(row.substr(pos_start_sz, pos_end_sz));

			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
			view = glm::rotate(view, h, glm::vec3(1, 0, 0));
			view = glm::rotate(view, p, glm::vec3(0, 1, 0));
			view = glm::rotate(view, r, glm::vec3(0, 0, 1));
			node->m_mat = view;
			node->m_inv = inverse(node->m_mat);
		}
		else if (row.find("- triangle:") != std::string::npos)
		{
			glm::vec3 p0, p1, p2;
			for (unt i = 0; i < 3; i++)
			{
				if (!fgets(buf, 1000, f))
					return node;
				std::string row(buf);
				unt pos_start_x = row.find("x:") + 2, pos_end_x = row.find(",");
				unt pos_start_y = row.find("y:") + 2, pos_end_y = row.substr(pos_start_y, row.size() - pos_start_y).find(",");
				unt pos_start_z = row.find("z:") + 2, pos_end_z = row.find("}");
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).x = std::stof(row.substr(pos_start_x, pos_end_x));
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).y = std::stof(row.substr(pos_start_y, pos_end_y));
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).z = std::stof(row.substr(pos_start_z, pos_end_z));
			}
			node->push_back(std::shared_ptr<Triangle>(new Triangle(p0, p1, p2)));
		}
		else if (row.find("- plane:") != std::string::npos)
		{
			glm::vec3 p0, p1, p2;
			for (unt i = 0; i < 3; i++)
			{
				if (!fgets(buf, 1000, f))
					return node;
				std::string row(buf);
				unt pos_start_x = row.find("x:") + 2, pos_end_x = row.find(",");
				unt pos_start_y = row.find("y:") + 2, pos_end_y = row.substr(pos_start_y, row.size() - pos_start_y).find(",");
				unt pos_start_z = row.find("z:") + 2, pos_end_z = row.find("}");
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).x = std::stof(row.substr(pos_start_x, pos_end_x));
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).y = std::stof(row.substr(pos_start_y, pos_end_y));
				((i == 0) ? p0 : ((i == 1) ? p1 : p2)).z = std::stof(row.substr(pos_start_z, pos_end_z));
			}
			node->push_back(std::shared_ptr<Plane>(new Plane(p0, p1, p2)));
		}
		else if (row.find("- sphere:") != std::string::npos)
		{
			float r;
			if (!fgets(buf, 1000, f))
				return node;
			std::string row(buf);
			unt pos_start_r = row.find("radius:") + 7;
			r = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			node->push_back(std::shared_ptr<Sphere>(new Sphere(r)));
		}
		else if (row.find("- cylinder:") != std::string::npos)
		{
			float r, h;
			if (!fgets(buf, 1000, f))
				return node;
			std::string row(buf);
			unt pos_start_r = row.find("radius:") + 7;
			r = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			if (!fgets(buf, 1000, f))
				return node;
			row = buf;
			pos_start_r = row.find("height:") + 7;
			h = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			node->push_back(std::shared_ptr<Cylinder>(new Cylinder(r, h)));
		}
		else if (row.find("- cone:") != std::string::npos)
		{
			float r, h;
			if (!fgets(buf, 1000, f))
				return node;
			std::string row(buf);
			unt pos_start_r = row.find("radius:") + 7;
			r = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			if (!fgets(buf, 1000, f))
				return node;
			row = buf;
			pos_start_r = row.find("height:") + 7;
			h = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			node->push_back(std::shared_ptr<Cone>(new Cone(r, h)));
		}
		else if (row.find("- torus:") != std::string::npos)
		{
			float r, h;
			if (!fgets(buf, 1000, f))
				return node;
			std::string row(buf);
			unt pos_start_r = row.find("radius:") + 7;
			r = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			if (!fgets(buf, 1000, f))
				return node;
			row = buf;
			pos_start_r = row.find("tube_radius:") + 12;
			h = std::stof(row.substr(pos_start_r, row.size() - pos_start_r));
			node->push_back(std::shared_ptr<Torus>(new Torus(r, h)));
		}
		else if (row.find("- obj_model:") != std::string::npos)
		{
			std::string fname;
			if (!fgets(buf, 1000, f))
				return node;
			std::string row(buf);
			unt pos_start_f = row.find("\"") + 1;
			row = row.substr(pos_start_f, row.size() - pos_start_f);
			unt pos_end_f = row.find("\"");
			fname = row.substr(0, pos_end_f);
			node->push_back(std::shared_ptr<ObjModel>(new ObjModel(fname)));
		}
		else
		{
			unt pos = row.find("- node:");
			if (pos != std::string::npos)
				node->push_back(ParseNode(f, pos + 1, CSG_NONE));
			else
			{
				csg_type type = CSG_NONE;
				if (row.find("- csg_union:") != std::string::npos)
					type = CSG_UNION;
				else if (row.find("- csg_difference:") != std::string::npos)
					type = CSG_DIFFERENCE;
				else if (row.find("- csg_intersection:") != std::string::npos)
					type = CSG_INTERSECTION;

				if (type != CSG_NONE)
				{
					pos = row.find("- csg_");
					node->push_back(ParseNode(f, pos + 1, type));
				}
				else
				{
					pos = row.find("- left_node:");
					if (pos != std::string::npos)
						node->push_back(ParseNode(f, pos + 1, CSG_NONE));
					else
					{
						pos = row.find("- right_node:");
						if (pos != std::string::npos)
							node->push_back(ParseNode(f, pos + 1, CSG_NONE));
					}
				}
			}
		}
	}
	return node;
}

void RayTracer::ScanObject(FILE *f)
{
	m_scene.push_back(ParseNode(f, 1));
	m_scene.m_csg_type = CSG_UNION;
}

RayTracer::RayTracer(	std::string		scene,
						std::string		out,
						std::string		norm,
						unt				x_res,
						unt				y_res,
						unt				trace_depth) :						
camera(Camera()),
width(x_res),
height(y_res),
image(Image(x_res, y_res)),
image_normal(Image(x_res, y_res)),
out_name(out),
norm_name(norm),
m_scene()
{
	FILE *f = fopen(scene.data(), "r");
	if (f == NULL)
		return;

	char buf[1001];
	while (fgets(buf, 1000, f))
	{
		std::string row(buf);
		if (row.find("- camera:") != std::string::npos)
		{
			ScanCamera(f);
		}
		else if (row.find("- node:") != std::string::npos)
		{
			ScanObject(f);
		}
	}
	fclose(f);
}


void RayTracer::traceRays() {
	float u, v;
	Ray ray;
	ray.origin = glm::vec3(0.0f, 0.0f, 0.0f);
	camera.calculateViewingCoordinates();
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			u = (x - 0.5f * width);
			v = (y - 0.5f * height);
			ray.direction = /*glm::normalize*/(glm::vec3(u, dist, v));
			if (x == 147 && y == height / 2)
			{
				x = x;
			}
			PointColor col = traceRay(ray);
			image.setPixelColor(x + width_shift, y + height_shift, col.out);
			image_normal.setPixelColor(x + width_shift, y + height_shift, col.norm);
		}
	}
	image.saveImage(out_name.c_str());
	image_normal.saveImage(norm_name.c_str());
}

float len(glm::vec3 vec){
	float res = abs(vec.y);// sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	if (res > 255)
	{
		res = 255;
	}
	return res;
}

PointColor &RayTracer::traceRay(Ray ray) {
	Intersector in = m_scene.intersect(ray);
	if (in.size() > 0) {
		Intersection r = in.getFirst();
		if (r.param <= 0)
			return PointColor(Color(0, 0, 0), Color(0, 0, 0));
		return PointColor(Color(len(r.point)), Color(abs(glm::normalize(r.point).z * 255.f)));
	}
	else
	{
		return PointColor(Color(0, 0, 0), Color(0, 0, 0));
	}
}