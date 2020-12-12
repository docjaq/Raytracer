#ifndef RAYTRACER_UTILITY_H
#define RAYTRACER_UTILITY_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::default_random_engine(generator);
	return distribution(generator);
}

inline double random_double(double min, double max) {
	return min + (max - min)*random_double();
}

inline double clamp(double value, double min, double max) {

	if (value < min) return min;
	if (value > max) return max;

	return value;
}

/// @param[in] nb_elements : size of your for loop
/// @param[in] functor(start, end) :
/// your function processing a sub chunk of the for loop.
/// "start" is the first index to process (included) until the index "end"
/// (excluded)
/// @code
///     for(int i = start; i < end; ++i)
///         computation(i);
/// @endcode
/// @param use_threads : enable / disable threads.
///
///
static void parallel_for(unsigned nb_elements, std::function<void(int start, int end)> functor,	bool use_threads = true){

	unsigned nb_threads_hint = std::thread::hardware_concurrency();
	//unsigned nb_threads = nb_threads_hint == 0 ? 16 : (nb_threads_hint);

	unsigned nb_threads = 32;

	unsigned batch_size = nb_elements / nb_threads;
	unsigned batch_remainder = nb_elements % nb_threads;

	std::vector< std::thread > my_threads(nb_threads);

	if (use_threads)
	{
		// Multithread execution
		for (unsigned i = 0; i < nb_threads; ++i)
		{
			int start = i * batch_size;
			my_threads[i] = std::thread(functor, start, start + batch_size);
		}
	}
	else
	{
		// Single thread execution (for easy debugging)
		for (unsigned i = 0; i < nb_threads; ++i) {
			int start = i * batch_size;
			functor(start, start + batch_size);
		}
	}

	// Deform the elements left
	int start = nb_threads * batch_size;
	functor(start, start + batch_remainder);

	// Wait for the other thread to finish their task
	if (use_threads)
		std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
}

#include "ray.h"
#include "vec3.h"

#endif
