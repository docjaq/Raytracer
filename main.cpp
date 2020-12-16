#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <mutex>

#include "utility.h"
#include "camera.h"
#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "material.h"

std::mutex mu;

color ray_color(const ray& r, hittable& world, int depth){

    hit_record rec;

	//Black if hitting max bounce number (recursion depth)
	if (depth <= 0)
		return color(0, 0, 0);

	//Fire a ray into world
	if (world.hit(r, 0.001, infinity, rec)){
		ray scattered;
		color attenuation;

		//Fire ray (scattered according to material), attenuated product of final color on pop
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);

		return color(0, 0, 0);
	}

	//If no geometry hit, returns background gradient
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    
	return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

//Iterative version instead...
color ray_color_iter(const ray& r, hittable& world, int maxBounces) {

	
	ray current_ray = r;
	color currentAttenuation = vec3(1.0, 1.0, 1.0);

	for (int i = 0; i < maxBounces; ++i) {

		hit_record rec;
		if (world.hit(current_ray, 0.001, infinity, rec)) {
			ray scattered;
			color attenuation;
 
			if (rec.mat_ptr->scatter(current_ray, rec, attenuation, scattered)) {
				currentAttenuation = attenuation * currentAttenuation;
				current_ray = scattered;
			}
		}
		else {
			vec3 unit_direction = unit_vector(current_ray.direction());
			double t = 0.5*(unit_direction.y() + 1.0);

			return currentAttenuation*((1.0 - t)*color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0));
		}
	}

	return vec3(0.0, 0.0, 0.0);
}

void LockedOutput(int totalScanLines, int finishedScanLines){
	std::lock_guard<std::mutex> lock(mu);
	std::cout << "\r% Complete: " << static_cast<int>((float)finishedScanLines / totalScanLines * 100) << ' ' << std::flush;
}

int main() {

	//Some basic parameters
	auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 50;
	const int max_depth = 50;

	//Parameterise camera
	point3 cameraPosition(3, 3, 2);
	point3 lookAtPosition(0, 0, -1);
	vec3 up(0, 1, 0);
	double fov = 20;
	double aperture = 0.5;
	double dist_to_focus = (cameraPosition - lookAtPosition).length();

	camera cam(cameraPosition, lookAtPosition, up, fov, aspect_ratio, aperture, dist_to_focus );
	
	//Materials
	auto mat_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto mat_lambertian = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto mat_glass = make_shared<dielectric>(1.5);
	auto mat_metallic = make_shared<metal>(color(1.0, 0.6, 0.6), 0.0);

	//World
	hittable_list world;
	auto ground = make_shared<sphere>(point3(0, -100.5, -1), 100, mat_ground);
	
	auto sphere_left_inner = make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, mat_glass);
	auto sphere_left_outer = make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, mat_glass);
	auto sphere_right = make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, mat_metallic);
	auto sphere_centre = make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, mat_lambertian);

	world.add(std::move(ground));
	world.add(std::move(sphere_left_inner));
	world.add(std::move(sphere_left_outer));
	world.add(std::move(sphere_right));
	world.add(std::move(sphere_centre));

	auto startTime = std::chrono::high_resolution_clock::now();

	//Need to write to datastructure when parallelising
	std::vector<color> colorData(image_width*image_height);

	int totalScanLines = image_height;
	int finishedScanLines = 0;

	parallel_for(image_height, [&](int start, int end) {
		for (int j = start; j < end; ++j) {
			
			LockedOutput(totalScanLines, finishedScanLines);

			for (int i = 0; i < image_width; ++i) {

				color pixel_color(0, 0, 0);

				for (int s = 0; s < samples_per_pixel; ++s) {

					//Send ray from origin, through offset position
					double u = (i + random_double()) / (image_width - 1);
					double v = (j + random_double()) / (image_height - 1);

					ray r = cam.get_ray(u, v);
					pixel_color += ray_color_iter(r, world, max_depth);
				}

				colorData[((image_height - 1) - j)*image_width + i] = std::move(pixel_color);
			}

			finishedScanLines++;
		}
	});

	LockedOutput(totalScanLines, finishedScanLines);

	std::cout << "Writing...";

	auto finishTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
	auto durationSeconds = duration / 1000000.0;

	std::ofstream outputPPM;
	outputPPM.open("Output.ppm");
	outputPPM << "P3\n" << image_width << " " << image_height << "\n255\n";
	write_color(outputPPM, colorData, samples_per_pixel);
	outputPPM.close();

	std::cout << "done\n";

	std::cout << "Time: " << durationSeconds << " seconds\n";
}