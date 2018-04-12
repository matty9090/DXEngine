#include "AABB.hpp"
#include "Ray.hpp"

bool AABB::intersects(Ray ray, float t0, float t1) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[ray.sign[0]].x + pos.x - ray.origin.x) * ray.inv_direction.x;
	tmax = (bounds[1 - ray.sign[0]].x + pos.x - ray.origin.x) * ray.inv_direction.x;
	tymin = (bounds[ray.sign[1]].y + pos.y - ray.origin.y) * ray.inv_direction.y;
	tymax = (bounds[1 - ray.sign[1]].y + pos.y - ray.origin.y) * ray.inv_direction.y;

	if ((tmin > tymax) || (tymin > tmax)) return false;
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	tzmin = (bounds[ray.sign[2]].z + pos.z - ray.origin.z) * ray.inv_direction.z;
	tzmax = (bounds[1 - ray.sign[2]].z + pos.z - ray.origin.z) * ray.inv_direction.z;

	if ((tmin > tzmax) || (tzmin > tmax)) return false;
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	return ((t0 < tmax) && (tmin < t1));
}
