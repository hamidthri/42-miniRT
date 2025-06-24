/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmomeni <mmomeni@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 19:57:18 by mmomeni           #+#    #+#             */
/*   Updated: 2024/03/18 20:37:59 by mmomeni          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

float	ft_atof(const char *str)
{
	float	n;
	float	neg;
	float	dec;
	float	dec_place;

	n = 0;
	neg = 1;
	dec = 0;
	dec_place = 0.1;
	while ((*str >= 9 && *str <= 13) || *str == 32)
		str++;
	if (*str == '+' || *str == '-')
		if (*str++ == '-')
			neg = -1;
	while (*str >= '0' && *str <= '9')
		n = (n * 10) + ((*str++) - 48);
	if (*str == '.')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		dec += ((*str++) - 48) * dec_place;
		dec_place *= 0.1;
	}
	return ((n + dec) * neg);
}
