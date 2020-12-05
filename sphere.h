#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "hitable.h"

class sphere: public hitable {
public:
    sphere(){}
    sphere(vec3 cen, double r): centre(cen), radius(r) {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

    vec3 centre;
    double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - centre;
    double a = dot(r.direction(), r.direction());
    double b = dot(oc, r.direction());
    double c = dot(oc, oc) - radius*radius;
    double discriminant = b*b - a*c;

    if (discriminant > 0){

        double temp = (-b - sqrt(b*b - a*c))/a;
        if(temp < t_max && temp > t_min){

            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - centre)/radius;

            return true;
        }

        temp = (-b + sqrt(b*b - a*c))/a;
        if(temp < t_max && temp > t_min){

            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - centre)/radius;

            return true;
        }

    }

    return false;
}





#endif //RAYTRACER_SPHERE_H
