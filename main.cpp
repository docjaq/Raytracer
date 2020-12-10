#include <iostream>
#include <fstream>

#include "utility.h"
#include "camera.h"
#include "hittable_list.h"
#include "color.h"
#include "sphere.h"

color ray_color(const ray& r, hittable& world, int depth){

    hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)){
		point3 target = rec.p + rec.normal + random_in_unit_sphere();

		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
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

	//World
	auto ground = make_shared<sphere>(point3(0, -100.5, -1), 100);
	auto sphere1 = make_shared<sphere>(vec3(0.0f, 0.0f, -1.0f), 0.5f);
    hittable_list world;
	world.add(ground);
	world.add(sphere1);

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