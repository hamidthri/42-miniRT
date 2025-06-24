#ifndef MINIRT_H
# define MINIRT_H
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <math.h>
#include "../libft/libft.h"
#include <limits.h>
#include <string.h>  // for strtok




# include <unistd.h>
# include "MLX42/include/MLX42/MLX42.h"


# define MAX_OBJECTS 10000
# define MAX_LIGHTS 10000
# define EPSILON 0.0001



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

typedef struct s_ray
{
	t_vector	origin;
	t_vector	direction;
	double		t;
}	t_ray;


typedef struct s_light
{
	t_vector	pos;
	double		brightness;
	t_color		color;
	double		specular_exp; // Phong specular exponent
}	t_light;

typedef enum e_object_type
{
	SPHERE,
	PLANE,
	CYLINDER,
	CONE,
	HYPERBOLOID,
	TRIANGLE
}	t_object_type;


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

typedef struct s_triangle
{
    t_vector    v1;
    t_vector    v2;
    t_vector    v3;
    t_vector    normal;
}   t_triangle;


typedef struct s_cone
{
	t_vector	vertex;
	t_vector	axis;
	double		angle;
	double		height;
}	t_cone;

typedef struct s_hyperboloid
{
    t_vector    center;
    t_vector    axis;
    double      a;
    double      b;
    double      c;
    double      height;
}   t_hyperboloid;

typedef struct s_texture
{
    int         width;
    int         height;
    uint32_t    *data;
    uint32_t    *bump_map;
    int         has_bump_map;
}   t_texture;


typedef struct s_object
{
	t_object_type	type;
	t_color			color;
	union
	{
		t_sphere	sphere;
		t_plane		plane;
		t_cylinder	cylinder;
		t_cone		cone;
		t_hyperboloid hyperboloid;
		t_triangle	triangle;
		
	};
	t_texture        *texture;      // Add this for bump mapping

}	t_object;



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

/* ==== Vector Ops ==== */
t_vector	vec_add(t_vector v1, t_vector v2);
t_vector	vec_sub(t_vector v1, t_vector v2);
t_vector	vec_mul(t_vector v, double t);
t_vector	vec_div(t_vector v, double t);
double		vec_dot(t_vector v1, t_vector v2);
double		vec_length(t_vector v);
t_vector	vec_normalize(t_vector v);
t_vector	vec_cross(t_vector v1, t_vector v2);

/* ==== Ray Tracing ==== */
t_ray		ray_create(t_vector origin, t_vector direction);
t_vector	ray_at(t_ray ray, double t);
t_vector	ray_dir(t_scene *scene, size_t x, size_t y);
uint32_t	ray_get_color(t_scene *scene, t_ray *ray);

/* ==== Scene ==== */
t_viewport	viewport_dim(t_canvas canvas, t_camera camera);
int			read_map(t_scene *scene, int fd);
int			parse_line(t_scene *scene, char *line);
int			parse_ambient(t_scene *scene, char **parts);
int			parse_camera(t_scene *scene, char **parts);
int			parse_light(t_scene *scene, char **parts);
int			parse_sphere(t_scene *scene, char **parts);
int			parse_plane(t_scene *scene, char **parts);

/* ==== Utils ==== */
double		ft_atof(const char *str);
t_vector	parse_vector(char *str);
t_color		parse_color(char *str);
void		key_hook(mlx_key_data_t data, void *param);
t_color		apply_checkerboard(t_color base_color, t_vector hit_point); // Optional

t_texture    *load_texture(char *path);
t_vector     bump_map_normal(t_object obj, t_vector normal, t_vector point);
t_color      get_texture_color(t_texture *texture, double u, double v);
int 		intersect_hyperboloid(t_ray *ray, t_hyperboloid hyp);
t_vector    hyperboloid_normal(t_vector hit_point, t_hyperboloid hyp);
void        calculate_uv(t_object obj, t_vector point, double *u, double *v);
int parse_hyperboloid(t_scene *scene, char **parts);
int solve_quadratic(double coeffs[3], double *t1, double *t2);

/* ==== Intersections ==== */
int			intersect_sphere(t_ray *ray, t_sphere sphere);
int			intersect_plane(t_ray *ray, t_plane plane);
int			intersect_cylinder(t_ray *ray, t_cylinder cylinder);
int			intersect_cone(t_ray *ray, t_cone cone);
int			intersect_object(t_ray *ray, t_object obj);


/* ==== Lighting and Colors ==== */
t_color		calculate_lighting(t_scene *scene, t_ray *ray, int obj_idx);
int			is_in_shadow(t_scene *scene, t_vector point, t_vector light_dir, double light_dist);
uint32_t	color_to_int(t_color color);
t_color		color_scale(t_color color, double scale);
t_color		color_add(t_color c1, t_color c2);
t_color		color_mul(t_color c1, t_color c2);


int parse_sphere_compact(t_scene *scene, char **parts);


int     intersect_triangle(t_ray *ray, t_triangle triangle);
t_vector triangle_normal(t_triangle triangle);
int     parse_triangle(t_scene *scene, char **parts);


# endif