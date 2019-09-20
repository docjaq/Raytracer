#include <iostream>
#include <fstream>
#include "ray.h"

vec3 color(const ray& r){
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5f*(unit_direction.y() + 1.0f);

    return (1.0f-t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
}

int main() {
    std::ofstream outputPPM;
    outputPPM.open ("Output.ppm");

    int nx = 200;
    int ny = 100;

    outputPPM << "P3\n" << nx << " " << ny << "\n255\n";

    //Define camera transforms
    vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
    vec3 horizontal(4.0f, 0.0f, 0.0f);
    vec3 vertical(0.0f, 2.0f, 0.0f);
    vec3 origin(0.0f, 0.0f, 0.0f);


    for(int j = ny-1; j >= 0; j--){
        for(int i = 0; i < nx; i++){

            float u = float(i)/float(nx);
            float v = float(j)/float(ny);

            //Send ray from origin, through offset position
            ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            vec3 col = color(r);

            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);

            outputPPM << ir << " " << ig << " " << ib << "\n";
        }
    }

    outputPPM.close();

}