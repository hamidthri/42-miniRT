# include "../includes/minirt.h"

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

int main(int argc, char **argv)
{
    int fd;
    t_object *objs;
    t_light *lights;
    t_scene scene;


    if (argc != 2)
        return (ft_putstr_fd("Error: Invalid number of arguments\n", 2), 1);
    
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return (ft_putstr_fd("Error: Failed to open file\n", 2), 1);
    
	// Allocate memory for objects and lights
    objs = malloc(sizeof(t_object) * MAX_OBJECTS);
    lights = malloc(sizeof(t_light) * MAX_LIGHTS);
    if (!objs || !lights)
    {
        if (objs)
            free(objs);
        if (lights)
            free(lights);
        close(fd);
        return (ft_putstr_fd("Error: Memory allocation failed\n", 2), 1);
    }

    // initialize the scene
    scene = (t_scene){.objects = objs, .lights = lights,
		.canvas = (t_canvas){600, 400}, .obj_count = 0, .light_count = 0};
    // Read the scene file and populate the scene structure
    init_scene(&scene);
    return(0);
}