#include <iostream>
#include <fstream>

#include "utility.h"
#include "camera.h"
#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "material.h"

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

int main() {
    std::ofstream outputPPM;
    outputPPM.open ("Output.ppm");

	auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	outputPPM << "P3\n" << image_width << " " << image_height << "\n255\n";
	
	camera cam;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_sphere_lambertian_red = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto material_sphere_metal = make_shared<metal>(color(0.8, 0.8, 0.8));
	auto material_sphere_metal_green = make_shared<metal>(color(0.0, 0.6, 0.2));

	//World
	auto ground = make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground);
	auto sphere_middle = make_shared<sphere>(vec3(0.0f, 0.0f, -1.0f), 0.5f, material_sphere_lambertian_red);
	auto sphere_left = make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_sphere_metal);
	auto sphere_right = make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_sphere_metal_green);

    hittable_list world;
	world.add(ground);
	world.add(std::move(sphere_middle));
	world.add(std::move(sphere_left));
	world.add(std::move(sphere_right));

	std::cout << "num referendes to ground = " << ground.use_count() << "\n";

    for(int j = image_height-1; j >= 0; --j){
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for(int i = 0; i < image_width; ++i){

			color pixel_color(0, 0, 0);

			for (int s = 0; s < samples_per_pixel; ++s) {

				//Send ray from origin, through offset position
				double u = (i+random_double()) / (image_width - 1);
				double v = (j+random_double()) / (image_height - 1);

				ray r = cam.get_ray(u, v);

				pixel_color += ray_color(r, world, max_depth);

			}
			write_color(outputPPM, pixel_color, samples_per_pixel);
        }
    }

    outputPPM.close();

}