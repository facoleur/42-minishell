/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_copy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lferro <lferro@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/28 22:12:43 by Lu-ni             #+#    #+#             */
/*   Updated: 2024/04/30 20:31:29 by lferro           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env/env.h"

char** copy_env(char** env, int size)
{
	int	   env_size;
	int	   i;
	char** new_env;

	env_size = 0;
	while (env && env[env_size])
		env_size++;
	new_env = palloc(env_size + size + 1, sizeof(char*));
	if (!new_env)
		return (0);
	i = 0;
	while (env && env[i])
	{
		new_env[i] = ft_strdup(env[i]);
		i++;
	}
	new_env[i] = 0;
	return (new_env);
}

char** get_new_shlvl(char** env)
{
	int	   shlvl;
	char*  shlvl_str;
	char*  new_shlvl_string;
	char** new_shlvl;

	new_shlvl	 = palloc(3, sizeof(char*));
	new_shlvl[0] = ft_strdup("export");
	shlvl_str	 = ft_getenv("SHLVL", env);
	if (!shlvl_str)
		new_shlvl[1] = ft_strdup("SHLVL=1");
	else
	{
		shlvl			 = ft_atoi(shlvl_str);
		new_shlvl_string = ft_itoa(shlvl + 1);
		new_shlvl[1]	 = ft_strjoin("SHLVL=", new_shlvl_string);
		free(new_shlvl_string);
	}
	new_shlvl[2] = NULL;
	return (new_shlvl);
}
