#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include "utility.h"

struct  hit_record;

class material {

public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

//JAQ Move this into another class
class lambertian : public material {

public:
	color albedo; //JAQ: Try and make private

	lambertian(const color& albedoIn) : albedo(albedoIn){}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

		auto scatter_direction = rec.normal + random_unit_vector();

		//Catch degenerate scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;


		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;

		return true;
	}
};

class metal : public material {

public:
	color albedo; //JAQ: Try and make private

	metal(const color& albedoIn) : albedo(albedoIn) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		
		return (dot(scattered.direction(), rec.normal) > 0);
	}
};


#endif
