#include "../includes/minirt.h"

int	read_map(t_scene *scene, int fd)
{
	char	*line;

	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break;
		
		int len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
		
		if (!parse_line(scene, line))
		{
			free(line);
			close(fd);
			ft_putstr_fd("Error: Invalid scene file format\n", 2);
			return (0);
		}
		free(line);
	}
	close(fd);
	
	if (scene->obj_count == 0)
	{
		ft_putstr_fd("Error: No objects in scene\n", 2);
		return (0);
	}
	return (1);

}


int	parse_ambient(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2])
		return (0);
	
	scene->ambient.ratio = ft_atof(parts[1]);
	if (scene->ambient.ratio < 0 || scene->ambient.ratio > 1)
		return (0);
	
	scene->ambient.color = parse_color(parts[2]);
	return (1);
}

int	parse_camera(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3])
		return (0);
	
	scene->camera.pos = parse_vector(parts[1]);
	scene->camera.dir = parse_vector(parts[2]);
	
	scene->camera.dir = vec_normalize(scene->camera.dir);
	
	if (scene->camera.dir.x == 0 && scene->camera.dir.y == 0 && scene->camera.dir.z == 0)
		return (0);
	
	scene->camera.fov = ft_atof(parts[3]);
	if (scene->camera.fov <= 0 || scene->camera.fov >= 180)
		return (0);
	
	return (1);
}


int parse_light(t_scene *scene, char **parts)
{
    if (!parts[1] || !parts[2] || !parts[3] || scene->light_count >= 3)
        return (0);
    
    scene->lights[scene->light_count].pos = parse_vector(parts[1]);
    scene->lights[scene->light_count].brightness = ft_atof(parts[2]);
    
    if (scene->lights[scene->light_count].brightness < 0 || 
        scene->lights[scene->light_count].brightness > 1)
        return (0);
    
    scene->lights[scene->light_count].color = parse_color(parts[3]);
    
    scene->lights[scene->light_count].specular_exp = 32.0;
    
    if (parts[4])
        scene->lights[scene->light_count].specular_exp = ft_atof(parts[4]);
    
    scene->light_count++;
    return (1);
}


int	parse_sphere(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3] || scene->obj_count >= MAX_OBJECTS)
		return (0);
	
	scene->objects[scene->obj_count].type = SPHERE;
	scene->objects[scene->obj_count].sphere.center = parse_vector(parts[1]);
	scene->objects[scene->obj_count].sphere.diameter = ft_atof(parts[2]);
	
	if (scene->objects[scene->obj_count].sphere.diameter <= 0)
		return (0);
	
	scene->objects[scene->obj_count].sphere.radius = 
		scene->objects[scene->obj_count].sphere.diameter / 2.0;
	scene->objects[scene->obj_count].color = parse_color(parts[3]);

	if (parts[4])
	{
		ft_putstr_fd("Attempting to load texture: ", 1);
		ft_putendl_fd(parts[4], 1);
		scene->objects[scene->obj_count].texture = load_texture(parts[4]);
		if (!scene->objects[scene->obj_count].texture)
			ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
	}
	else
		scene->objects[scene->obj_count].texture = NULL;

	scene->obj_count++;
	return (1);
}


int	parse_plane(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3] || scene->obj_count >= MAX_OBJECTS)
		return (0);
	
	scene->objects[scene->obj_count].type = PLANE;
	scene->objects[scene->obj_count].plane.point = parse_vector(parts[1]);
	scene->objects[scene->obj_count].plane.normal = parse_vector(parts[2]);
	
	scene->objects[scene->obj_count].plane.normal = 
		vec_normalize(scene->objects[scene->obj_count].plane.normal);
	
	if (scene->objects[scene->obj_count].plane.normal.x == 0 && 
		scene->objects[scene->obj_count].plane.normal.y == 0 && 
		scene->objects[scene->obj_count].plane.normal.z == 0)
		return (0);
	
	scene->objects[scene->obj_count].color = parse_color(parts[3]);

	if (parts[4])
	{
		ft_putstr_fd("Attempting to load texture: ", 1);
		ft_putendl_fd(parts[4], 1);
		scene->objects[scene->obj_count].texture = load_texture(parts[4]);
		if (!scene->objects[scene->obj_count].texture)
			ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
	}
	else
		scene->objects[scene->obj_count].texture = NULL;

	scene->obj_count++;
	return (1);
}

