#include <iostream>
#include <fstream>
#include <limits>
#include "ray.h"
#include "hitable.h"
#include "float.h"
#include "sphere.h"
#include "hitable_list.h"
#include "main.h"

#include "vec3.h"
#include "color.h"

color ray_color(const ray& r, hitable *world){

    hit_record rec;

    if(world->hit(r, 0.0f, std::numeric_limits<double>::max(), rec))
        return 0.5f*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);

    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5f*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);

}

int main() {
    std::ofstream outputPPM;
    outputPPM.open ("Output.ppm");

	const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width/aspect_ratio);

    outputPPM << "P3\n" << image_width << " " << image_height << "\n255\n";


	auto viewport_height = 2.0;
	auto viewpoirt_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;
	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewpoirt_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    hitable *list[2];
    list[0] = new sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f);
    list[1] = new sphere(vec3(0.0f, -100.5f, -1.0f), 100.0f);

    //list[2] = new sphere(vec3(-0.8f, 0.0f, -1.0f), 0.1f);
    //list[3] = new sphere(vec3(0.8f, 0.0f, -1.0f), 0.1f);

    //Bug where the list won't properly scale past 4 elements?
    //list[4] = new sphere(vec3(0.0f, 0.8f, -1.0f), 0.1f);
    //list[5] = new sphere(vec3(0.0f, -0.5f, -1.0f), 0.1f);


    hitable *world = new hitable_list(list, 2);

    for(int j = image_height-1; j >= 0; --j){
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for(int i = 0; i < image_width; ++i){

            //Send ray from origin, through offset position
            double u = double(i)/double(image_width-1);
            double v = double(j)/double(image_height-1);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);

            //vec3 p = r.point_at_parameter(2.0f);
           
			color pixel_color = ray_color(r, world);
			write_color(outputPPM, pixel_color);
        }
    }

    outputPPM.close();

}