#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "vec3.h"

class ray{

    public:
        ray(){}
		ray(const point3 &originIn, const vec3 &directionIn) : orig(originIn), dir(directionIn) {}

        point3 origin() const {
            return orig;
        }

        vec3 direction() const{
            return dir;
        }

		//t parameterises our ray
        vec3 at(double t) const{
            return orig + t*dir;
        }

        point3 orig;
		vec3 dir;
};

#endif //RAYTRACER_RAY_H
