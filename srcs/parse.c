/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: htaheri <htaheri@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:00:00 by user              #+#    #+#             */
/*   Updated: 2025/06/20 16:45:08 by htaheri          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minirt.h"

/**
 * Parse ambient lighting information
 */
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

/**
 * Parse camera information
 */
int	parse_camera(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3])
		return (0);
	
	scene->camera.pos = parse_vector(parts[1]);
	scene->camera.dir = parse_vector(parts[2]);
	
	// Normalize direction vector
	scene->camera.dir = vec_normalize(scene->camera.dir);
	
	// Check if direction vector is valid
	if (scene->camera.dir.x == 0 && scene->camera.dir.y == 0 && scene->camera.dir.z == 0)
		return (0);
	
	scene->camera.fov = ft_atof(parts[3]);
	if (scene->camera.fov <= 0 || scene->camera.fov >= 180)
		return (0);
	
	return (1);
}

/**
 * Parse light information
 */
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
    
    // Set default specular exponent if not provided
    scene->lights[scene->light_count].specular_exp = 32.0;
    
    // Check if specular exponent is provided
    if (parts[4])
        scene->lights[scene->light_count].specular_exp = ft_atof(parts[4]);
    
    scene->light_count++;
    return (1);
}

/**
 * Parse sphere information
 */
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

	// Optional texture support
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

/**
 * Parse plane information
 */
int	parse_plane(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3] || scene->obj_count >= MAX_OBJECTS)
		return (0);
	
	scene->objects[scene->obj_count].type = PLANE;
	scene->objects[scene->obj_count].plane.point = parse_vector(parts[1]);
	scene->objects[scene->obj_count].plane.normal = parse_vector(parts[2]);
	
	// Normalize the normal vector
	scene->objects[scene->obj_count].plane.normal = 
		vec_normalize(scene->objects[scene->obj_count].plane.normal);
	
	// Check if normal vector is valid
	if (scene->objects[scene->obj_count].plane.normal.x == 0 && 
		scene->objects[scene->obj_count].plane.normal.y == 0 && 
		scene->objects[scene->obj_count].plane.normal.z == 0)
		return (0);
	
	scene->objects[scene->obj_count].color = parse_color(parts[3]);

	// Optional texture support
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

/**
 * Parse cylinder information
 */
int	parse_cylinder(t_scene *scene, char **parts)
{
	if (!parts[1] || !parts[2] || !parts[3] || !parts[4] || !parts[5] || scene->obj_count >= MAX_OBJECTS)
		return (0);
	
	scene->objects[scene->obj_count].type = CYLINDER;
	scene->objects[scene->obj_count].cylinder.center = parse_vector(parts[1]);
	scene->objects[scene->obj_count].cylinder.axis = parse_vector(parts[2]);
	
	// Normalize the axis vector
	scene->objects[scene->obj_count].cylinder.axis = 
		vec_normalize(scene->objects[scene->obj_count].cylinder.axis);
	
	// Check if axis vector is valid
	if (scene->objects[scene->obj_count].cylinder.axis.x == 0 && 
		scene->objects[scene->obj_count].cylinder.axis.y == 0 && 
		scene->objects[scene->obj_count].cylinder.axis.z == 0)
		return (0);
	
	scene->objects[scene->obj_count].cylinder.diameter = ft_atof(parts[3]);
	if (scene->objects[scene->obj_count].cylinder.diameter <= 0)
		return (0);
	
	scene->objects[scene->obj_count].cylinder.radius = 
		scene->objects[scene->obj_count].cylinder.diameter / 2.0;
	
	scene->objects[scene->obj_count].cylinder.height = ft_atof(parts[4]);
	if (scene->objects[scene->obj_count].cylinder.height <= 0)
		return (0);
	
	scene->objects[scene->obj_count].color = parse_color(parts[5]);

	// Optional texture support
	if (parts[6])
	{
		ft_putstr_fd("Attempting to load texture: ", 1);
		ft_putendl_fd(parts[6], 1);
		scene->objects[scene->obj_count].texture = load_texture(parts[6]);
		if (!scene->objects[scene->obj_count].texture)
			ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
	}
	else
		scene->objects[scene->obj_count].texture = NULL;

	scene->obj_count++;
	return (1);
}

/**
 * Parse cone information
 */
int parse_cone(t_scene *scene, char **parts)
{
    if (!parts[1] || !parts[2] || !parts[3] || !parts[4] || !parts[5] || scene->obj_count >= MAX_OBJECTS)
        return (0);
    
    scene->objects[scene->obj_count].type = CONE;
    scene->objects[scene->obj_count].cone.vertex = parse_vector(parts[1]);
    scene->objects[scene->obj_count].cone.axis = parse_vector(parts[2]);
    
    // Normalize the axis vector
    scene->objects[scene->obj_count].cone.axis = 
        vec_normalize(scene->objects[scene->obj_count].cone.axis);
    
    // Check if axis vector is valid
    if (scene->objects[scene->obj_count].cone.axis.x == 0 && 
        scene->objects[scene->obj_count].cone.axis.y == 0 && 
        scene->objects[scene->obj_count].cone.axis.z == 0)
        return (0);
    
    // Get angle in degrees and convert to radians
    double angle_deg = ft_atof(parts[3]);
    if (angle_deg <= 0 || angle_deg >= 180)
        return (0);
    
    scene->objects[scene->obj_count].cone.angle = (angle_deg * M_PI) / 180.0;
    
    scene->objects[scene->obj_count].cone.height = ft_atof(parts[4]);
    if (scene->objects[scene->obj_count].cone.height <= 0)
        return (0);
    
    scene->objects[scene->obj_count].color = parse_color(parts[5]);

    // Optional texture support
    if (parts[6])
    {
        ft_putstr_fd("Attempting to load texture: ", 1);
        ft_putendl_fd(parts[6], 1);
        scene->objects[scene->obj_count].texture = load_texture(parts[6]);
        if (!scene->objects[scene->obj_count].texture)
            ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
    }
    else
        scene->objects[scene->obj_count].texture = NULL;

    scene->obj_count++;
    return (1);
}

/**
 * Parse hyperboloid information
 */
int parse_hyperboloid(t_scene *scene, char **parts)
{
    if (!parts[1] || !parts[2] || !parts[3] || !parts[4] || !parts[5] || 
        !parts[6] || !parts[7] || scene->obj_count >= MAX_OBJECTS)
        return (0);
    
    scene->objects[scene->obj_count].type = HYPERBOLOID;
    scene->objects[scene->obj_count].hyperboloid.center = parse_vector(parts[1]);
    scene->objects[scene->obj_count].hyperboloid.axis = vec_normalize(parse_vector(parts[2]));

    if (scene->objects[scene->obj_count].hyperboloid.axis.x == 0 && 
        scene->objects[scene->obj_count].hyperboloid.axis.y == 0 && 
        scene->objects[scene->obj_count].hyperboloid.axis.z == 0)
        return (0);

    scene->objects[scene->obj_count].hyperboloid.a = ft_atof(parts[3]);
    scene->objects[scene->obj_count].hyperboloid.b = ft_atof(parts[4]);
    scene->objects[scene->obj_count].hyperboloid.c = ft_atof(parts[5]);
    scene->objects[scene->obj_count].hyperboloid.height = ft_atof(parts[6]);

    if (scene->objects[scene->obj_count].hyperboloid.a <= 0 ||
        scene->objects[scene->obj_count].hyperboloid.b <= 0 ||
        scene->objects[scene->obj_count].hyperboloid.c <= 0 ||
        scene->objects[scene->obj_count].hyperboloid.height <= 0)
        return (0);

    scene->objects[scene->obj_count].color = parse_color(parts[7]);

    // Optional texture support
    if (parts[8])
    {
        ft_putstr_fd("Attempting to load texture: ", 1);
        ft_putendl_fd(parts[8], 1);
        scene->objects[scene->obj_count].texture = load_texture(parts[8]);
        if (!scene->objects[scene->obj_count].texture)
            ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
    }
    else
        scene->objects[scene->obj_count].texture = NULL;

    scene->obj_count++;
    return (1);
}


/**
 * Parse a line from the .rt file
 */
int parse_line(t_scene *scene, char *line)
{
    char    **parts;
    int     result;
    int     i;

    if (!line || line[0] == '#' || line[0] == '\0')
        return (1);
    
    // Handle the new compact format
    if (strstr(line, " bum:") || strstr(line, " txm:"))
    {
        // Split the line carefully
        parts = malloc(sizeof(char *) * 10);  // Allocate space for multiple parts
        if (!parts)
            return (0);
        
        // Initialize to NULL
        for (i = 0; i < 10; i++)
            parts[i] = NULL;
        
        // Manual parsing for compact format
        parts[0] = strtok(line, " ");  // Object type
        if (parts[0] == NULL)
        {
            free(parts);
            return (0);
        }
        
        // Parse remaining parts
        for (i = 1; i < 9; i++)
        {
            parts[i] = strtok(NULL, " ");
            if (parts[i] == NULL)
                break;
        }
        
        // Determine object type and parse accordingly
        if (ft_strncmp(parts[0], "sp", 3) == 0)
            result = parse_sphere_compact(scene, parts);
        else
            result = 0;
    }
    else
    {
        // Original parsing method
        parts = ft_split(line, ' ');
        if (!parts || !parts[0])
        {
            if (parts)
                free(parts);
            return (0);
        }
        
        if (ft_strncmp(parts[0], "A", 2) == 0)
            result = parse_ambient(scene, parts);
        else if (ft_strncmp(parts[0], "C", 2) == 0)
            result = parse_camera(scene, parts);
        else if (ft_strncmp(parts[0], "L", 2) == 0)
            result = parse_light(scene, parts);
        else if (ft_strncmp(parts[0], "sp", 3) == 0)
            result = parse_sphere(scene, parts);
        else if (ft_strncmp(parts[0], "pl", 3) == 0)
            result = parse_plane(scene, parts);
        else if (ft_strncmp(parts[0], "cy", 3) == 0)
            result = parse_cylinder(scene, parts);
        else if (ft_strncmp(parts[0], "cn", 3) == 0)
            result = parse_cone(scene, parts);
        else if (ft_strncmp(parts[0], "hy", 3) == 0)
            result = parse_hyperboloid(scene, parts);
		else if (ft_strncmp(parts[0], "tr", 3) == 0)
			result = parse_triangle(scene, parts);
		
        else if (ft_strncmp(parts[0], "cb", 3) == 0)
        {
            scene->checkerboard = 1;
            result = 1;
        }
        else
            result = 0;
        
        // Free parts
        int i = 0;
        while (parts[i])
            free(parts[i++]);
        free(parts);
    }
    
    return (result);
}

int parse_triangle(t_scene *scene, char **parts)
{
    if (!parts[1] || !parts[2] || !parts[3] || !parts[4] || scene->obj_count >= MAX_OBJECTS)
        return (0);
    
    scene->objects[scene->obj_count].type = TRIANGLE;
    scene->objects[scene->obj_count].triangle.v1 = parse_vector(parts[1]);
    scene->objects[scene->obj_count].triangle.v2 = parse_vector(parts[2]);
    scene->objects[scene->obj_count].triangle.v3 = parse_vector(parts[3]);
    scene->objects[scene->obj_count].color = parse_color(parts[4]);

    // Calculate triangle normal
    t_vector e1 = vec_sub(scene->objects[scene->obj_count].triangle.v2, 
                           scene->objects[scene->obj_count].triangle.v1);
    t_vector e2 = vec_sub(scene->objects[scene->obj_count].triangle.v3, 
                           scene->objects[scene->obj_count].triangle.v1);
    scene->objects[scene->obj_count].triangle.normal = vec_normalize(vec_cross(e1, e2));

    // Optional texture support
    if (parts[5])
    {
        scene->objects[scene->obj_count].texture = load_texture(parts[5]);
        if (!scene->objects[scene->obj_count].texture)
            ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
    }
    else
        scene->objects[scene->obj_count].texture = NULL;

    scene->obj_count++;
    return (1);
}

// New function to parse compact sphere format
int parse_sphere_compact(t_scene *scene, char **parts)
{
    char *texture_path = NULL;
    char *bump_path = NULL;
    
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

    // Check for texture and bump map
    for (int i = 4; parts[i] != NULL; i++)
    {
        if (ft_strncmp(parts[i], "txm:", 4) == 0)
            texture_path = parts[i] + 4;
        else if (ft_strncmp(parts[i], "bum:", 4) == 0)
            bump_path = parts[i] + 4;
    }

    // Load texture if specified
    if (texture_path)
    {
        scene->objects[scene->obj_count].texture = load_texture(texture_path);
        if (!scene->objects[scene->obj_count].texture)
            ft_putendl_fd("Warning: Failed to load texture, continuing without it", 1);
    }
    else
        scene->objects[scene->obj_count].texture = NULL;

    scene->obj_count++;
    return (1);
}
/**
 * Read and parse the scene description file
 */
int	read_map(t_scene *scene, int fd)
{
	char	*line;

	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break;
		
		// Remove newline character if present
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
	
	// Verify that we have at least one object, ambient and camera
	if (scene->obj_count == 0)
	{
		ft_putstr_fd("Error: No objects in scene\n", 2);
		return (0);
	}
	return (1);

}
