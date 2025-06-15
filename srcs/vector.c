#include "../includes/minirt.h"

t_vector	vec_add(t_vector v1, t_vector v2)
{
	t_vector	result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return (result);
}

t_vector	vec_sub(t_vector v1, t_vector v2)
{
	t_vector	result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return (result);
}

t_vector	vec_mul(t_vector v, double t)
{
	t_vector	result;

	result.x = v.x * t;
	result.y = v.y * t;
	result.z = v.z * t;
	return (result);
}

t_vector	vec_div(t_vector v, double t)
{
	t_vector	result;

	if (t == 0)
		return (v);
	result.x = v.x / t;
	result.y = v.y / t;
	result.z = v.z / t;
	return (result);
}

double	vec_dot(t_vector v1, t_vector v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

double	vec_length(t_vector v)
{
	return (sqrt(vec_dot(v, v)));
}

t_vector	vec_normalize(t_vector v)
{
	double	len;

	len = vec_length(v);
	if (len == 0)
		return (v);
	return (vec_div(v, len));
}

t_vector	vec_cross(t_vector v1, t_vector v2)
{
	t_vector	result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return (result);
}
