#include "MainDef.h"

#include "HittableList.h"
#include "Sphere.h"
#include "Camera.h"
#include "Vector3.h"
#include "Material.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <thread>
#include <array>

using namespace std;

// To which file the image will be written
const char* FILEPATH = "reflectionpic.ppm";
const int NUMBER_OF_THREADS = 100;

// Window measurments
const int WIDTH = 40;
const int HEIGHT = 20;
const double ASPECT_RATIO = double(WIDTH) / HEIGHT;

// Samples per pixel in Antiailising
const int SAMPLES_PER_PIXEL_SQRT = 10;
// The maximum bounds of a ray
const int MAX_DEPTH = 50;

// Wich Gamma value to use in the gamma correction.
const double GAMMA_VALUE = 2.0;


Vector3 RayColor(const Ray& r, const Hitable& world, int depth);
HitableList RandomScene();

void threadAction(Camera& cam, const Hitable& world, array<array<Vector3, HEIGHT>, WIDTH>& colors, int offset);

int main(void)
{
	ofstream file;
	file.open(FILEPATH);

	file << "P3\n" << WIDTH << ' ' << HEIGHT << "\n255\n";

	// Creating the World
	HitableList world = RandomScene();

	Vector3 lookfrom = Vector3(13, 2, 3);
	Vector3 lookat = Vector3(0, 0, 0);
	Vector3 vup = Vector3(0, 1, 0);
	double dist_to_focus = 10.0;
	double aperture = .1;

	Camera cam = Camera(lookfrom, lookat, vup, 20, ASPECT_RATIO, aperture, dist_to_focus);

	// Creating colors array
	array<array<Vector3, HEIGHT>, WIDTH> colors; // = (Vector3**)malloc(sizeof(Vector3*) * WIDTH);
	
	/*for (int i = 0; i < WIDTH; ++i)
	{
		colors[i] = (Vector3*)malloc(sizeof(Vector3*) * HEIGHT);
	}*/

	// Creating array of all threads
	array<std::thread, NUMBER_OF_THREADS> threads;
	/*
	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		threads[i] = thread([] {std::cout << "registered" << std::endl; });
	}
	*/
	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		threads[i] = thread(threadAction, cam, world, colors, i);
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		threads[i].join();
	}
	
	/*
	int currentThread = 0;
	for (int j = HEIGHT - 1; j >= 0; --j)
	{
		if (j % 1 == 0)
			std::cout << double(HEIGHT - j) / HEIGHT << "   PERCENT" << std::endl;
		for (int i = 0; i < WIDTH; ++i)
		{

			// threadAction(cam, world, colors, i, j);
			
			// Await thread and then call again
			if (!threads[currentThread].joinable())
			{
				cout << "fiuawfbaiwoubf " << i << " " << currentThread << " " << j << endl;
				cin.get();
			}
			threads[currentThread].join();
			threads[currentThread] = thread(threadAction, cam, world, colors, i, j);

			++currentThread;
			if (currentThread >= NUMBER_OF_THREADS)
				currentThread = 0;
			
		}
		
	}
	*/

	// Drawing the picture
	for (int j = HEIGHT - 1; j >= 0; --j)
	{
		for (int i = 0; i < WIDTH; ++i)
		{
			colors[i][j].WriteColor(file, SAMPLES_PER_PIXEL_SQRT * SAMPLES_PER_PIXEL_SQRT, GAMMA_VALUE);
		}
	}

	for (int i = 0; i < NUMBER_OF_THREADS; i++)
	{
		threads[i].join();
	}

	/*for (int i = 0; i < WIDTH; ++i)
	{
		free(colors[i]);
	}
	free(colors);*/

	file.close();
	return 0;
}


void threadAction(Camera& cam, const Hitable& world, array<array<Vector3, HEIGHT>, WIDTH>& colors, int offset)
{
	
	for (int N = offset; N < WIDTH * HEIGHT; N += NUMBER_OF_THREADS)
	{
		int i = N % WIDTH;
		int j = (N / WIDTH) % HEIGHT;
	Vector3 color;
	// SAMPLING LINEAR
	for (int xs = 1; xs <= SAMPLES_PER_PIXEL_SQRT; ++xs)
	{
		for (int ys = 1; ys <= SAMPLES_PER_PIXEL_SQRT; ++ys)
		{
			double u = (double(i) + double(xs) / (SAMPLES_PER_PIXEL_SQRT + 2)) / double(WIDTH);
			double v = (double(j) + double(ys) / (SAMPLES_PER_PIXEL_SQRT + 2)) / double(HEIGHT);
			Ray r = cam.GetRay(u, v);
			color += RayColor(r, world, MAX_DEPTH);
		}
	}
	colors[i][j] = color;
}
}

// Color calculation
Vector3 RayColor(const Ray& r, const Hitable& world, int depth)
{
	if (depth <= 0)
		return Vector3();

	// Checking for collisoin with world object
	HitRecord rec;
	if (world.Hit(r, 0.001, infinity, rec))
	{
		Vector3 target;

		Ray scattered = Ray(Vector3(), Vector3());
		Vector3 attenuation;
		if (rec.MatPtr->Scatter(r, rec, attenuation, scattered))
			return mult(attenuation, RayColor(scattered, world, depth - 1));

		return Vector3(0, 0, 0);
	}

	// otherwise returning sky background
	Vector3 unitDir = r.Direction.Normalized();
	double t = .5 * (unitDir.y + 1.0);
	return (1.0 - t) * Vector3(1, 1, 1) + t * Vector3(0.5, 0.7, 1.0);
}

HitableList RandomScene() {
	HitableList world = HitableList();

	world.Add(make_shared<Sphere>(
		Vector3(0, -1000, 0), 1000, make_shared<Lambertian>(Vector3(.3, .5, .5))));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			double chooseMat = ffrnd();
			Vector3 center = Vector3(a + .9 * ffrnd(), .2, b + .9 * ffrnd());
			if ((center - Vector3(4, .2, 0)).Length() > .9)
			{
				if (chooseMat < .8)
				{
					//diffuse
					Vector3 albedo = mult(Vector3::Random(), Vector3::Random());
					world.Add(
						make_shared<Sphere>(center, .2, make_shared<Lambertian>(albedo))
					);
				}
				else if (chooseMat < .95)
				{
					// metal
					Vector3 albedo = Vector3::Random(.5, 1);
					double fuzz = ffrnd(0, .5);
					world.Add(
						make_shared<Sphere>(center, .2, make_shared<Metal>(albedo, fuzz))
					);
				}
				else
				{
					// glass
					world.Add(make_shared<Sphere>(center, .2, make_shared<Dielectric>(1.5)));
				}
			}
		}
	}

	world.Add(make_shared<Sphere>(Vector3(0, 1, 0), 1.0, make_shared<Dielectric>(1.5)));

	world.Add(
		make_shared<Sphere>(Vector3(-4, 1, 0), 1.0, make_shared<Lambertian>(Vector3(0.4, 0.2, 0.1))));

	world.Add(
		make_shared<Sphere>(Vector3(4, 1, 0), 1.0, make_shared<Metal>(Vector3(0.7, 0.6, 0.5), 0.0)));

	return world;
}