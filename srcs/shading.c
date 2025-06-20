#include "../includes/minirt.h"


uint32_t	color_to_int(t_color color)
{
	return ((color.r << 24) | (color.g << 16) | (color.b << 8) | 255);
}


t_color	color_scale(t_color color, double scale)
{
	t_color	result;

	result.r = fmin(255, fmax(0, color.r * scale));
	result.g = fmin(255, fmax(0, color.g * scale));
	result.b = fmin(255, fmax(0, color.b * scale));
	return (result);
}


t_color	color_add(t_color c1, t_color c2)
{
	t_color	result;

	result.r = fmin(255, c1.r + c2.r);
	result.g = fmin(255, c1.g + c2.g);
	result.b = fmin(255, c1.b + c2.b);
	return (result);
}

t_color	color_mul(t_color c1, t_color c2)
{
	t_color	result;

	result.r = (c1.r * c2.r) / 255;
	result.g = (c1.g * c2.g) / 255;
	result.b = (c1.b * c2.b) / 255;
	return (result);
}

t_vector	sphere_normal(t_vector point, t_sphere sphere)
{
	return (vec_normalize(vec_sub(point, sphere.center)));
}

t_vector	plane_normal(t_plane plane)
{
	return (plane.normal);
}

t_vector	cylinder_normal(t_vector point, t_cylinder cylinder)
{
	t_vector	cp;
	t_vector	projection;
	t_vector	normal;
	double		proj_len;
	t_vector	top_center;

	cp = vec_sub(point, cylinder.center);
	
	top_center = vec_add(cylinder.center, vec_mul(cylinder.axis, cylinder.height));
	
	if (vec_length(vec_sub(point, cylinder.center)) <= cylinder.radius + EPSILON &&
		fabs(vec_dot(vec_sub(point, cylinder.center), cylinder.axis)) < EPSILON)
		return (vec_mul(cylinder.axis, -1));
	
	if (vec_length(vec_sub(point, top_center)) <= cylinder.radius + EPSILON &&
		fabs(vec_dot(vec_sub(point, top_center), cylinder.axis)) < EPSILON)
		return (cylinder.axis);
	
	proj_len = vec_dot(cp, cylinder.axis);
	projection = vec_mul(cylinder.axis, proj_len);
	normal = vec_sub(cp, projection);
	
	return (vec_normalize(normal));
}

t_vector get_normal(t_object obj, t_vector hit_point)
{
    t_vector normal;
    
    if (obj.type == SPHERE)
        normal = sphere_normal(hit_point, obj.sphere);
    else if (obj.type == PLANE)
        normal = plane_normal(obj.plane);
    else if (obj.type == CYLINDER)
        normal = cylinder_normal(hit_point, obj.cylinder);
    // else if (obj.type == CONE)
    //     normal = cone_normal(hit_point, obj.cone);
    // else if (obj.type == HYPERBOLOID)
    //     normal = hyperboloid_normal(hit_point, obj.hyperboloid);
    // else if (obj.type == TRIANGLE)
    //     normal = triangle_normal(obj.triangle);
    
    else
        return ((t_vector){0, 0, 0});
    
    // Apply bump mapping if available
    if (obj.texture && obj.texture->has_bump_map)
        normal = bump_map_normal(obj, normal, hit_point);
    
    return normal;
}


int	is_in_shadow(t_scene *scene, t_vector point, t_vector light_dir, double light_dist)
{
	t_ray		shadow_ray;
	size_t		i;

	shadow_ray = ray_create(point, light_dir);
	shadow_ray.t = light_dist;

	i = 0;
	while (i < scene->obj_count)
	{
		if (intersect_object(&shadow_ray, scene->objects[i]))
			return (1);  // Object is blocking the light
		i++;
	}
	return (0);  // No objects blocking the light
}


t_color apply_checkerboard(t_color base_color, t_vector hit_point)
{
    int x = (int)(hit_point.x * 2);
    int y = (int)(hit_point.y * 2);
    int z = (int)(hit_point.z * 2);
    
    if ((x + y + z) % 2 == 0)
        return base_color;
    else
        return (t_color){
            base_color.r / 2,
            base_color.g / 2,
            base_color.b / 2
        };
}

t_color calculate_lighting(t_scene *scene, t_ray *ray, int obj_idx)
{
    t_vector    intersection_point;
    t_vector    normal;
    t_color     obj_color;
    t_color     ambient_color;
    t_color     diffuse_color;
    t_color     specular_color;
    t_vector    light_dir;
    t_vector    view_dir;
    t_vector    reflect_dir;
    double      diffuse_factor;
    double      specular_factor;
    size_t      i;
    t_color     color;
    double      light_dist;

    intersection_point = ray_at(*ray, ray->t);
    obj_color = scene->objects[obj_idx].color;
    
    if (scene->checkerboard)
        obj_color = apply_checkerboard(obj_color, intersection_point);
    
    ambient_color = color_scale(color_mul(scene->ambient.color, obj_color), 
                                scene->ambient.ratio);
    
    color = ambient_color;
    
    normal = get_normal(scene->objects[obj_idx], intersection_point);
    
    if (vec_dot(normal, ray->direction) > 0)
        normal = vec_mul(normal, -1);
    
    view_dir = vec_mul(ray->direction, -1);
    
    if (scene->objects[obj_idx].texture)
    {
        double u, v;
        calculate_uv(scene->objects[obj_idx], intersection_point, &u, &v);
        obj_color = get_texture_color(scene->objects[obj_idx].texture, u, v);
    }
    else
    {
        obj_color = scene->objects[obj_idx].color;
        
        if (scene->checkerboard)
            obj_color = apply_checkerboard(obj_color, intersection_point);
    }
    i = 0;
    while (i < scene->light_count)
    {
        light_dir = vec_sub(scene->lights[i].pos, intersection_point);
        light_dist = vec_length(light_dir);
        light_dir = vec_normalize(light_dir);
        
        if (!is_in_shadow(scene, vec_add(intersection_point, vec_mul(normal, EPSILON)), 
                          light_dir, light_dist))
        {
            diffuse_factor = fmax(0.0, vec_dot(normal, light_dir)) * scene->lights[i].brightness;
            
            reflect_dir = vec_sub(vec_mul(normal, 2 * vec_dot(normal, light_dir)), light_dir);
            reflect_dir = vec_normalize(reflect_dir);
            
            specular_factor = pow(fmax(0.0, vec_dot(view_dir, reflect_dir)), 
                                  scene->lights[i].specular_exp) * scene->lights[i].brightness;
            
            // diffuse_factor /= (1.0 + 0.01 * light_dist * light_dist);
            
            diffuse_color = color_scale(color_mul(scene->lights[i].color, obj_color), 
                                        diffuse_factor);
            
            specular_color = color_scale(scene->lights[i].color, specular_factor * 0.5);
            
            color = color_add(color, color_add(diffuse_color, specular_color));
        }
        i++;
    }
    
    return (color);
}

uint32_t	ray_get_color(t_scene *scene, t_ray *ray)
{
	size_t	i;
	int		hit_index;
	t_color	color;

	hit_index = -1;
	i = 0;
	while (i < scene->obj_count)
	{
		if (intersect_object(ray, scene->objects[i]))
			hit_index = i;
		i++;
	}
	
	if (hit_index == -1)
		return (0);  // Background color (black)
	
	color = calculate_lighting(scene, ray, hit_index);
	return (color_to_int(color));
}