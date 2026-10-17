/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnicolli <lnicolli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:42 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/29 14:34:45 by lnicolli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common/common.h"

/**
 * @brief get the number of element in a 3d array
 *
 * @param array3d 3d array
 * @return size of the array
 */
int get_3darray_size(char*** array3d)
{
	int i;
	int j;
	int size;

	i	 = 0;
	j	 = 0;
	size = 0;
	while (array3d[i])
	{
		while (array3d[i][j++])
			size++;
		j = 0;
		i++;
	}
	return (size);
}

char** flatten_3d_array(char*** array3d)
{
	char** arr_flatten;
	int	   i;
	int	   j;
	int	   k;
	int	   array3d_size;

	i			 = 0;
	j			 = 0;
	k			 = 0;
	array3d_size = get_3darray_size(array3d);
	arr_flatten	 = palloc(array3d_size, sizeof(char*));
	while (array3d[i])
	{
		while (array3d[i][j])
		{
			arr_flatten[k++] = ft_strdup(array3d[i][j]);
			free(array3d[i][j++]);
		}
		free(array3d[i++]);
		j = 0;
	}
	arr_flatten[k] = 0;
	free(array3d);
	return (arr_flatten);
}

int count_arr_elems(char** array)
{
	int i;

	i = 0;
	while (array[i])
		i++;
	return (i);
}

char* flatten_2d_array(char** array)
{
	char* res;
	int	  i;
	int	  j;
	int	  k;

	i = -1;
	j = 0;
	k = 0;
	while (array[++i])
	{
		while (array[i][j++])
			k++;
		j = 0;
	}
	res = palloc(k, sizeof(char));
	i	= -1;
	k	= 0;
	while (array[++i])
	{
		while (array[i][j])
			res[k++] = array[i][j++];
		j = 0;
	}
	res[k] = 0;
	return (res);
}

char** free_2starchar(char** strstr)
{
	int i;

	i = 0;
	if (!strstr)
		return (NULL);
	while (strstr[i])
		free(strstr[i++]);
	free(strstr);
	strstr = NULL;
	return (NULL);
}
