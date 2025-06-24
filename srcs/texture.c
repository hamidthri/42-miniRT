#include "../includes/minirt.h"

void calculate_uv(t_object obj, t_vector point, double *u, double *v)
{
    t_vector dir;
    
    if (obj.type == SPHERE)
    {
        dir = vec_normalize(vec_sub(point, obj.sphere.center));
        *u = 0.5 + atan2(dir.z, dir.x) / (2 * M_PI);
        *v = 0.5 - asin(dir.y) / M_PI;
    }
    else if (obj.type == PLANE)
    {
        t_vector x_axis, y_axis;
        t_vector local_point;
        
        if (fabs(obj.plane.normal.y) > 0.9)
            x_axis = vec_normalize((t_vector){1, 0, 0});
        else
            x_axis = vec_normalize(vec_cross((t_vector){0, 1, 0}, obj.plane.normal));
        
        y_axis = vec_normalize(vec_cross(obj.plane.normal, x_axis));
        
        local_point = vec_sub(point, obj.plane.point);
        *u = fmod(vec_dot(local_point, x_axis) * 0.1, 1.0);
        *v = fmod(vec_dot(local_point, y_axis) * 0.1, 1.0);
        
        if (*u < 0) *u += 1.0;
        if (*v < 0) *v += 1.0;
    }
    else if (obj.type == CYLINDER)
    {
        t_vector cp = vec_sub(point, obj.cylinder.center);
        t_vector proj = vec_mul(obj.cylinder.axis, vec_dot(cp, obj.cylinder.axis));
        t_vector local_point = vec_sub(cp, proj);
        
        *u = 0.5 + atan2(local_point.z, local_point.x) / (2 * M_PI);
        *v = fmod(vec_length(proj) / obj.cylinder.height, 1.0);
    }
    else if (obj.type == CONE)
    {
        t_vector vp = vec_sub(point, obj.cone.vertex);
        double height = vec_dot(vp, obj.cone.axis);
        t_vector proj = vec_mul(obj.cone.axis, height);
        t_vector local_point = vec_sub(vp, proj);
        
        *u = 0.5 + atan2(local_point.z, local_point.x) / (2 * M_PI);
        *v = height / obj.cone.height;
    }
    else if (obj.type == HYPERBOLOID)
    {
        // Hyperboloid mapping
        t_vector cp = vec_sub(point, obj.hyperboloid.center);
        
        *u = 0.5 + atan2(cp.z, cp.x) / (2 * M_PI);
        *v = 0.5 + cp.y / obj.hyperboloid.height;
    }
    else
    {
        *u = 0;
        *v = 0;
    }
}

t_color get_texture_color(t_texture *texture, double u, double v)
{
    int x, y;
    uint32_t pixel;
    t_color color;
    
    if (!texture || !texture->data)
        return ((t_color){255, 255, 255});
    
    x = (int)(u * (texture->width - 1)) % texture->width;
    y = (int)(v * (texture->height - 1)) % texture->height;
    
    if (x < 0) x += texture->width;
    if (y < 0) y += texture->height;
    
    pixel = texture->data[y * texture->width + x];
    
    color.r = (pixel >> 24) & 0xFF;
    color.g = (pixel >> 16) & 0xFF;
    color.b = (pixel >> 8) & 0xFF;
    
    return (color);
}

t_vector bump_map_normal(t_object obj, t_vector normal, t_vector point)
{
    double u, v;
    int x, y;
    uint32_t bump;
    t_vector tangent, bitangent;
    t_vector bump_normal;
    double bump_strength = 0.05;
    
    if (!obj.texture || !obj.texture->bump_map)
        return (normal);
    
    calculate_uv(obj, point, &u, &v);
    
    x = (int)(u * (obj.texture->width - 1)) % obj.texture->width;
    y = (int)(v * (obj.texture->height - 1)) % obj.texture->height;
    
    if (x < 0) x += obj.texture->width;
    if (y < 0) y += obj.texture->height;
    
    bump = obj.texture->bump_map[y * obj.texture->width + x];
    
    // Calculate tangent and bitangent vectors
    if (fabs(normal.x) > 0.9)
        tangent = vec_normalize((t_vector){0, 1, 0});
    else
        tangent = vec_normalize(vec_cross((t_vector){1, 0, 0}, normal));
    
    bitangent = vec_normalize(vec_cross(normal, tangent));
    
    // Calculate displaced normal
    bump_normal = normal;
    bump_normal = vec_add(bump_normal, vec_mul(tangent, ((double)((bump >> 16) & 0xFF) / 128.0 - 1.0) * bump_strength));
    bump_normal = vec_add(bump_normal, vec_mul(bitangent, ((double)((bump >> 8) & 0xFF) / 128.0 - 1.0) * bump_strength));
    
    return (vec_normalize(bump_normal));
}

char *fix_texture_path(char *path)
{
    if (!path)
        return NULL;
    
    char *path_variations[] = {
        path,  // Original path
        ft_strjoin("textures/", path),  // Textures directory
        ft_strjoin("imgs/", path),      // Images directory
        NULL
    };
    
    for (int i = 0; path_variations[i] != NULL; i++)
    {
        char *png_path = malloc(ft_strlen(path_variations[i]) + 5);  // +5 for ".png" and null terminator
        if (png_path)
        {
            ft_strlcpy(png_path, path_variations[i], PATH_MAX);
            
            char *dot = ft_strrchr(png_path, '.');
            if (dot)
                *dot = '\0';
            
            ft_strlcat(png_path, ".png", PATH_MAX);

            if (i > 0)
                free(path_variations[i]);
            
            FILE *test_file = fopen(png_path, "r");
            if (test_file)
            {
                fclose(test_file);
                return png_path;
            }
            free(png_path);
        }
    }
    
    return NULL;
}

t_texture *load_texture(char *path)
{
    t_texture *texture;
    mlx_texture_t *mlx_texture;
    int i, j;
    char *fixed_path;
    char *file_ext;
    
    if (!path)
        return (NULL);
    
    ft_putstr_fd("Loading texture: ", 1);
    ft_putendl_fd(path, 1);
    
    fixed_path = fix_texture_path(path);
    if (!fixed_path)
        return (NULL);
    
    file_ext = ft_strrchr(fixed_path, '.');
    if (!file_ext)
    {
        ft_putstr_fd("Error: No file extension in texture path: ", 2);
        ft_putendl_fd(fixed_path, 2);
        free(fixed_path);
        return (NULL);
    }
    
    for (i = 0; file_ext[i]; i++)
        file_ext[i] = ft_tolower(file_ext[i]);
    
    if (ft_strcmp(file_ext, ".png") != 0)
    {
        ft_putstr_fd("Error: Unsupported texture format. Only PNG files are supported: ", 2);
        ft_putendl_fd(fixed_path, 2);
        free(fixed_path);
        return (NULL);
    }
    
    texture = malloc(sizeof(t_texture));
    if (!texture)
    {
        free(fixed_path);
        return (NULL);
    }
    
    texture->data = NULL;
    texture->bump_map = NULL;
    texture->has_bump_map = 0;
    
    mlx_texture = mlx_load_png(fixed_path);
    if (!mlx_texture)
    {
        ft_putstr_fd("Error: Failed to load texture (possible reasons: file not found, corrupted, or not a valid PNG): ", 2);
        ft_putendl_fd(fixed_path, 2);
        free(texture);
        free(fixed_path);
        return (NULL);
    }
    
    texture->width = mlx_texture->width;
    texture->height = mlx_texture->height;
    
    texture->data = malloc(sizeof(uint32_t) * texture->width * texture->height);
    if (!texture->data)
    {
        mlx_delete_texture(mlx_texture);
        free(texture);
        free(fixed_path);
        return (NULL);
    }
    
    for (i = 0; i < texture->height; i++)
    {
        for (j = 0; j < texture->width; j++)
        {
            uint8_t *pixel = &mlx_texture->pixels[(i * texture->width + j) * 4];
            texture->data[i * texture->width + j] = (pixel[0] << 24) | (pixel[1] << 16) | (pixel[2] << 8) | pixel[3];
        }
    }
    
    char *bump_path = malloc(ft_strlen(fixed_path) + 6);
    if (bump_path)
    {
        ft_strlcpy(bump_path, fixed_path, PATH_MAX);
        char *dot = ft_strrchr(bump_path, '.');
        if (dot)
            ft_strlcpy(dot, "_bump.png", PATH_MAX);

        
        mlx_texture_t *bump_texture = mlx_load_png(bump_path);
        
        if (bump_texture)
        {
            texture->has_bump_map = 1;
            texture->bump_map = malloc(sizeof(uint32_t) * texture->width * texture->height);
            
            if (!texture->bump_map)
            {
                texture->has_bump_map = 0;
            }
            else
            {
                for (i = 0; i < texture->height; i++)
                {
                    for (j = 0; j < texture->width; j++)
                    {
                        uint8_t *pixel = &bump_texture->pixels[(i * texture->width + j) * 4];
                        texture->bump_map[i * texture->width + j] = (pixel[0] << 24) | (pixel[1] << 16) | (pixel[2] << 8) | pixel[3];
                    }
                }
            }
            mlx_delete_texture(bump_texture);
        }
        else
        {
            texture->has_bump_map = 0;
            texture->bump_map = NULL;
        }
        
        free(bump_path);
    }
    
    mlx_delete_texture(mlx_texture);
    free(fixed_path);
    
    ft_putendl_fd("Texture loaded successfully", 1);
    return (texture);
}