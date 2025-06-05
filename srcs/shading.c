#include "../includes/minirt.h"


uint32_t	color_to_int(t_color color)
{
	return ((color.r << 24) | (color.g << 16) | (color.b << 8) | 255);
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