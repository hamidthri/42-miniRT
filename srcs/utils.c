# include "../includes/minirt.h"

t_viewport	viewport_dim(t_canvas canvas, t_camera camera)
{
	t_viewport	viewport;
	double		aspect_ratio;

	aspect_ratio = (double)canvas.w / (double)canvas.h;
	viewport.dist = 1.0;
	viewport.height = 2.0 * viewport.dist * tan(camera.fov * M_PI / 360.0);
	viewport.width = viewport.height * aspect_ratio;
	return (viewport);
}

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

double	ft_atof(const char *str)
{
	int		sign;
	double	result;
	double	decimal;
	int		decimal_places;

	sign = 1;
	result = 0.0;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
		result = result * 10 + (*str++ - '0');
	if (*str == '.')
	{
		str++;
		decimal = 0.0;
		decimal_places = 0;
		while (*str >= '0' && *str <= '9')
		{
			decimal = decimal * 10 + (*str++ - '0');
			decimal_places++;
		}
		result += decimal / pow(10, decimal_places);
	}
	return (result * sign);
}

t_vector	parse_vector(char *str)
{
	t_vector	vec;
	char		**parts;

	parts = ft_split(str, ',');
	if (!parts || !parts[0] || !parts[1] || !parts[2] || parts[3])
	{
		if (parts)
		{
			free(parts[0]);
			free(parts[1]);
			free(parts[2]);
			free(parts);
		}
		return ((t_vector){0, 0, 0});
	}
	vec.x = ft_atof(parts[0]);
	vec.y = ft_atof(parts[1]);
	vec.z = ft_atof(parts[2]);
	free(parts[0]);
	free(parts[1]);
	free(parts[2]);
	free(parts);
	return (vec);
}

t_color	parse_color(char *str)
{
	t_color	color;
	char	**parts;

	parts = ft_split(str, ',');
	if (!parts || !parts[0] || !parts[1] || !parts[2] || parts[3])
	{
		if (parts)
		{
			free(parts[0]);
			free(parts[1]);
			free(parts[2]);
			free(parts);
		}
		return ((t_color){0, 0, 0});
	}
	color.r = ft_atoi(parts[0]);
	color.g = ft_atoi(parts[1]);
	color.b = ft_atoi(parts[2]);
	free(parts[0]);
	free(parts[1]);
	free(parts[2]);
	free(parts);
	return (color);
}

void	key_hook(mlx_key_data_t data, void *param)
{
	mlx_t	*mlx;

	mlx = (mlx_t *)param;
	if (data.key == MLX_KEY_ESCAPE && data.action == MLX_PRESS)
		mlx_close_window(mlx);
}
