/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htaheri <htaheri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:00:00 by user              #+#    #+#             */
/*   Updated: 2025/06/15 13:28:37 by htaheri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minirt.h"


int	intersect_sphere(t_ray *ray, t_sphere sphere)
{
	t_vector	oc;
	double		a;
	double		b;
	double		c;
	double		discriminant;
	double		t1;
	double		t2;

	oc = vec_sub(ray->origin, sphere.center);
	a = vec_dot(ray->direction, ray->direction);
	b = 2.0 * vec_dot(oc, ray->direction);
	c = vec_dot(oc, oc) - sphere.radius * sphere.radius;
	discriminant = b * b - 4 * a * c;
	
	if (discriminant < 0)
		return (0);
		
	t1 = (-b - sqrt(discriminant)) / (2.0 * a);
	t2 = (-b + sqrt(discriminant)) / (2.0 * a);
	
	if (t1 > EPSILON && t1 < ray->t)
	{
		ray->t = t1;
		return (1);
	}
	if (t2 > EPSILON && t2 < ray->t)
	{
		ray->t = t2;
		return (1);
	}
	return (0);
}

/**
 * Checks for intersection between a ray and a plane.
 * Returns 1 if there's an intersection, 0 otherwise.
 * Updates ray->t with the closest intersection distance.
 */
int	intersect_plane(t_ray *ray, t_plane plane)
{
	double		denom;
	t_vector	p0l0;
	double		t;

	denom = vec_dot(plane.normal, ray->direction);
	if (fabs(denom) < EPSILON)
		return (0);  // Ray is parallel to the plane

	p0l0 = vec_sub(plane.point, ray->origin);
	t = vec_dot(p0l0, plane.normal) / denom;

	if (t > EPSILON && t < ray->t)
	{
		ray->t = t;
		return (1);
	}
	return (0);
}

/**
 * Helper function to solve quadratic equation
 * Returns 1 if valid solutions are found, 0 otherwise
 */
int	solve_quadratic(double coeffs[3], double *t1, double *t2)
{
	double	discriminant;
	double	q;

	discriminant = coeffs[1] * coeffs[1] - 4 * coeffs[0] * coeffs[2];
	if (discriminant < 0)
		return (0);

	if (coeffs[1] < 0)
		q = -0.5 * (coeffs[1] - sqrt(discriminant));
	else
		q = -0.5 * (coeffs[1] + sqrt(discriminant));

	*t1 = q / coeffs[0];
	*t2 = coeffs[2] / q;

	if (*t1 > *t2)
	{
		double tmp = *t1;
		*t1 = *t2;
		*t2 = tmp;
	}
	return (1);
}

int	intersect_cylinder(t_ray *ray, t_cylinder cylinder)
{
	t_vector	oc;
	t_vector	abc;
	double		coeffs[3];
	double		t1;
	double		t2;
	t_vector	hit_point;
	t_vector	cp;
	double		proj;
	int			result;

	oc = vec_sub(ray->origin, cylinder.center);
	
	abc.x = vec_dot(ray->direction, ray->direction) - 
			pow(vec_dot(ray->direction, cylinder.axis), 2);
	abc.y = 2 * (vec_dot(ray->direction, oc) - 
			vec_dot(ray->direction, cylinder.axis) * vec_dot(oc, cylinder.axis));
	abc.z = vec_dot(oc, oc) - pow(vec_dot(oc, cylinder.axis), 2) - 
			cylinder.radius * cylinder.radius;
	
	coeffs[0] = abc.x;
	coeffs[1] = abc.y;
	coeffs[2] = abc.z;
	
	if (!solve_quadratic(coeffs, &t1, &t2))
		return (0);
	
	result = 0;
	if (t1 > EPSILON && t1 < ray->t)
	{
		hit_point = ray_at(*ray, t1);
		cp = vec_sub(hit_point, cylinder.center);
		proj = vec_dot(cp, cylinder.axis);
		if (proj >= 0 && proj <= cylinder.height)
		{
			ray->t = t1;
			result = 1;
		}
	}
	
	if (t2 > EPSILON && t2 < ray->t)
	{
		hit_point = ray_at(*ray, t2);
		cp = vec_sub(hit_point, cylinder.center);
		proj = vec_dot(cp, cylinder.axis);
		if (proj >= 0 && proj <= cylinder.height)
		{
			ray->t = t2;
			result = 1;
		}
	}
	

	t_plane bottom_cap = {cylinder.center, {-cylinder.axis.x, -cylinder.axis.y, -cylinder.axis.z}};
	t_ray cap_ray = *ray;
	if (intersect_plane(&cap_ray, bottom_cap))
	{
		hit_point = ray_at(cap_ray, cap_ray.t);
		if (vec_length(vec_sub(hit_point, cylinder.center)) <= cylinder.radius && 
			cap_ray.t < ray->t && cap_ray.t > EPSILON)
		{
			ray->t = cap_ray.t;
			result = 1;
		}
	}
	
	t_vector top_center = vec_add(cylinder.center, vec_mul(cylinder.axis, cylinder.height));
	t_plane top_cap = {top_center, cylinder.axis};
	cap_ray = *ray;
	if (intersect_plane(&cap_ray, top_cap))
	{
		hit_point = ray_at(cap_ray, cap_ray.t);
		if (vec_length(vec_sub(hit_point, top_center)) <= cylinder.radius && 
			cap_ray.t < ray->t && cap_ray.t > EPSILON)
		{
			ray->t = cap_ray.t;
			result = 1;
		}
	}
	
	return (result);
}


// int intersect_cone(t_ray *ray, t_cone cone)
// {
//     t_vector    co;
//     double      a, b, c;
//     double      discriminant;
//     double      cos2;
//     double      t1, t2;
//     double      m1, m2;
//     t_vector    hit_point;
//     int         result;

//     co = vec_sub(ray->origin, cone.vertex);
//     cos2 = cos(cone.angle) * cos(cone.angle);
    
//     double dot_dir_axis = vec_dot(ray->direction, cone.axis);
//     double dot_co_axis = vec_dot(co, cone.axis);
    
//     a = pow(dot_dir_axis, 2) - cos2;
//     b = 2 * (dot_dir_axis * dot_co_axis - vec_dot(ray->direction, co) * cos2);
//     c = pow(dot_co_axis, 2) - vec_dot(co, co) * cos2;
    
//     discriminant = b * b - 4 * a * c;
    
//     if (discriminant < 0)
//         return (0);
        
//     t1 = (-b - sqrt(discriminant)) / (2 * a);
//     t2 = (-b + sqrt(discriminant)) / (2 * a);
    
//     result = 0;
    
//     if (t1 > EPSILON && t1 < ray->t)
//     {
//         hit_point = ray_at(*ray, t1);
//         m1 = vec_dot(vec_sub(hit_point, cone.vertex), cone.axis);
        
//         if (m1 >= 0 && m1 <= cone.height)
//         {
//             ray->t = t1;
//             result = 1;
//         }
//     }
    
//     if (t2 > EPSILON && t2 < ray->t)
//     {
//         hit_point = ray_at(*ray, t2);
//         m2 = vec_dot(vec_sub(hit_point, cone.vertex), cone.axis);
        
//         if (m2 >= 0 && m2 <= cone.height)
//         {
//             ray->t = t2;
//             result = 1;
//         }
//     }
    
//     t_vector base_center = vec_add(cone.vertex, vec_mul(cone.axis, cone.height));
//     double base_radius = cone.height * tan(cone.angle);
    
//     t_plane base_plane = {base_center, cone.axis};
//     t_ray cap_ray = *ray;
    
//     if (intersect_plane(&cap_ray, base_plane))
//     {
//         hit_point = ray_at(cap_ray, cap_ray.t);
//         if (vec_length(vec_sub(hit_point, base_center)) <= base_radius && 
//             cap_ray.t < ray->t && cap_ray.t > EPSILON)
//         {
//             ray->t = cap_ray.t;
//             result = 1;
//         }
//     }
    
//     return (result);
// }

t_ray	ray_create(t_vector origin, t_vector direction)
{
	t_ray	ray;

	ray.origin = origin;
	ray.direction = vec_normalize(direction);
	ray.t = INFINITY;
	return (ray);
}

t_vector	ray_at(t_ray ray, double t)
{
	return (vec_add(ray.origin, vec_mul(ray.direction, t)));
}

// int intersect_triangle(t_ray *ray, t_triangle triangle)
// {
//     t_vector edge1, edge2, h, s, q;
//     double a, f, u, v, t;

//     edge1 = vec_sub(triangle.v2, triangle.v1);
//     edge2 = vec_sub(triangle.v3, triangle.v1);
//     h = vec_cross(ray->direction, edge2);
//     a = vec_dot(edge1, h);

//     if (a > -EPSILON && a < EPSILON)
//         return (0);

//     f = 1.0 / a;
//     s = vec_sub(ray->origin, triangle.v1);
//     u = f * vec_dot(s, h);

//     if (u < 0.0 || u > 1.0)
//         return (0);

//     q = vec_cross(s, edge1);
//     v = f * vec_dot(ray->direction, q);

//     if (v < 0.0 || u + v > 1.0)
//         return (0);

//     t = f * vec_dot(edge2, q);

//     if (t > EPSILON && t < ray->t)
//     {
//         ray->t = t;
//         return (1);
//     }

//     return (0);
// }

t_vector triangle_normal(t_triangle triangle)
{
    return (triangle.normal);
}

int intersect_object(t_ray *ray, t_object obj)
{
    if (obj.type == SPHERE)
        return (intersect_sphere(ray, obj.sphere));
    else if (obj.type == PLANE)
        return (intersect_plane(ray, obj.plane));
    else if (obj.type == CYLINDER)
        return (intersect_cylinder(ray, obj.cylinder));
    // else if (obj.type == CONE)
    //     return (intersect_cone(ray, obj.cone));
    // else if (obj.type == HYPERBOLOID)
    //     return (intersect_hyperboloid(ray, obj.hyperboloid));
	// else if (obj.type == TRIANGLE)
    //     return (intersect_triangle(ray, obj.triangle));
    return (0);
}
