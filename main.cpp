#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

#include "utility.h"
#include "camera.h"
#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "material.h"

//Test scene render
hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}


color ray_color(const ray& r, hittable& world, int depth){

    hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)){
		ray scattered;
		color attenuation;

		//JAQ: not sure I understand the changes here
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}

	//If no geometry hit, returns background
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5*(unit_direction.y() + 1.0);
    
	return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);

}

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

	//If no geometry hit, returns background
	return vec3(0.0, 0.0, 0.0);
}

int main() {

	//Image
	auto aspect_ratio = 3.0 / 2.0;
	const int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 500;
	const int max_depth = 50;

	//Camera
	point3 cameraPosition(13, 2, 3);
	point3 lookAtPosition(0, 0, 0);
	vec3 up(0, 1, 0);
	double fov = 20;
	double aperture = 0.1;
	double dist_to_focus = 10.0;
	camera cam(cameraPosition, lookAtPosition, up, fov, aspect_ratio, aperture, dist_to_focus );
	
	//World
	auto world = random_scene();

	auto startTime = std::chrono::high_resolution_clock::now();

	std::vector<color> colorData(image_width*image_height);

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

		parallel_for(image_width, [&](int start, int end) {
			for (int i = start; i < end; ++i) {

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
		});
	}

	auto finishTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
	auto durationSeconds = duration / 1000000.0;

	std::ofstream outputPPM;
	outputPPM.open("Output.ppm");
	outputPPM << "P3\n" << image_width << " " << image_height << "\n255\n";
	write_color(outputPPM, colorData, samples_per_pixel);
	outputPPM.close();

	std::cout << "Time: " << durationSeconds << " seconds\n";
}