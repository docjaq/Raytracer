#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "hittable.h"

class sphere: public hittable {
public:
    sphere(){}
    sphere(point3 cen, double r): centre(cen), radius(r) {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override;

    point3 centre;
    double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - centre;
	double a = r.direction().length_squared();
	double half_b = dot(oc, r.direction());
	double c = oc.length_squared() - radius * radius;
	
	double discriminant = half_b * half_b - a * c;


	if (discriminant < 0) {
		return false;
	}
	auto sqrt_disc = sqrt(discriminant);
	
	//Find the nearest root that lies in the acceptable range
	auto root = (-half_b - sqrt_disc) / a;

	if (root < t_min || t_max < root) {

		root = (-half_b + sqrt_disc) / a;
		if (root < t_min || t_max < root) {
			return false;
		}
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	rec.normal = (rec.p - centre) / radius;
	vec3 outward_normal = (rec.p - centre) / radius;
	rec.set_face_normal(r, outward_normal);

	return true;
}





#endif //RAYTRACER_SPHERE_H
