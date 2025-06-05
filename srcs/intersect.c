#include "../includes/minirt.h"


int intersect_object(t_ray *ray, t_object obj)
{
    if (obj.type == SPHERE)
        return (intersect_sphere(ray, obj.sphere));
    else if (obj.type == PLANE)
        return (intersect_plane(ray, obj.plane));
    else if (obj.type == CYLINDER)
        return (intersect_cylinder(ray, obj.cylinder));
    else if (obj.type == CONE)
        return (intersect_cone(ray, obj.cone));
    else if (obj.type == HYPERBOLOID)
        return (intersect_hyperboloid(ray, obj.hyperboloid));
	else if (obj.type == TRIANGLE)
        return (intersect_triangle(ray, obj.triangle));
    return (0);
}