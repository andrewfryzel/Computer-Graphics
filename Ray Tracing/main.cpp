//u1070220
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include <Eigen>

using namespace Eigen;

// image background color
Vector3f bgcolor(1.0f, 1.0f, 1.0f);

// lights in the scene
std::vector<Vector3f> lightPositions = { Vector3f(0.0, 60, 60)
									   , Vector3f(-60.0, 60, 60)
									   , Vector3f(60.0, 60, 60) };

class Sphere
{
public:
	Vector3f center;  // position of the sphere
	float radius;  // sphere radius
	Vector3f surfaceColor; // surface color

	Sphere(
		const Vector3f& c,
		const float& r,
		const Vector3f& sc) :
		center(c), radius(r), surfaceColor(sc)
	{
	}

	// line vs. sphere intersection (note: this is slightly different from ray vs. sphere intersection!)
	bool intersect(const Vector3f& rayOrigin, const Vector3f& rayDirection, float& t0, float& t1) const
	{
		Vector3f l = center - rayOrigin;
		float tca = l.dot(rayDirection);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > (radius * radius)) return false;
		float thc = sqrt(radius * radius - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

Vector3f diffuse(const Vector3f& L, // direction vector from the point on the surface towards a light source
	const Vector3f& N, // normal at this point on the surface
	const Vector3f& diffuseColor,
	const float kd // diffuse reflection constant
)
{
	Vector3f resColor = Vector3f::Zero();
	// Have to make sure that the light isn't blocked / dot == 0
	if (L.dot(N) > 0)
	{
		//From an intersection point P we cast rays to all lights (as in Task 2),
		//and if the light is not blocked, we add 0.333 * kd * max(L.dot(N), 0) * diffuseColor
		resColor = 0.333 * kd * L.dot(N) * diffuseColor;
	}

	return resColor;
}

// Phong reflection model
Vector3f phong(const Vector3f& L, // direction vector from the point on the surface towards a light source
	const Vector3f& N, // normal at this point on the surface
	const Vector3f& V, // direction pointing towards the viewer
	const Vector3f& diffuseColor,
	const Vector3f& specularColor,
	const float kd, // diffuse reflection constant
	const float ks, // specular reflection constant
	const float alpha) // shininess constant
{
	Vector3f resColor = Vector3f::Zero();

	// TODO: implement Phong shading model
	//slide 11 lecture 14 R = 2N (N · L) -L
	Vector3f R = 2 * N * N.dot(L) - L;

	//Find the diffuse value
	Vector3f diffused = diffuse(L, N, diffuseColor, kd);

	float maximum;

	//Calculate maximum. If it dot isnt greater than 0, set it to something (I.E. 0)
	R.dot(V) > 0 ? maximum = std::pow(R.dot(V), alpha) : maximum = std::pow(0, alpha);

	//Calculate resColod
	resColor = diffused + 0.333 * ks * specularColor * maximum;

	return resColor;
}

Vector3f trace(
	const Vector3f& rayOrigin,
	const Vector3f& rayDirection,
	const std::vector<Sphere>& spheres)
{
	Vector3f pixelColor = bgcolor;
	Vector3f point = Vector3f::Zero();
	Vector3f locationOnHit;

	float min = INFINITY;
	int distance = -1;
	float t0;
	float t1;

	for (unsigned int i = 0; i < spheres.size(); i++)
	{
		bool intersectionCheck = spheres[i].intersect(rayOrigin, rayDirection, t0, t1);

		// If “true” is returned, the function also returns the intersection points as distances along the ray(t0, t1, where t0 is the 
		// first intersection and t1 the second one).
		if (min > t0&& intersectionCheck)
		{
			min = t0;
			distance = i;
			locationOnHit = rayOrigin + (min * rayDirection);
		}
	}

	float t00;
	float t11;

	for (unsigned int j = 0; j < lightPositions.size(); j++)
	{
		Vector3f lights = lightPositions[j] - locationOnHit;
		lights.normalize();
		bool intersectionCheck = false;

		for (unsigned int k = 0; k < spheres.size(); k++)
		{
			if (spheres[k].intersect(locationOnHit, lights, t00, t11))
			{
				intersectionCheck = true;
			}
		}

		if (!intersectionCheck)
		{
			//This line is for the part 1 color shading
			//point += 0.333 * spheres[distance].surfaceColor;

			Vector3f normalized = locationOnHit - spheres[distance].center;
			normalized.normalize();

			//This line is for diffuse
			//point += diffuse(lights, normalized, spheres[distance].surfaceColor, 1);

			//All this is for Phong
			Vector3f L = lights;
			Vector3f N = normalized;
			Vector3f V = -rayDirection;
			Vector3f specularColor = Vector3f::Ones();

			point += phong(L, N, V, spheres[distance].surfaceColor, specularColor, 1, 3, 100);

		}
	}

	//Flag to check if there had been an intersection
	if (distance != -1)
	{
		return point;
	}
	//The default bgcolor
	else
	{
		return bgcolor;
	}
}


void render(const std::vector<Sphere>& spheres)
{
	unsigned width = 640;
	unsigned height = 480;
	Vector3f* image = new Vector3f[width * height];
	Vector3f* pixel = image;
	float invWidth = 1 / float(width);
	float invHeight = 1 / float(height);
	float fov = 30;
	float aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);

	// Trace rays
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			float rayX = (2 * ((x + 0.5f) * invWidth) - 1) * angle * aspectratio;
			float rayY = (1 - 2 * ((y + 0.5f) * invHeight)) * angle;
			Vector3f rayDirection(rayX, rayY, -1);
			rayDirection.normalize();
			*(pixel++) = trace(Vector3f::Zero(), rayDirection, spheres);
		}
	}

	// Save result to a PPM image
	std::ofstream ofs("./render.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i)
	{
		const float x = image[i](0);
		const float y = image[i](1);
		const float z = image[i](2);

		ofs << (unsigned char)(std::min(float(1), x) * 255)
			<< (unsigned char)(std::min(float(1), y) * 255)
			<< (unsigned char)(std::min(float(1), z) * 255);
	}

	ofs.close();
	delete[] image;
}

int main(int argc, char** argv)
{
	std::vector<Sphere> spheres;
	// position, radius, surface color
	spheres.push_back(Sphere(Vector3f(0.0, -10004, -20), 10000, Vector3f(0.50, 0.50, 0.50)));
	spheres.push_back(Sphere(Vector3f(0.0, 0, -20), 4, Vector3f(1.00, 0.32, 0.36)));
	spheres.push_back(Sphere(Vector3f(5.0, -1, -15), 2, Vector3f(0.90, 0.76, 0.46)));
	spheres.push_back(Sphere(Vector3f(5.0, 0, -25), 3, Vector3f(0.65, 0.77, 0.97)));
	spheres.push_back(Sphere(Vector3f(-5.5, 0, -13), 3, Vector3f(0.90, 0.90, 0.90)));

	render(spheres);

	return 0;
}
