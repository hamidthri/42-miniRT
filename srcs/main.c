# include "../includes/minirt.h"


void	render(mlx_t *mlx, t_scene *scene)
{
	size_t		i;
	size_t		j;
	mlx_image_t	*img;
	t_ray		ray;

	img = mlx_new_image(mlx, scene->canvas.w, scene->canvas.h);
	if (!img)
	{
		ft_putstr_fd("Error: Could not create image\n", 2);
		return;
	}
	i = 0;
	scene->viewport = viewport_dim(scene->canvas, scene->camera);
	while (i < scene->canvas.w)
	{
		j = 0;
		while (j < scene->canvas.h)
		{
			ray = (t_ray){scene->camera.pos, ray_dir(scene, i, j), INFINITY};
			mlx_put_pixel(img, i, j, ray_get_color(scene, &ray));
			j++;
		}
		i++;
	}
	mlx_image_to_window(mlx, img, 0, 0);
}

/**
 * Free resources and exit
 */
void cleanup_and_exit(t_scene *scene, mlx_t *mlx, int status)
{
    size_t i;
    
    if (mlx)
        mlx_terminate(mlx);
    
    if (scene->objects)
    {
        // Free textures
        for (i = 0; i < scene->obj_count; i++)
        {
            if (scene->objects[i].texture)
            {
                if (scene->objects[i].texture->data)
                    free(scene->objects[i].texture->data);
                if (scene->objects[i].texture->bump_map)
                    free(scene->objects[i].texture->bump_map);
                free(scene->objects[i].texture);
            }
        }
        free(scene->objects);
    }
    
    if (scene->lights)
        free(scene->lights);
    
    exit(status);
}

void init_scene(t_scene *scene)
{
    scene->obj_count = 0;
    scene->light_count = 0;
    scene->ambient.ratio = 0.1;
    scene->ambient.color = (t_color){255, 255, 255};
    scene->camera.pos = (t_vector){0, 0, 0};
    scene->camera.dir = (t_vector){0, 0, 1};
    scene->camera.fov = 70.0;
    scene->checkerboard = 0;  // Disabled by default
}

int	main(int argc, char **argv)
{
	int			fd;
	mlx_t		*mlx;
	t_object	*objs;
	t_light		*lights;
	t_scene		scene;

	if (argc != 2)
		return (ft_putstr_fd("Error: Invalid number of arguments\n", 2), 1);
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return (ft_putstr_fd("Error: Could not open file\n", 2), 1);
	
	// Allocate memory for objects and lights
	objs = malloc(sizeof(t_object) * MAX_OBJECTS);
	lights = malloc(sizeof(t_light) * MAX_LIGHTS);
	if (!objs || !lights)
	{
		if (objs)
			free(objs);
		if (lights)
			free(lights);
		return (ft_putstr_fd("Error: Memory allocation failed\n", 2), 1);
	}
	
	scene = (t_scene){.objects = objs, .lights = lights,
		.canvas = (t_canvas){1200, 800}, .obj_count = 0, .light_count = 0};
	init_scene(&scene);
	
	if (!read_map(&scene, fd))
		cleanup_and_exit(&scene, NULL, 1);
	
	mlx = mlx_init(scene.canvas.w, scene.canvas.h, "MiniRT", 1);
	if (!mlx)
	{
		ft_putstr_fd("Error: MLX initialization failed\n", 2);
		cleanup_and_exit(&scene, NULL, 1);
	}
	
	render(mlx, &scene);
	
	mlx_key_hook(mlx, key_hook, mlx);
	
	mlx_loop(mlx);
	
	cleanup_and_exit(&scene, mlx, 0);
	return (0);
}