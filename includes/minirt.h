#ifndef MINIRT_H
# define MINIRT_H
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>

# include <unistd.h>
# include "MLX42/include/MLX42/MLX42.h"


# define MAX_OBJECTS 100
# define MAX_LIGHTS 100

typedef struct s_color
{
    int r;
    int g;
    int b;
} t_color;

typedef struct s_vector
{
    double x;
    double y;
    double z;
} t_vector;

typedef struct s_light
{
	t_vector	pos;
	double		brightness;
	t_color		color;
	double		specular_exp; // Phong specular exponent
}	t_light;


// The following structs are used to represent different geometric shapes in the scene.
typedef struct s_sphere
{
	t_vector	center;
	double		diameter;
	double		radius;
}	t_sphere;

typedef struct s_plane
{
	t_vector	point;
	t_vector	normal;
}	t_plane;

typedef struct s_cylinder
{
	t_vector	center;
	t_vector	axis;
	double		diameter;
	double		radius;
	double		height;
}	t_cylinder;

typedef struct s_texture
{
    int         width;
    int         height;
    uint32_t    *data;
    uint32_t    *bump_map;
    int         has_bump_map;
}   t_texture;

// Each shape has its own specific properties, such as center, diameter, radius, etc.
// The t_object struct is a union that can hold any of the above shapes.
typedef struct s_object
{
    t_color color;
    union 
    {
        t_plane plane;
        t_sphere sphere;
        t_cylinder cylinder;
    };
	t_texture *texture;
} t_object;



typedef struct s_ambient
{
	double	ratio;
	t_color	color;
}	t_ambient;

typedef struct s_camera
{
	t_vector	pos;
	t_vector	dir;
	double		fov;
}	t_camera;


typedef struct s_canvas
{
	size_t	w;
	size_t	h;
}	t_canvas;

typedef struct s_viewport
{
	double	width;
	double	height;
	double	dist;
}	t_viewport;

typedef struct s_scene
{
	t_canvas	canvas;
	t_camera	camera;
	t_ambient	ambient;
	t_object	*objects;
	t_light		*lights;
	size_t		obj_count;
	size_t		light_count;
	t_viewport	viewport;
	int			checkerboard; // Optional checkerboard toggle
}	t_scene;

# endif