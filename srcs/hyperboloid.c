#include "../includes/minirt.h"

int intersect_hyperboloid(t_ray *ray, t_hyperboloid hyp)
{
    t_vector oc;
    double   coeffs[3];
    double   t1, t2;
    t_vector hit_point;
    double   height;
    int      result;

    oc = vec_sub(ray->origin, hyp.center);
    
    coeffs[0] = (ray->direction.x * ray->direction.x) / (hyp.a * hyp.a) +
                (ray->direction.y * ray->direction.y) / (hyp.b * hyp.b) -
                (ray->direction.z * ray->direction.z) / (hyp.c * hyp.c);
                
    coeffs[1] = (2 * oc.x * ray->direction.x) / (hyp.a * hyp.a) +
                (2 * oc.y * ray->direction.y) / (hyp.b * hyp.b) -
                (2 * oc.z * ray->direction.z) / (hyp.c * hyp.c);
                
    coeffs[2] = (oc.x * oc.x) / (hyp.a * hyp.a) +
                (oc.y * oc.y) / (hyp.b * hyp.b) -
                (oc.z * oc.z) / (hyp.c * hyp.c) - 1;
    
    if (!solve_quadratic(coeffs, &t1, &t2))
        return (0);
    
    result = 0;
    
    if (t1 > EPSILON && t1 < ray->t)
    {
        hit_point = ray_at(*ray, t1);
        height = vec_length(vec_sub(hit_point, hyp.center));
        
        if (height <= hyp.height)
        {
            ray->t = t1;
            result = 1;
        }
    }
    
    if (t2 > EPSILON && t2 < ray->t)
    {
        hit_point = ray_at(*ray, t2);
        height = vec_length(vec_sub(hit_point, hyp.center));
        
        if (height <= hyp.height)
        {
            ray->t = t2;
            result = 1;
        }
    }
    
    return (result);
}

t_vector hyperboloid_normal(t_vector point, t_hyperboloid hyp)
{
    t_vector normal;
    t_vector p;
    
    p = vec_sub(point, hyp.center);
    
    normal.x = 2 * p.x / (hyp.a * hyp.a);
    normal.y = 2 * p.y / (hyp.b * hyp.b);
    normal.z = -2 * p.z / (hyp.c * hyp.c);
    
    return (vec_normalize(normal));
}