# include "../includes/minirt.h"

t_vector	ray_dir(t_scene *scene, size_t x, size_t y)
{
	t_vector	forward;
	t_vector	right;
	t_vector	up;
	t_vector	dir;
	double		u;
	double		v;

	u = (double)x / (scene->canvas.w - 1) - 0.5;
	v = 0.5 - (double)y / (scene->canvas.h - 1);
	
	forward = vec_normalize(scene->camera.dir);
	
	right = vec_normalize(vec_cross((t_vector){0, 1, 0}, forward));
	
	up = vec_cross(forward, right);
	
	dir = vec_add(forward, 
		vec_add(vec_mul(right, u * scene->viewport.width), 
				vec_mul(up, v * scene->viewport.height)));
	
	return (vec_normalize(dir));
}